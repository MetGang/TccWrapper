CXX = g++
FLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic

IDIR = -I../include

LDIR = -L.
LIBS = -ltcc

SRC = main.cpp
OUT = a.exe

all:
	$(CXX) -o $(OUT) $(SRC) $(FLAGS) $(IDIR) $(LDIR) $(LIBS) && ./$(OUT)
