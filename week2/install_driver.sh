#!/bin/sh

module=$1
mode="666"
major=420

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
shift
/sbin/insmod ./$module.ko $@ || exit 1

# remove stale nodes
rm -f /dev/${module}

mknod /dev/${module} c $major 0

# give appropriate group/permissions, and change the group.
# Not all distributions have staff, some have "wheel" instead.
group="debian"

chgrp $group /dev/${module}
chmod $mode /dev/${module}