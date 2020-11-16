#!/bin/bash

gnome-terminal -x taskset 0x1 ./SEND
taskset 0x2 ./RECV
