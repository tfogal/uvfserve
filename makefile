CXXFLAGS=-ggdb -Wall -Wextra -std=c++0x -pedantic
OBJ=serve-uvf.o socket-watchable.o

all: $(OBJ) uvfserve

uvfserve: serve-uvf.o socket-watchable.o
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJ) uvfserve
