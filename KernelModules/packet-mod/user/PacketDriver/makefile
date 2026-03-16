CXX 			:= clang++
CXX_FLAGS := -O2 -Wall -Wextra -Werror -Wunused
SRC				:= packetDriver.cpp
OBJ				:= packetDriver.o
LIB				:= libpacketDriver.a

default:
	$(CXX) $(CXX_FLAGS) -c $(SRC) -o $(OBJ)
	ar rcs $(LIB) $(OBJ)
clean:
	rm -rf $(OBJ)
	rm -rf $(LIB)
