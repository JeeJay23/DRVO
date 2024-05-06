#!/bin/bash

module=$1

echo "uninstalling module $module"

# invoke rmmod with all arguments we got
/sbin/rmmod $module || exit 1

rm -f /dev/${module} 