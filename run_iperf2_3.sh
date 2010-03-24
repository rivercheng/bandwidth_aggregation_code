#!/bin/bash
for bandwidth in 2M 2.1M 2.2M 2.3M 2.4M 2.5M 2.6M 2.7M 2.8M 2.9M 3M
do
    iperf -u -c 180.129.107.204 -p 10000 -l 1360 -t 30 -b $bandwidth
    sleep 8
done
