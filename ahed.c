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
	bool bit;

	while((c = fgetc(inputFile)) != EOF)
	{
		int i = 8;
		while(i-- > 0)
		{
			bit = GET_MSB(c);
			SHIFT(c);

			node = bit == 1 ? node->right : node->left;

			if(node->symbol >= 0)
			{
				ahed->uncodedSize++;
				fprintf(outputFile, "%c", node->symbol);
				node = root_node;
			}
		}

		ahed->codedSize++;
	}

	// should stop at leaf node, othwerwise error
	return node == root_node ? AHEDOK : AHEDFail;
}



