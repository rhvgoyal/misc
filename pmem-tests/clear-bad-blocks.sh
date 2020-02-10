#!/bin/bash
set -x

PMEM_DEVICE="pmem0"

if [ $# -lt 2 ];then
  echo "Usage: $0 <starting_block> <number-of-blocks>"
  exit 1
fi

START_BLOCK=$1
NR_BLOCKS=$2

START_BLOCK_4K=$((START_BLOCK/8))
NR_BLOCKS_4K=$((NR_BLOCKS/8))

if [ "$NR_BLOCKS_4K" == "0" ]; then
  NR_BLOCKS_4K="1"
fi
cat /sys/block/$PMEM_DEVICE/badblocks

#dd if=/dev/zero of=/dev/$PMEM_DEVICE bs=512 seek=$START_BLOCK count=$NR_BLOCKS oflag=direct
dd if=/dev/zero of=/dev/$PMEM_DEVICE bs=4K seek=$START_BLOCK_4K count=$NR_BLOCKS_4K oflag=direct

cat /sys/block/$PMEM_DEVICE/badblocks
ndctl inject-error -t namespace0.0
