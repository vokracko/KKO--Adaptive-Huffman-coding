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
		return 0;

	return 1;

}


int main(int argc, char **argv)
{
	int (*f)(tAHED *, FILE *, FILE*) = NULL;
	FILE * ofile = NULL;
	FILE * ifile = NULL;
	FILE * logfile = NULL;
	tAHED ahed;
	char c;
	int res = 0;

	while((c = getopt(argc, argv, "i:o:l:cxh")) != -1)
	{
		switch(c) 
		{
			case 'i':
				open_file(&ifile, "r", optarg) || printf("Failed to open %s\n", optarg), res -1;
				break;
			case 'o':
				open_file(&ofile, "w+", optarg) || printf("Failed to open %s\n", optarg), res -1;
				break;
			case 'l':
				open_file(&logfile, "w+", optarg) || printf("Failed to open %s\n", optarg), res -1;
				break;
			case 'c':
				f = AHEDEncoding;
				break;
			case 'x':
				f = AHEDDecoding;
				break;
			case 'h':
				print_help();
				res = 0;
		}
	}

	if(res < 0)
	{

		if(ifile == NULL)
			ifile = stdin;

		if(ofile == NULL)
			ofile = stdout;

		if(f == NULL)
		{
			puts("No mode specified");
			print_help();
			res = -1;
		}
		else
		{
			res = f(&ahed, ifile, ofile);
		}
	}

	if(res >= 0 && logfile)
	{
		fprintf(logfile, "login = xvokra00\n");
		fprintf(logfile, "uncodedSize = %d\n", ahed.uncodedSize);
		fprintf(logfile, "codedSize = %d\n", ahed.codedSize);
	}

	fclose(ifile);
	fclose(ofile);
	fclose(logfile);

	return res * -1;
}


	
