obj-m += list-sym.o

KDIR      := /lib/modules/$(shell uname -r)/build
PWD       := $(CURDIR)

# Build Configs
ccflags-y += -Werror -Wextra -O3

# Static Link symbols
KBUILD_EXTRA_SYMBOLS += ~/Dev/Device-Drivers/list-sync-mod/Module.symvers

# Build Normal
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Build compile_commands.json
json:
	bear -- $(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean Folder
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
