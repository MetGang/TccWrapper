CXX = g++
CXX_FLAGS = -DTW_USE_EXCEPTIONS -DTW_USE_OPTIONAL -std=c++17 -O2 -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wcast-align -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2 -Wp,-w

TCC_IDIR = -ID:/Dev/Repositories/tinycc

IDIR = -I../include $(TCC_IDIR)
LDIR = -Llib

ifeq ($(OS), Windows_NT)
	LIBS = -ltcc
else
	LIBS = -ltcc -ldl -lpthread
endif

all: hello hello2 error fibonacci

hello:
	$(CXX) -o Hello Hello.cpp $(CXX_FLAGS) $(IDIR) $(LDIR) $(LIBS)

run-hello: hello
	@ ./Hello

hello2:
	$(CXX) -o Hello2 Hello2.cpp $(CXX_FLAGS) $(IDIR) $(LDIR) $(LIBS)

run-hello2: hello2
	@ ./Hello2

error:
	$(CXX) -o Error Error.cpp $(CXX_FLAGS) $(IDIR) $(LDIR) $(LIBS)

run-error: error
	@ ./Error

fibonacci:
	$(CXX) -o Fibonacci Fibonacci.cpp $(CXX_FLAGS) $(IDIR) $(LDIR) $(LIBS)

run-fibonacci: fibonacci
	@ ./Fibonacci

compiledb:
	compiledb --command-style --full-path --no-build make
