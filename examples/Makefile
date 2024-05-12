CXX = g++
CXX_FLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2

IDIR = -I../include
LDIR = -Llib

ifeq ($(OS), Windows_NT)
	LIBS = -ltcc
else
	LIBS = -ltcc -ldl -lpthread
endif

all: basic

basic:
	$(CXX) -o Basic Basic.cpp $(CXX_FLAGS) $(IDIR) $(LDIR) $(LIBS)

run-basic: basic
	./Basic

compiledb:
	compiledb --command-style --full-path --no-build make
