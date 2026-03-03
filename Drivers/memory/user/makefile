CC    := clang++
SRC   := main.cpp
EXE   := main
FLAGS := -Werror -Wextra -O3

default:
	$(CC) $(SRC) $(FLAGS) -o $(EXE)

run:
	./$(EXE)

json:
	bear --filter /usr/bin/clang++ -- make

clean:
	rm $(EXE)
