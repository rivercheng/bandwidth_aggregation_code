/*
 * try to read a file and convert it to numbers and processing
 */

package main

import (
	"strings"
	"strconv"
	"io/ioutil"
	"bytes"
)

type Record struct {
	id   int
    success bool
	sec  int
	usec int
}

func readRecords(filename string) []Record {
	content, err := ioutil.ReadFile(filename)
	if err != nil {
		handleError(err)
	}

	buf := bytes.NewBuffer(content)
	str := buf.String()
	lines := strings.Split(str, "\n", 0)
	records := make([]Record, len(lines))
	index := 0

	for _, line := range lines {
		words := strings.Split(line, " ", 0)
		if len(words) < 4 {
			continue
		}
		id, err := strconv.Atoi(words[0])
		if err != nil {
			handleError(err)
		}

        success := false
        if words[1] == "true" {
            success = true
        }
		
        sec, err := strconv.Atoi(words[2])
		if err != nil {
			handleError(err)
		}
		usec, err := strconv.Atoi(words[3])
		if err != nil {
			handleError(err)
		}
		records[index] = Record{id, success, sec, usec}
		index++
	}
    records = records[0:index]
	return records
}
