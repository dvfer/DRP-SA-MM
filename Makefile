CC = g++
CFLAGS = -std=c++17 -Wall

all: solver

solver: main.o functions.o
	$(CC) $(CFLAGS) main.o functions.o -o solver

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

functions.o: src/functions.cpp include/functions.h
	$(CC) $(CFLAGS) -c src/functions.cpp -o functions.o

clean:
	rm -rf *.o solver
