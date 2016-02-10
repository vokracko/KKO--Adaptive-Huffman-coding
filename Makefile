CC=gcc
FLAGS=-std=c11 -Wall -pedantic -Wextra

all: main
main:
		$(CC) $(FLAGS) -O2 ahed.c main.c -o ahed
debug:
		$(CC) $(FLAGS) -ggdb3 ahed.c main.c -o ahed

clean:
	rm -rf ahed *.zip

pack: clean
	cd .. ; zip -r kko.proj1.xvokra00.zip kko.proj1.xvokra00 --exclude *.git* --exclude test.sh
