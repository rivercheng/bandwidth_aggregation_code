package main

import (
    "fmt"
    "strings"
    "strconv"
    "io/ioutil"
    "bytes"
    "os"
)

func handleError (err os.Error) {
    fmt.Fprintln(os.Stderr, err)
    os.Exit(1)
}

func main() {
    if content, err := ioutil.ReadFile("record"); err != nil {
        handleError(err)
    } else {
        buf := bytes.NewBuffer(content)
        str := buf.String()
        lines := strings.Split(str, "\n", 0)
        for _, line := range lines {
            words   := strings.Split(line, " ", 0)
            if len(words) < 3 {
                continue
            }
            id, err := strconv.Atoi(words[0])
            if err != nil {
                handleError(err)
            }
            ts, err := strconv.Atoi(words[1])
            if err != nil {
                handleError(err)
            }
            uts,err := strconv.Atoi(words[2])
            if err != nil {
                handleError(err)
            }
            fmt.Println(id, ts, uts)
        }
    }
}


