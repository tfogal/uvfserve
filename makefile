CXXFLAGS=-ggdb -Wall -Wextra -std=c++0x
OBJ=serve-uvf.o socket-event.o socket-watchable.o

all: $(OBJ) uvfserve

uvfserve: serve-uvf.o socket-watchable.o
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJ) uvfserve
