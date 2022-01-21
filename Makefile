.PHONY: make test diag

make: pebbles.h pebbles.cpp main.cpp
	g++ -c pebbles.cpp -O3; g++ -c main.cpp -O3; g++ main.o pebbles.o -o main.out -O3

test: pebbles.h pebbles.cpp tests.cpp
	g++ -c pebbles.cpp -O3; g++ -c tests.cpp; g++ tests.o pebbles.o -o tests.out

diag: pebbles.h pebbles.cpp diag.cpp
	g++ -c pebbles.cpp -O3; g++ -c diag.cpp -O3; g++ diag.o pebbles.o -o diag.out -O3