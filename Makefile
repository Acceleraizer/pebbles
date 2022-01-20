.PHONY: make test

make: pebbles.cpp main.cpp
	g++ -c pebbles.cpp -O3; g++ -c main.cpp -O3; g++ main.o pebbles.o -o main.out -O3

test: pebbles.cpp tests.cpp
	g++ -c pebbles.cpp -O3; g++ -c tests.cpp; g++ tests.o pebbles.o -o tests.out