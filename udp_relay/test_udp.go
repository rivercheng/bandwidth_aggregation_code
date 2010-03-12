package main
import "net"
import "os"
import "fmt"

func handle_err(err os.Error) {
    fmt.Println(err.String())
    os.Exit(1)
}

func byte_array_to_int32(buf []byte) (res int32, err os.Error) {
    fmt.Println(len(buf))
    if len(buf) != 4 {
        err = os.NewError("buf is not 4 bytes.")
    } else {
        res = int32(buf[3])
        res += int32(buf[2]) * 256
        res += int32(buf[1]) * 256 * 256
        res += int32(buf[0]) * 256 * 256 * 256
    }
    return
}
    
func main() {
    addr, err := net.ResolveUDPAddr("0.0.0.0:10000")
    if err != nil {
        handle_err(err)
    }

    /*
    addr_out, err := net.ResolveUDPAddr("localhost:10020")
    if err != nil {
        handle_err(err)
    }
    */
    
    c, err := net.ListenUDP("udp4", addr)
    if err != nil {
        handle_err(err)
    }
    
    /*
    c_out, err := net.DialUDP("udp4", nil, addr_out)
    if err != nil {
        handle_err(err)
    }
    */

    buf := make([]byte, 2000)
    for {
        length, _, err := c.ReadFrom(buf)
        if err != nil {
            handle_err(err)
        }
        if length < 4 {
            continue
        }
        if buf[0] == 255 && buf[1] == 255 {
            break
        }
        res, err := byte_array_to_int32(buf[0:4])
        fmt.Println(err == nil)
        if err != nil {
            fmt.Println(err)
        }
        fmt.Println(buf[0:4], res)
    }
    return
}


        
