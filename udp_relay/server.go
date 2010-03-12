package main
import (
    "net"
    "os"
    "fmt"
    "encoding/binary"
    "bytes"
    "flag"
)

func handle_err(err os.Error) {
    fmt.Println(err)
    os.Exit(1)
}

type iperfHeader struct {
    Id int32;
    Second int32;
    Usecond int32
}

type record struct {
    Id int32;
    Length int32;
    SecondSend int32;
    USecondSend int32;
    SecondRecv  int64;
    USecondRecv int64;
}

type report struct {
    totalCount int32;
    receivedCount int32;
    lossrate      float64;
    jitter        float64;
    bandwidth     int64;
    totalTime     int64;
}

func outputResult (channel chan *record, finish chan *report) {
    var (
        maxId int32 = 0
        prevId int32 = 0
        count int32 = 0
        maxDelay int64 = -999999999999999999 
        minDelay int64 = 1000000000000000000
        delay    int64 = 0
        startTime int64 = 0
        endTime   int64 = 0
        totalFlow int64 = 0
        jitter    float64 = 0
    )

    //jitter computation follows RFC1889
    for record := range channel {
        if record.Id < 0 {
            maxId = -record.Id
            if maxId <= prevId + 1 {
                maxId = prevId + 1
            }
            continue
        } else {
            prevId = record.Id
        }
        if count == 0 {
            startTime = int64(record.SecondRecv * 1e6 + record.USecondRecv)
        }
        endTime = int64(record.SecondRecv * 1e6 + record.USecondRecv)

        totalFlow += int64(record.Length)
        currDelay := (record.SecondRecv - int64(record.SecondSend))*1e6 + record.USecondRecv - int64(record.USecondSend)
        if count != 0 {
            diff := currDelay - delay
            if diff < 0 {
                diff = -diff
            }
            jitter = jitter + (float64(diff) - jitter) / 16
        }
        delay = currDelay
        if delay > maxDelay {
            maxDelay = delay
        }
        if delay < minDelay {
            minDelay = delay
        }
        count++

        fmt.Println(record.Id, record.Length, record.SecondSend,
                    record.USecondSend, record.SecondRecv, record.USecondRecv,
                    (record.SecondRecv - int64(record.SecondSend))*1e6 + record.USecondRecv - int64(record.USecondSend),
                    jitter)
    }
    totalCount := maxId
    receivedCount := count
    lossrate := float64(totalCount - receivedCount) / float64(totalCount)
    totalTime := endTime - startTime
    bandwidth := int64(0)
    if totalTime != 0 {
        bandwidth = totalFlow * 8 * 1e6 / totalTime / 1024 
    }
    finish <- &report{totalCount, receivedCount, lossrate, jitter, bandwidth, totalTime}
}

func main() {
    flag.Parse()
    name := "0.0.0.0:10000"
    if flag.NArg() > 0 {
        name = flag.Arg(0)
    }
    addr, err := net.ResolveUDPAddr(name)
    if err != nil {
        handle_err(err)
    }

    c, err := net.ListenUDP("udp4", addr)
    if err != nil {
        handle_err(err)
    }

    buf := make([]byte, 2000)
    header := new(iperfHeader)
    channel := make(chan *record, 1000000)
    finish  := make(chan *report)
    ready   := true
    go outputResult(channel, finish)
    for {
        length, addr_out, err := c.ReadFromUDP(buf)
        if err != nil {
            handle_err(err)
        }

        sec, nsec, err := os.Time()
        if err != nil {
            handle_err(err)
        }

        buffer := bytes.NewBuffer(buf)
        err = binary.Read(buffer, binary.BigEndian, header)
        if err != nil {
            handle_err(err)
        }

        usec := nsec / 1000
        channel <- &record{header.Id, int32(length), header.Second, header.Usecond, sec, usec}

        if header.Id < 0 && ready{
            ready = false
            close(channel)

            c_out, err := net.DialUDP("udp4", nil, addr_out)
            if err != nil {
                handle_err(err)
            }
    

            r := <-finish
            fmt.Println(r)
            c_out.Write(make([]byte,1400))
            break
        } else if header.Id >= 0 {
            ready = true
        }
    }
    return
}
