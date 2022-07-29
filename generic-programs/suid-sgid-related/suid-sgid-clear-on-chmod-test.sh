#!/bin/bash

# Run this program as "root".
# Create a file without group exec permission. Set sgid bit and write to
# file with CAP_FSETID dropped. As file does not have group execute bit
# set, writing to it should not clear SGID bit. Note, "root" will write
# to file without CAP_FSETID and file is owned by uid/gid "daemon/daemon".

# Purpose of this test is to figure out when should SUID/SGID bit be
# cleared.

prep_file() {
    filename="$1"

    touch $filename
    chmod 644  $filename
    chown 2:2  $filename
}

test_with_only_sgid_set()
{
    filename="$1"

    # First just test with SGID bit set (no SETUID) bit set. chmod does
    # clear SGID bit even if file does not have group execute permission.
    echo "Testing with only SGID bit set"
    chmod g+s $filename

    echo "before:"; stat -c '%A' $filename

    capsh --drop=cap_fsetid -- -c "/usr/bin/chmod u-w $filename >> /dev/null"

    echo "after:"; stat -c '%A' $filename
}

test_with_suid_sgid_set()
{
    filename="$1"

    # Now test with both SUID and SGID bit. This does clear SGID bit but
    # leaves SUID in place.
    echo -e "\nTesting with both suid/sgid bit set"
    chmod ug+s $filename

    echo "before:"; stat -c '%A' $filename

    capsh --drop=cap_fsetid -- -c "/usr/bin/chmod u-w $filename >> /dev/null"

    echo "after:"; stat -c '%A' $filename
}

usage() {
  cat <<-FOE
Usage: $1 <file-to-create>
FOE
}

if [ $# -ne 1 ];then
  usage $(basename $0)
  exit 1
fi

FILENAME="$1"

# Remove any previous file.
rm $FILENAME
prep_file $FILENAME

test_with_only_sgid_set "$FILENAME"
test_with_suid_sgid_set "$FILENAME"
