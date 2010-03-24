#!/bin/bash
for bandwidth in 1M 1.1M 1.2M 1.3M 1.4M 1.5M 1.6M 1.7M 1.8M 1.9M 2M
do
    iperf -u -c 180.129.107.204 -p 10000 -l 1360 -t 30 -b $bandwidth
    sleep 8
done
