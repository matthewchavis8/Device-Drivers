obj-m += hello-1.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.* *.symvers *.order *.cmd
	rm -rf .tmp_versions
	rm -rf .cache
	rm *.mod
	rm .*

