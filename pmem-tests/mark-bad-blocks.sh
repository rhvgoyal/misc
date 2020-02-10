#!/bin/bash
set -x

PMEM_DEVICE="pmem0"

if [ $# -lt 2 ];then
  echo "Usage: $0 <starting_block> <number-of-blocks>"
  exit 1
fi

START_BLOCK=$1
NR_BLOCKS=$2

ndctl inject-error -B $START_BLOCK -n $NR_BLOCKS -N namespace0.0

ndctl inject-error -t namespace0.0

sleep 5

dd if=/dev/$PMEM_DEVICE of=/dev/null bs=512 skip=$START_BLOCK count=$NR_BLOCKS iflag=direct

sleep 5

cat /sys/block/$PMEM_DEVICE/badblocks
