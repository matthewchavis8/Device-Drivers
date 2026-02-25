CXX 			:= g++
CXX_FLAGS := -O3 -Wall -Wextra -Werror
SRC				:= timerDriver.cpp
OBJ				:= timerDriver.o
LIB				:= timerDriver.a

default:
	$(CXX) $(CXX_FLAGS) -c $(SRC) -o $(OBJ)
	ar rcs $(LIB) $(OBJ)
clean:
	rm -rf $(OBJ)
	rm -rf $(LIB)
