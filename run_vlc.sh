#!/bin/bash
~/Projects/emulator/emulator $1 10000 10020 &
~/Projects/fec/fec e $2 $3 0.0.0.0 9000 10000 &
~/Projects/fec/fec d $2 $3 0.0.0.0 10020 10030 $4 &
sleep 1
cvlc -vvv ~/Downloads/long_combined_1.5M.mts --sout udp:127.0.0.1:9000 >/dev/null 2>/dev/null
