# Device Drivers
#### Purpose: Experimenting with Linux Device Drivers on a Raspberry PI5


#### CMDS:
 - bear -- make CC=gcc:                        
    generate a compile_commands.json for LSP server
 - ssh kernel@192.168.50.2:
    SSH onto raspberry PI5 dev board to build linux kernel
 - sudo ip addr add 192.168.50.1/24 dev enp0s31f6: 
    Add address for PI5 which is connected by ethernet so you can ssh into into it
