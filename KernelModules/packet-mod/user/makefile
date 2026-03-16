SRC   := main.cpp
EXE   := main
FLAGS := -O2 -Wall -Wextra -Werror -Wunused
CXX   := clang++
LIBS  := socket/libsocket.a PacketDriver/libpacketDriver.a


all: default

build_depencies:
	$(MAKE) -C socket
	$(MAKE) -C PacketDriver

default: build_depencies
	$(CXX) $(CXX_FLAGS) $(SRC) $(LIBS) -o $(EXE)

json:
	bear -- make

run:
	-./$(EXE)

clean:
	rm -rf $(EXE)
	$(MAKE) -C socket clean
