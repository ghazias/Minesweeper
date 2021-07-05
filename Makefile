CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -g -std=c++17
LINK.o = $(LINK.cc)
TEST_OBJS = minesweeper.o

all: minesweeper

test_main: $(TEST_OBJS)

clean:
	rm -f *.o minesweeper