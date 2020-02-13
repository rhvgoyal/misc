#!/bin/bash

LTPPATH=/opt/ltp/testcases/bin/

export TMPDIR=/mnt/virtio-fs/ltp/

run_tests()
{
# nfslocktest
  $LTPPATH/locktests -n 1  -f /mnt/virtio-fs/foo.txt

# fcntl05 passes
  $LTPPATH/fcntl05

# fcntl11 gives warnings because GETLK does not return correct pid. That's
# limitation of emulating posix locks using OFD locks.
  $LTPPATH/fcntl11

# fcntl13 passes
  $LTPPATH/fcntl13

#  Bunch of tests fail due to pid and lack of blocking support.
 $LTPPATH/fcntl14

# Deadlock detection test fails. Which is bound to fail as OFD locks don't
# perform deadlock detection. This in fact hangs some threads in daemon
# because of deadlock and makes it impossible to unmount virtiofs. I think
# solution is to implement interrupt request and be able to cancel outstanding
# requests.
# For now do not run this test 
#$LTPPATH/fcntl17

# fcntl18 passed
 $LTPPATH/fcntl18
# Failures related to locking pid wrong.
 $LTPPATH/fcntl19
# Failures related to locking pid.
 $LTPPATH/fcntl20
# Failures related to locking pid.
 $LTPPATH/fcntl21
}

run_tests
