#!/bin/bash
~/Projects/emulator/emulator $1 9000 10030 &
sleep 1
iperf -u -c localhost -p 9000 -t $4
