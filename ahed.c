/*
 * Autor: Jmeno Prijmeni (login)
 * Datum:
 * Soubor:
 * Komentar:
 */ 

#include "ahed.h"

/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupní soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - pøi kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
	return AHEDOK;
}

/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupní soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - pøi dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{

	ahed->codedSize = 0;
	ahed->uncodedSize = 0;

	t_node * root_node = NULL;

	// TODO zkonstruovat strom

	t_node * node = root_node;
	int c;

	while((c = fgetc(inputFile)) != EOF)
	{
		if(c == '1')
			node = node->right;
		else if(c == '0')
			node = node->left;
		else
			return AHEDFail; 

		if(node->symbol >= 0)
		{
			ahed->uncodedSize++;
			fprintf(outputFile, "%c", node->symbol);
			node = root_node;
		}

		ahed->codedSize++;
	}

	return node == root_node ? AHEDOK : AHEDFail;
}



