#!/bin/bash

NR_RUN=10

for (( i=0; i<$NR_RUN; i++ ))
do
  blogbench -d /mnt/virtio-fs/blogbench/ -w 100 -W 100 -c 100
done
