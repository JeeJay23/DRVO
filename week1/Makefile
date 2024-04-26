KDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
MAKE = /usr/bin/make

%.ko : %.c
	$(MAKE) $(*).ko obj-m=$(*).o -C $(KDIR) M=$(PWD) modules

clean :
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions

