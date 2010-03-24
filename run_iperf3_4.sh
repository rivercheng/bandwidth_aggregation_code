#!/bin/bash
for bandwidth in 3M 3.1M 3.2M 3.3M 3.4M 3.5M 3.6M 3.7M 3.8M 3.9M 4M
do
    iperf -u -c 180.129.107.204 -p 10000 -l 1360 -t 30 -b $bandwidth
    sleep 8
done
