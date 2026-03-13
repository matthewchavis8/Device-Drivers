CXX 			:= clang++
CXX_FLAGS := -O2 -Wall -Wextra -Werror -Wunused
SRC				:= socket.cpp
OBJ				:= socket.o
LIB				:= libsocket.a

default:
	$(CXX) $(CXX_FLAGS) -c $(SRC) -o $(OBJ)
	ar rcs $(LIB) $(OBJ)
clean:
	rm -rf $(OBJ)
	rm -rf $(LIB)
