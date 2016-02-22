CC=gcc
FLAGS=-std=c99 -Wall -pedantic -Wextra

all: main
main:
		$(CC) $(FLAGS) -O2 ahed.c main.c -o ahed
debug:
		$(CC) $(FLAGS) -ggdb3 ahed.c main.c -o ahed

doc:
	pdflatex ahed.tex

clean:
	rm -rf ahed *.zip *.aux *.log *.out ../kko.proj1.xvokra00.zip 2> /dev/null

pack: clean
	mv test.sh ../
	mv ahed.tex ../
	cd .. ; zip -r kko.proj1.xvokra00.zip kko.proj1.xvokra00 --exclude *.git* 
	mv ../test.sh .
	mv ../ahed.tex .
