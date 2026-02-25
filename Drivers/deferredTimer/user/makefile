CXX 			:= g++
CXX_FLAGS := -O3 -Wall -Wextra -Werror
LIBS      := TimerDriver/timerDriver.a

SRC				:= user.cpp
EXE				:= driver

build_depencies:
	$(MAKE) -C TimerDriver


default: build_depencies
	$(CXX) $(CXX_FLAGS) $(SRC) -L $(LIBS) -o $(EXE)
json:
	bear -- make
run:
	-./$(EXE)
clean:
	rm -rf $(EXE)
	$(MAKE) -C TimerDriver clean
