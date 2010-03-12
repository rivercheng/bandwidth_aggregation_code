package main
import "net"
import "os"
import "fmt"

func handle_err(err os.Error) {
    fmt.Print(err.String())
    os.Exit(1)
}

func main() {
    addr, err := net.ResolveUDPAddr("0.0.0.0:10000")
    if err != nil {
        handle_err(err)
    }

    addr_out, err := net.ResolveUDPAddr("localhost:10020")
    if err != nil {
        handle_err(err)
    }
    
    c, err := net.ListenUDP("udp4", addr)
    if err != nil {
        handle_err(err)
    }
    
    c_out, err := net.DialUDP("udp4", nil, addr_out)
    if err != nil {
        handle_err(err)
    }

    buf := make([]byte, 2000)
    for {
        length, _, err := c.ReadFrom(buf)
        if err != nil {
            handle_err(err)
        }
        c_out.Write(buf[0:length])
    }
}


        
