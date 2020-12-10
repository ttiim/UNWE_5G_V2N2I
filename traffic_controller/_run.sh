#!/bin/bash

echo $(sudo tshark -i eth0 -Y "udp.dstport==6053" -c 20 -T fields -e data.data | tr -d ':')>data_UDP
#python3 tscbm.py
./a.out


