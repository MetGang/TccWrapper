CXX = g++
FLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

IDIR = -I../include

LDIR =
LIBS =

SRC = main.cpp
OUT =

ifeq ($(OS), Windows_NT)
	LDIR = -Lwin32
	LIBS = -ltcc
	OUT = a.exe
else
	LDIR = -Llinux
	LIBS = -ltcc -ldl
	OUT = a.out
endif

all:
	$(CXX) -o $(OUT) $(SRC) $(FLAGS) $(IDIR) $(LDIR) $(LIBS) && ./$(OUT)
