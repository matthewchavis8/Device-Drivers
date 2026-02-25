CXX 			:= g++
CXX_FLAGS := -O3
SRC				:= test.cpp
EXE				:= driver

default:
	$(CXX) $(CXX_FLAGS) $(SRC) -o $(EXE)
json:
	bear -- make
run:
	-./$(EXE)
clean:
	rm -rf $(EXE)
