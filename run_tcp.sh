#!/bin/bash
iperf -c 180.129.107.204 -p 18000 -i 1 -t 60 >$1
