/*
 * Autor: Lukáš Vokráčko (xvokra00)
 * Datum: 11.2
 * Soubor: ahed.h
 * Komentar: Struktury a makra pro kodovani a dekodovani adaptivniho huffmanova kodu
 */

#ifndef __KKO_AHED_H__
#define __KKO_AHED_H__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define AHEDOK 0
#define AHEDFail -1

typedef struct{
	uint64_t uncodedSize;
	uint64_t codedSize;
} tAHED;

typedef struct tree_node
{
	struct tree_node * left;
	struct tree_node * right;
	struct tree_node * parent;
	uint64_t weight;
	uint16_t symbol;
	uint16_t number; // for finding node with lower number
} tree_node;

typedef struct t_buffer
{
	unsigned char buff;
	uint8_t pos; // position in buffer
	uint64_t counter; // number of characters printed to outputFile
	FILE * outputFile;
} t_buffer;

#define SYMBOL_COUNT 257 // 256 + delimiter
#define DELIMITER (SYMBOL_COUNT-1)
#define NOT_SYMBOL SYMBOL_COUNT // for node who does not have symbol

#define GET_BIT(item, pos) (item >> (7 - pos)) & 1
#define SET_BIT(item, bit, pos) item &= ~(1 << (7 - pos)) ; item |= (bit << (7 - pos))


int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile);
#endif

