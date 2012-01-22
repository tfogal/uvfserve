CXXFLAGS=-ggdb -Wall -Wextra -std=c++0x -pedantic
OBJ=serve-uvf.o socket-watchable.o shutdown.o stdin-watchable.o watchable.o

all: $(OBJ) uvfserve

uvfserve: serve-uvf.o socket-watchable.o shutdown.o stdin-watchable.o \
          watchable.o
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJ) uvfserve
