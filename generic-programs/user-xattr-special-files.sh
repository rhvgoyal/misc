#!/bin/bash

# Test user.* xattr on symlinks and special files on various filesystems.

set -e
TEST_FS="ext4 xfs btrfs overlay"

setup_filesystem () {
  local fstype=$1 block_dev=$2 mnt=$3

  if [ "$fstype" == "ext4" ];then
    mkfs.ext4 -F $block_dev > /dev/null
    mount -t $fstype $block_dev $mnt
  elif [ "$fstype" == "xfs" ];then
    mkfs.xfs -f $block_dev > /dev/null
    mount -t $fstype $block_dev $mnt
  elif [ "$fstype" == "btrfs" ];then
    mkfs.btrfs -f $block_dev > /dev/null
    mount -t $fstype $block_dev $mnt
  elif [ "$fstype" == "overlay" ];then
    mkdir -p $mnt/lower $mnt/upper $mnt/work
    mount -t overlay -o lowerdir=$mnt/lower,upperdir=$mnt/upper,workdir=$mnt/work none $mnt
  else
    echo "Unknown filesystem $fstype"
    exit 1
  fi
}

cleanup_filesystem() {
  local fstype=$1 mnt=$2

  umount $mnt
  if [ "$fstype" == "overlay" ];then
     rm -rf $mnt/lower $mnt/upper $mnt/work
  fi
}


test_user_xattr()
{
  local file=$1 xattr_value
  local value="bar"
  
  setfattr -h -n "user.foo" -v "$value" $file
  xattr_value=`getfattr -h --only-values --absolute-names -n "user.foo" $file`
  if [ "$xattr_value" != "$value" ];then
     echo "Error: Got xattr value=$xattr_value. Expected=$value on file $file"
     return 1
  fi

  return 0
}

test_user_xattr_special_files()
{
  local fstype=$1 mnt=$2 xattr_value
  local value="bar"
  local major="1" minor="50"
  
  # Test symlink
  touch $mnt/foo.txt
  ln -s $mnt/foo.txt $mnt/foo-link.txt
  if ! test_user_xattr "$mnt/foo-link.txt";then
    echo "Test failed on fstype=$fstype filetype=symlink"
    return
  fi

  # Test device node
  mknod $mnt/test-dev c $major $minor 
  if ! test_user_xattr "$mnt/test-dev";then
    echo "Test failed on fstype=$fstype filetype=char-device"
    return
  fi

  echo "Test succeeded on fstype=$fstype"
}

run_fs_test() {
  local fstype=$1 block_dev=$2

  echo "Running test on fstype=$fstype block_dev=$block_dev"
  setup_filesystem $fstype $block_dev $MNT_POINT
  test_user_xattr_special_files $fstype $MNT_POINT
  cleanup_filesystem $fstype $MNT_POINT
}

run_test() {
  local block_dev=$1 fstype

  for fstype in $TEST_FS;do
    run_fs_test $fstype $block_dev
  done
    
}

usage() {
  echo "Usage: $1 <block-device> <mnt-point>"
}

# Main
if [ $# -ne 2 ];then
  usage $(basename $0)
  exit 1
fi

BLOCK_DEV=$1
MNT_POINT=$2

if [ ! -b "$BLOCK_DEV" ];then
  echo "Error: $BLOCK_DEV is not a block device"
  exit 1
fi

if [ ! -d "$MNT_POINT" ];then
  echo "Error: $MNT_POINT is not a directory"
  exit 1
fi

umount -q $MNT_POINT || true
run_test $BLOCK_DEV
