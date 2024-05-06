#!/bin/bash

make $1.ko obj-m=$1.o -C /lib/modules/$(uname -r)/build M=$(pwd) modules
