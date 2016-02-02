/*
 * Autor: Jmeno Prijmeni (login)
 * Datum:
 * Soubor:
 * Komentar:
 */ 
#include <stdlib.h>
#include "ahed.h"
#include <getopt.h>

enum 
{
	CODE,
	DECODE
};

void print_help()
{
	puts("ahed [-o ofile] [-i ifile] [-l logfile] [-c|-x|-h]");
	puts("\t -o ofile - output file, stdout if not specified");
	puts("\t -i ifile - input file, stdin if not specified");
	puts("\t -l logfile");
	puts("\t -h print help");
	puts("\t -c encode");
	puts("\t -x decode");
}

int open_file(FILE ** file, const char * mode, const char * path)
{
	*file = fopen(path, mode);

	if(*file < 0)
		return -1;

	return 1;

}


int main(int argc, char **argv)
{
	FILE * ofile = stdout;
	FILE * ifile = stdin;
	FILE * logfile;
	char c;
	int mode;
	int res;

	while((c = getopt(argc, argv, "i:o:l:cxh")) != -1)
	{
		switch(c) 
		{
			case 'i':
				open_file(&ifile, "r", optarg);
				break;
			case 'o':
				open_file(&ofile, "w+", optarg);
				break;
			case 'l':
				open_file(&logfile, "w+", optarg);
				break;
			case 'c':
				mode = CODE;
				break;
			case 'x':
				mode = DECODE;
				break;
			case 'h':
				print_help();
				return 0;
		}
	}

	return res;
}


	
