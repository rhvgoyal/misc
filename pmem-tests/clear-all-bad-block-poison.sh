#!/bin/bash
set -x

clear_ndctl_poison() {
  local block_number count

  block_number=`ndctl inject-error -t namespace0.0 | grep "\"block\":" | cut -d ":" -f2 | sed 's/,//'`
  count=`ndctl inject-error -t namespace0.0 | grep "\"count\":" | cut -d ":" -f2 | sed 's/,//'`
  [ -z "$block_number" ] && return

  ndctl inject-error --uninject --block=$block_number --count=$count namespace0.0
  ndctl inject-error -t namespace0.0
}


clear_bad_blocks() {
  local bad_sector nr_sector;

  while read bad_sector nr_sector; do
    dd if=/dev/zero of=/dev/pmem0 bs=512 seek=$bad_sector count=$nr_sector oflag=direct
  done < "/sys/block/pmem0/badblocks" 
}

clear_bad_blocks
clear_ndctl_poison
