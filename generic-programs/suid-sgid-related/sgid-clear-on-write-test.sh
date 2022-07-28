#!/bin/bash

# Run this program as "root".
# Create a file without group exec permission. Set sgid bit and write to
# file with CAP_FSETID dropped. As file does not have group execute bit
# set, writing to it should not clear SGID bit. Note, "root" will write
# to file without CAP_FSETID and file is owned by uid/gid "test/test".

# Purpose of this test is to figure out when should SGID bit be cleared
# and when it should not be. Current behavior seems strange. It clear
# SGID if SUID is set as well. But leaves SGID intact if SUID is not
# set.

FILENAME="foo.txt"

# Remove any previous file.
rm $FILENAME
touch $FILENAME
chmod 644  $FILENAME
chown test:test  $FILENAME

# First just test with SGID bit set (no SETUID) bit set. This does not
# clear SGID bit. 
echo "Testing with only SGID bit set"
chmod g+s $FILENAME

echo "before:"; stat -c '%A' $FILENAME

capsh --drop=cap_fsetid -- -c "/usr/sbin/xfs_io -d -c 'pwrite 0 4k' $FILENAME >> /dev/null"

echo "after:"; stat -c '%A' $FILENAME


# Now test with both SUID and SGID bit. This does clear SGID bit.
echo -e "\nTesting with both suid/sgid bit set"
chmod ug+s $FILENAME

echo "before:"; stat -c '%A' $FILENAME

capsh --drop=cap_fsetid -- -c "/usr/sbin/xfs_io -d -c 'pwrite 0 4k' $FILENAME >> /dev/null"

echo "after:"; stat -c '%A' $FILENAME
