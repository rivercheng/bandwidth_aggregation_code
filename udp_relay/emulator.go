package main
import (
    "net"
    "os"
    "fmt"
    "encoding/binary"
    "bytes"
    "flag"
)

type iperfHeader struct {
    Id int32;
    Second int32;
    Usecond int32
}

func handleError(err os.Error) {
    fmt.Fprintln(os.Stderr, err)
    os.Exit(1)
}

func main() {
    records := readRecords("records")
    for i, record := range records {
        fmt.Println(i, record)
    }

    name := "0.0.0.0:10000"
    flag.Parse()
    if flag.NArg() > 0 {
        name = flag.Arg(0)
    }
    addr, err := net.ResolveUDPAddr(name)
    if err != nil {
        handleError(err)
    }

    outName := "localhost:10020"
    if flag.NArg() > 1 {
        outName = flag.Arg(1)
    }
    addr_out, err := net.ResolveUDPAddr(outName)
    if err != nil {
        handleError(err)
    }

    c, err := net.ListenUDP("udp4", addr)
    if err != nil {
        handleError(err)
    }

    c_out, err := net.DialUDP("udp4", nil, addr_out)
    if err != nil {
        handleError(err)
    }

    buf := make([]byte, 2000)
    header := new(iperfHeader)
    for {
        length, _, err := c.ReadFrom(buf)
        if err != nil {
            handleError(err)
        }

        buffer := bytes.NewBuffer(buf)
        err = binary.Read(buffer, binary.BigEndian, header)
        if err != nil {
            handleError(err)
        }

        c_out.Write(buf[0:length])
    }
}
