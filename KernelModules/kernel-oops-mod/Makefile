obj-m 		+= kernel-oops-mod.o

KDIR      := /lib/modules/$(shell uname -r)/build
PWD       := $(CURDIR)

# Build Configs
ccflags-y += -Werror -Wextra -O3 -g

# Build Normal
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Build compile_commands.json
json:
	bear -- $(MAKE) -C $(KDIR) M=$(PWD)

# Clean Folder
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
