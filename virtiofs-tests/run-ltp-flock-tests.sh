#!/bin/bash

LTPPATH=/opt/ltp/testcases/bin/
export TMPDIR=/mnt/virtio-fs/ltp/

run_tests()
{
   $LTPPATH/flock01
}

run_tests
