FLAGS=-std=c99 -Wall -pedantic -Wextra

all: main
main:
		gcc $(FLAGS) -O2 ahed.c main.c -o ahed
debug:
		gcc $(FLAGS) -ggdb3 ahed.c main.c -o ahed

clean:
	rm -rf ahed *.zip

pack:
	zip kko.proj1.xvokra00.zip ahed.* main.* Makefile  
