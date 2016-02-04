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

	if(*file == NULL)
		return AHEDFail;

	return AHEDOK;
}


int main(int argc, char **argv)
{
	int (*f)(tAHED *, FILE *, FILE*) = NULL;
	FILE * ofile = NULL;
	FILE * ifile = NULL;
	FILE * logfile = NULL;
	tAHED ahed;
	char c;
	int res = AHEDOK;

	while((c = getopt(argc, argv, "i:o:l:cxh")) != -1)
	{
		switch(c) 
		{
			case 'i':
				if(open_file(&ifile, "r", optarg) == AHEDFail)
					printf("Failed to open %s\n", optarg), res = AHEDFail;
				break;
			case 'o':
				if(open_file(&ofile, "w+", optarg) == AHEDFail)
					printf("Failed to open %s\n", optarg), res = AHEDFail;
				break;
			case 'l':
				if(open_file(&logfile, "w+", optarg) == AHEDFail)
					printf("Failed to open %s\n", optarg), res = AHEDFail;
				break;
			case 'c':
				f = AHEDEncoding;
				break;
			case 'x':
				f = AHEDDecoding;
				break;
			case 'h':
				print_help();
			     goto END;
		}
	}

	if(res == AHEDOK)
	{

		if(ifile == NULL)
			ifile = stdin;

		if(ofile == NULL)
			ofile = stdout;

		if(f == NULL)
		{
			puts("No mode specified");
			print_help();
			res = AHEDFail;
		}
		else
		{
			res = f(&ahed, ifile, ofile);
		}
	}

	if(res == AHEDOK && logfile)
	{
		fprintf(logfile, "login = xvokra00\n");
		fprintf(logfile, "uncodedSize = %ld\n", ahed.uncodedSize);
		fprintf(logfile, "codedSize = %ld\n", ahed.codedSize);
	}

END:
	if(ifile != NULL)
		fclose(ifile);
	if(ofile != NULL)
		fclose(ofile);
	if(logfile != NULL)
		fclose(logfile);

	return res * -1;
}


	
