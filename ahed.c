/*
 * Autor: Jmeno Prijmeni (login)
 * Datum:
 * Soubor:
 * Komentar:
 */ 

#include "ahed.h"

t_node * create_node(int32_t symbol, t_node ** pointer)
{
	t_node * new = malloc(sizeof(t_node));

	if(new == NULL)
		return NULL;

	if(pointer != NULL)
		*pointer = new;

	new->symbol = symbol;
	new->frequency = 0;

	return new;
}

void free_tree(t_node * root_node)
{
	if(root_node == NULL) return;

	free_tree(root_node->left);
	free_tree(root_node->right);

	free(root_node);
}

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
	ahed->codedSize = 0;
	ahed->uncodedSize = 0;

	t_node * root_node = create_node(-1, NULL);
	int16_t c;

	while((c = fgetc(inputFile)) != EOF)
	{
		if(ahed->uncodedSize == 0) // set first character plain
		{
			root_node->symbol = c;
		    root_node->frequency++;
			fprintf("%c", c);
		}

		ahed->uncodedSize++;
	}

	free_tree(root_node);

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

	t_node * root_node = create_node(-1, NULL);

	t_node * node = root_node;
	int16_t c;
	bool bit;

	while((c = fgetc(inputFile)) != EOF)
	{
		int i = 8;
		while(i-- > 0)
		{
			bit = GET_MSB(c);
			SHIFT_LEFT(c);

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

	free_tree(root_node);
	// should stop at leaf node, othwerwise error
	return node == root_node ? AHEDOK : AHEDFail;
}



