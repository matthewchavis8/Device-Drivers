CXX 			:= g++
CXX_FLAGS := -O3 -Wall -Wextra -Werror
LIBS      := TimerDriver/libtimerDriver.a
SRC				:= user.cpp
EXE				:= driver

all: default

build_depencies:
	$(MAKE) -C TimerDriver

default: build_depencies
	$(CXX) $(CXX_FLAGS) $(SRC) $(LIBS) -o $(EXE)

json:
	bear -- make

run:
	-./$(EXE)

clean:
	rm -rf $(EXE)
	$(MAKE) -C TimerDriver clean
