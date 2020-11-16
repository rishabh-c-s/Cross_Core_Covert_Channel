#!/bin/bash

rm -f hits_misses.txt
rm -f access_time.txt
gnome-terminal -x taskset 0x1 ./SEND_CONFIG
taskset 0x2 ./RECV_CONFIG
