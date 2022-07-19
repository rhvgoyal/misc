#!/bin/bash

#  Copyright 2022 Red Hat, Inc.
#
#  Purpose:  Prepare metacopy image layer. 
#  Author:   Vivek Goyal <vgoyal@redhat.com>

set -e

# If given an image dir rootfs, this script will first create a copy of that
# dir (say lower) where files are hardlinked with original dir (rootfs) and
# then it will create a metacopy layer (say lower-meta) and chown all files
# in lower dir. This metacopy layer (lower-meta) and now be used with "lower"
# for overlayfs mount.

WORK_DIR="tmp_work"
MERGED_DIR="tmp_merged"

prepare_metacopy_layer()
{

   mkdir -p $_METACOPY_DIR $WORK_DIR $MERGED_DIR
   mount -t overlay -o lowerdir=$_LOWER_DIR,upperdir=$_METACOPY_DIR,workdir=$WORK_DIR,metacopy=on,index=on none $MERGED_DIR
   cd $MERGED_DIR
   echo "Will chown all files triggering metacopy"
   chown -R bin:bin .
   cd ..
   umount $MERGED_DIR
   rm -rf $MERGED_DIR $WORK_DIR
}

# Create directory tree and hardlinks into source dir
prepare_lower_dir()
{
    mkdir -p $_LOWER_DIR

    # Create a copy with hard links
    cp -al $_SOURCE_DIR/* $_LOWER_DIR/
    echo "Finished preparing hardlinked lower layer"
}

prepare_metacopy()
{
    prepare_lower_dir   
    prepare_metacopy_layer
}

usage() {
  cat <<-FOE
Usage: $1 [OPTIONS] <source-dir> <lower-dir> <metacopy-dir>
Options:
  -h, --help	Print help message
FOE
}

# Main
_INPUT_STR="$@"
_OPTS=`getopt -o h -l help -- $_INPUT_STR`
eval set -- "$_OPTS"
while true ; do
    case "$1" in
        -h | --help)  usage $(basename $0); exit 0;;
        --) shift; break;;
    esac
done

if [ $# -ne 3 ];then
  usage $(basename $0)
  exit 1
fi

_SOURCE_DIR=$1
if [ ! -d $_SOURCE_DIR ]; then
  echo "Error: $_SOURCE_DIR is not a dir"
  usage $(basename $0)
  exit 1
fi

_LOWER_DIR=$2
if [ -d $_LOWER_DIR ]; then
  echo "Error: $_LOWER_DIR dir already exists"
  exit 1
fi

_METACOPY_DIR=$3
if [ -d $_METACOPY_DIR ]; then
  echo "Error: $_METACOPY_DIR dir already exists"
  exit 1
fi

prepare_metacopy
