# Device Drivers

A collection of Linux kernel modules and character device drivers developed for a Raspberry Pi 5, covering fundamentals from "Hello World" modules through synchronized linked lists to interrupt-driven keyboard input.

## Directory Structure

```
KernelModules/
  hello-1/            — Minimal hello-world module
  error-mod/          — Variable init and arithmetic in kernel space
  kernel-oops-mod/    — Intentional NULL deref for GDB debugging practice
  cmd-mod/            — Command-line parameter passing via module_param
  proc-info-mod/      — Iterating the process list with for_each_process
  kernelAlloc-mod/    — Heap allocation with kmalloc/kfree
  kernelMemory-mod/   — Dynamic task_info structs from the process hierarchy
  multi-mod/          — Multi-file module build (mod1.c + mod2.c)
  kernel-Lists-mod/   — Kernel linked list basics (list_head, list_add, list_del)
  kernel-Lists-full-mod/ — Extended list with find, expiration, and atomic counters
  sched-spin-mod/     — Intentional sleep-under-spinlock for debugging
  list-sync-mod/      — Synchronized list with rwlock and EXPORT_SYMBOL
  list-sym-mod/       — Consumer of exported symbols from list-sync

Drivers/
  so2_cdev/           — Character device driver with open/read/write/release
  kbd/                — Keyboard driver using the i8042 controller and IRQ handling
```

## Prerequisites

- Raspberry Pi 5 (or any ARM64 Linux target)
- Linux kernel headers matching the target kernel
- Cross-compilation toolchain (if building on a different host)

## Building

Each module has its own `Makefile`. To build a single module:

```bash
cd KernelModules/hello-1
make
```

To generate a `compile_commands.json` for your LSP (clangd, etc.):

```bash
bear -- make CC=gcc
```

## Deploying to the Raspberry Pi

```bash
# Add a static IP for the Pi (connected via Ethernet)
sudo ip addr add 192.168.50.1/24 dev enp0s31f6

# SSH onto the Pi
ssh kernel@192.168.50.2

# Copy a built module to the Pi
scp hello-1.ko kernel@192.168.50.2:~/

# On the Pi — load, inspect, and unload
sudo insmod hello-1.ko
dmesg | tail
sudo rmmod hello-1
```

## Documentation

All source files use Doxygen-style documentation (`/** */` blocks with `@file`, `@brief`, `@param`, `@return` tags). To generate HTML docs (optional):

```bash
doxygen Doxyfile
```

## License

GPL
