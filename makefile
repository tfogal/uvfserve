CXXFLAGS=-ggdb -Wall -Wextra -std=c++0x -pedantic
OBJ=cmd-list.o serve-uvf.o socket-watchable.o shutdown.o stdin-watchable.o \
    watchable.o strfunc.o cmd-disconnect.o fs-scan.o

all: $(OBJ) uvfserve

uvfserve: cmd-list.o serve-uvf.o socket-watchable.o shutdown.o \
          fs-scan.o \
          stdin-watchable.o \
          cmd-disconnect.o \
          strfunc.o \
          watchable.o
	$(CXX) $^ -o $@

clean:
	rm -f $(OBJ) uvfserve
