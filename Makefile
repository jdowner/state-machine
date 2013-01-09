CC=g++
CXXFLAGS=-Wall -g -std=c++0x
LDFLAGS=

HDR=$(wildcard *.h)
SRC=$(wildcard *.cpp)
OBJ=$(patsubst %.cpp,%.o,$(SRC))
EXEC=statemachine
ARCHIVE=$(EXEC).tar.gz

default: $(SRC) compile build

compile: $(SRC)
	$(CC) -c $< $(CXXFLAGS) 

build: $(OBJ)
	$(CC) -o $(EXEC) $(LDFLAGS) $(OBJ)

clean:
	rm -f $(EXEC)
	rm -f $(OBJ)
	rm -f $(ARCHIVE)

archive:
	tar -zcf $(ARCHIVE) $(SRC) $(HDR) Makefile

