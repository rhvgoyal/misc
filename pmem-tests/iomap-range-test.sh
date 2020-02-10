#!/bin/bash

#set -x
set -e

PMEMDEV="pmem0"
SYSPMEMDEV="pmem0"
MNTPOINT="/mnt/dax-ext4/"
NAMESPACE="namespace0.0"
NR_POISON_SECTORS="1"
TRUNCATE_SIZE="23"
PARTOFFSET="4096"

remove_partition() {
  local pmemdev=$1

  kpartx -d /dev/$pmemdev
  parted /dev/$pmemdev "rm 1" | true
}

create_partition() {
  local pmemdev=$1

  parted /dev/$pmemdev --script -- mklabel msdos
  # Size 2G
  parted /dev/$pmemdev "unit s mkpart primary ${PARTOFFSET} 4198399"
  sleep 2
}

poison_sector()
{
  local sector_start poison_sector partition_offset

  partition_offset=$1

  # Kept here for debugging
  xfs_io -c "fiemap" $MNTPOINT/zerofile

  sector_start=`xfs_io -c "fiemap" $MNTPOINT/zerofile | tail -1 | cut -d " " -f3 | cut -d "." -f1`

 poison_sector=$((sector_start+4))
 if [ -n "$partition_offset" ]; then
   poison_sector=$(($poison_sector+partition_offset))
 fi
 echo "sector_start=$sector_start, partition_offset=$partition_offset poison_sector=$poison_sector"

 ndctl inject-error -B $poison_sector -n $NR_POISON_SECTORS -N $NAMESPACE
 sleep 5
 ndctl inject-error -t $NAMESPACE
}

is_poison_cleared()
{
  local syspmemdev=$1 start_sector nr_sectors

  while read start_sector nr_sector; do
    if [ -n "$start_sector" ]; then
      echo "Error: badblocks is not cleared at sector $start_sector:$nr_sectors"
      return 1
    fi
  done < "/sys/block/$syspmemdev/badblocks"

  # Check ndctl
  start_sector=`ndctl inject-error -t namespace0.0 | grep "\"block\":" | cut -d ":" -f2 | sed 's/,//'`
  nr_sectors=`ndctl inject-error -t namespace0.0 | grep "\"count\":" | cut -d ":" -f2 | sed 's/,//'`
    if [ -n "$start_sector" ]; then
      echo "Error: ndctl reports poison at sector $start_sector:$nr_sectors"
      return 1
    fi
  return 0
}

do_iomap_zero_range_test()
{
        local pmemdev=$1
        local syspmemdev=$2
        local partition_offset=$3
	local fstype=$4

	if [ "$fstype" == "ext4" ] ; then
	  mkfs.ext4 -F /dev/$pmemdev
	else
	  mkfs.xfs -f /dev/$pmemdev -m reflink=0
	fi

	mount -o dax /dev/$pmemdev $MNTPOINT

	dd if=/dev/zero of=$MNTPOINT/zerofile bs=4K count=1

	# Poison memory
	poison_sector $partition_offset

	echo "Will read file now after sleeping a bit"
	sleep 10

	#dd if=$MNTPOINT/zerofile of=/dev/null bs=512 count=8 iflag=direct || true
	cat $MNTPOINT/zerofile || true
	cat /sys/block/$syspmemdev/badblocks

	# Truncate file to invoke iomap_zero_range
	echo "Truncating File and that should clear poison"
	truncate -s $TRUNCATE_SIZE $MNTPOINT/zerofile

	sleep 5
	if ! is_poison_cleared $syspmemdev; then
	  echo "FAIL: Poison not cleared on device $syspmemdev while testing $pmemdev"
	  exit
	else
	  echo "PASS: Poison cleared on device $syspmemdev while testing $pmemdev"
	fi
}

wait_for_keypress()
{
	echo "Press a key to continue"
	read
}

do_test()
{
	local fstype=$1
	
	# Do cleanup from previous runs
	umount $MNTPOINT || true
	remove_partition "$PMEMDEV"

	# Do full block device test
	echo "Doing Test on device $PMEMDEV filesystem=$fstype"
	do_iomap_zero_range_test "$PMEMDEV" "$SYSPMEMDEV" "0" "$fstype"

	wait_for_keypress

	# Create partition
	umount $MNTPOINT || true
	create_partition "$PMEMDEV"

	# Partition device test
	echo "Doing Test on device ${PMEMDEV}p1 filesystme=$fstype"
	do_iomap_zero_range_test "${PMEMDEV}p1" "$SYSPMEMDEV" "$PARTOFFSET" "$fstype"

	wait_for_keypress

	# Device mapper partition device test
	umount $MNTPOINT || true
	kpartx -a /dev/$PMEMDEV
	echo -e "\n\n\n\nDoing Test on device /dev/mapper/${PMEMDEV}p1 filesystem=$fstype\n"
	do_iomap_zero_range_test "mapper/${PMEMDEV}p1" "$SYSPMEMDEV" "$PARTOFFSET" "$fstype"
}

# Main tests
#do_test "ext4"
do_test "xfs"
