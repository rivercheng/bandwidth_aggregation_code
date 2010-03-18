#!/bin/bash
~/Projects/emulator/emulator $1 10000 10020 &
~/Projects/fec/fec e $2 $3 0.0.0.0 9000 10000 &
~/Projects/fec/fec d $2 $3 0.0.0.0 10020 10030 &
sleep 1
iperf -u -c localhost -p 9000 -t $4
