#!/bin/bash
~/Projects/emulator/emulator $1 9000 10030 172.20.133.13 &
sleep 1
cvlc -vvv ~/Downloads/long_combined_1.5M.mts --sout udp:127.0.0.1:9000 >/dev/null 2>/dev/null
