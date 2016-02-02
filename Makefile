FLAGS=-std=c++11 -Wall -pedantic -Wextra
all: main
main:
		gcc $(FLAGS) -O2 ahed.cpp main.cpp -o ahed
debug:
		gcc $(FLAGS) -ggdb3 ahed.cpp main.cpp -o ahed


