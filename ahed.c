/*
 * Autor: Jmeno Prijmeni (login)
 * Datum:
 * Soubor:
 * Komentar:
 */ 

#include "ahed.h"

void destroy_tree(tree_node * symbol_tree)
{
	if(symbol_tree == NULL)
		return;

	destroy_tree(symbol_tree->left);
	destroy_tree(symbol_tree->right);

	free(symbol_tree);
}

bool construct_tree(tree_node ** symbol_tree, tree_node ** symbol_array)
{
	tree_node * node = *symbol_tree = malloc(sizeof(tree_node) * 2 * SYMBOL_COUNT);

	if(symbol_tree == NULL)
	{
		destroy_tree(*symbol_tree);
		return false;
	}

	node->parent = NULL;

	// -1 last node does not have children
	for(int i = 0; i < SYMBOL_COUNT - 1; ++i)
	{
		node->left = node + sizeof(tree_node);
		node->right = node + 2 * sizeof(tree_node);
		node->weight = 0;
		node->symbol = ~0; // to difirentiate between symbol and path node

		//right child is symbol
		symbol_array[i] = node->right;
		node->right->symbol = i;
		node->right->weight = 0;
		node->right->parent = node;
		node->right->left = NULL;
		node->right->right = NULL;
		
		//left is path node
		node->left->parent = node;
		node = node->left;
	}

	// DELIMITER is last symbol
	symbol_array[DELIMITER] = node;
	node->weight = 0;
	node->symbol = DELIMITER;
	node->left = NULL;
	node->right = NULL;
	return true;
}

// adapt tree structure if needed
void adapt_tree(tree_node * parent)
{
	tree_node * tmp;

	while(parent != NULL)
	{
		// swap left for right
		if(parent->left->weight > parent->right->weight)
		{
			tmp = parent->left;
			parent->left = parent->right;
			parent->right = tmp;
		}

		parent->weight += 1; // left + right is current weight + 1
		parent = parent->parent;
	}
}

// construct symbol code and print byte if needed
void encode_symbol(FILE * outputFile, tree_node * node, t_buffer buffer)
{
	bool bit;

	while(node->parent != NULL)
	{
		bit = node == node->parent->right;
		SET_BIT(buffer.buff, bit, buffer.pos);
		buffer.pos++;

		if(buffer.pos == 8)
		{
			fprintf(outputFile, "%c", buffer.buff);
			buffer.pos = 0;
			buffer.counter++;
		}

		node = node->parent;
	}
}

void plain_symbol(FILE * outputFile, char c, t_buffer buffer)
{
	bool bit;

	for(int i = 0; i < 8; ++i)
	{
		bit = GET_MSB(c);
		SHIFT_LEFT(c);
		SET_BIT(buffer.buff, bit, buffer.pos);
		buffer.pos++;

		if(buffer.pos == 8)
		{
			fprintf(outputFile, "%c", buffer.buff);
			buffer.pos = 0;
			buffer.counter++;
		}
	}
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
	int16_t c;
	t_buffer code_buffer;

	code_buffer.pos = 0;
	code_buffer.counter = 0;
	code_buffer.buff = 0;

	tree_node * symbol_array[SYMBOL_COUNT];
	tree_node * symbol_tree = NULL;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	while((c = fgetc(inputFile)) != EOF)
	{
		if(symbol_array[c]->weight == 0)
		{
			if(ahed->uncodedSize != 0)
				encode_symbol(outputFile, symbol_array[DELIMITER], code_buffer);
			else
				plain_symbol(outputFile, c, code_buffer);

			symbol_array[c]->weight++;
		}

		ahed->uncodedSize++;
		adapt_tree(symbol_array[c]->parent);
	}

	// delimiter is used as EOF in this case
	encode_symbol(outputFile, symbol_array[DELIMITER], code_buffer);

	// print delimiter as EOF + garbage if wasnt printed in encode_symbol
	if(code_buffer.pos != 0)
		fprintf(outputFile, "%c", code_buffer.buff);

	destroy_tree(symbol_tree);

	ahed->codedSize = code_buffer.counter;

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
	int16_t c;
	t_buffer char_buffer;

	char_buffer.pos = 0;
	char_buffer.counter = 0;
	char_buffer.buff = 0;

	tree_node * symbol_array[SYMBOL_COUNT];
	tree_node * symbol_tree = NULL;

	bool bit;
	bool delimiter = true;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	tree_node * node = symbol_tree;

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



