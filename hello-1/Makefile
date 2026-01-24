obj-m += hello-1.o

KDIR      := /lib/modules/$(shell uname -r)/build
PWD       := $(CURDIR)

# Build Configs
ccflags-y += -Werror -Wextra -O3

# Build Normal
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Build compile_commands.json
json:
	$(MAKE) -C $(KDIR) M=$(PWD) modules CC=gcc

# Clean Folder
clean:
	rm -f *.o *.ko *.mod.* *.symvers *.order *.cmd
	rm -rf .tmp_versions
	rm -rf .cache
	rm *.mod
	rm .*
	$(MAKE) -C $(KDIR) M=$(PWD) clean


