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
	$(MAKE) -C $(KDIR) M=$(PWD) compile_commands.json 

# Clean Folder
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


