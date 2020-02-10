#!/bin/bash
set -x

START_BLOCK="0"
NR_BLOCKS="1"
PMEMDEV="pmem0"
SYSPMEMDEV="pmem0"
PARTOFFSET="2048"

create_partition() {
  local pmemdev=$1

  parted /dev/$pmemdev --script -- mklabel msdos
  parted /dev/$pmemdev "unit s mkpart primary ${PARTOFFSET} 4096"
}

remove_partition() {
  local pmemdev=$1

  kpartx -d /dev/$pmemdev
  parted /dev/$pmemdev "rm 1" | true
}

poison_nvdimm_blocks() {
  local start_block=$1
  local nr_blocks=$2

  ndctl inject-error -B $start_block -n $nr_blocks -N namespace0.0
  sleep 5
  ndctl inject-error -t namespace0.0
}

do_block_poison_test()
{
  local pmemdev syspmemdev start_block nr_blocks

  pmemdev=$1
  syspmemdev=$2
  start_block=$3
  nr_blocks=$4

  poison_nvdimm_blocks "$start_block" "$nr_blocks"

  dd if=/dev/$pmemdev of=/tmp/testfile bs=4K count=2 iflag=direct

  sleep 2

  cat /sys/block/$syspmemdev/badblocks

  dd if=/dev/zero of=/dev/$pmemdev bs=4K count=2 oflag=direct

  cat /sys/block/$syspmemdev/badblocks

  ndctl inject-error -t namespace0.0
}


# Do full block device test
remove_partition "$PMEMDEV"
do_block_poison_test "$PMEMDEV" "$SYSPMEMDEV" "0" "1"

echo "Press a key to continue"
read

# Do partition device test
create_partition "$PMEMDEV"
do_block_poison_test "${PMEMDEV}p1" "$SYSPMEMDEV" "$PARTOFFSET" "1"

echo "Press a key to continue"
read

# Do dm partition device test
kpartx -a /dev/$PMEMDEV
do_block_poison_test "mapper/${PMEMDEV}p1" "$SYSPMEMDEV" "$PARTOFFSET" "1"
