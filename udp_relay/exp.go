package main
import "fmt"
import "os"

type Error struct {
    text string
}

func (e *Error) String() string{
    return e.text
}

func return_nil() *Error {
    return nil
}

func main() {
    var a os.Error = nil
    fmt.Println(a)
    a = return_nil()
    fmt.Println(a)
}
