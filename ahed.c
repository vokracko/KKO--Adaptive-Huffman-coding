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

bool construct_tree(tree_node ** tree, tree_node * symbol_array[])
{
	*tree = malloc(sizeof(tree_node));

	if(*tree == NULL)
		return false;

	(*tree)->weight = 0;
	(*tree)->symbol = DELIMITER;
	(*tree)->left = (*tree)->right = (*tree)->parent = NULL;
	(*tree)->number = 0;
	symbol_array[DELIMITER] = *tree;

	return true;
}


bool construct_subtree(tree_node * symbol_array[], uint16_t symbol)
{
	tree_node * delimiter_node = symbol_array[DELIMITER]; // create subtree in delimiter node;
	tree_node * left = malloc(sizeof(tree_node)); 
	tree_node * right = malloc(sizeof(tree_node));

	if(left == NULL || right == NULL)
	{
		free(left);
		free(right);
		return false;
	}

	delimiter_node->left = left;
	delimiter_node->right = right;
	delimiter_node->symbol = NOT_SYMBOL;

	left->parent = right->parent = delimiter_node;
	left->left = right->left = right->right = left->right = NULL;
	left->symbol = DELIMITER;
	left->weight = 0;
	left->number = delimiter_node->number + 2;

	right->symbol = symbol;
	right->weight = 0;
	right->number = delimiter_node->number + 1;

	symbol_array[DELIMITER] = left;
	symbol_array[symbol] = right;

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
void encode_symbol(FILE * outputFile, tree_node * node, t_buffer  * buffer)
{
	bool bit;
	char code = 0;
	int code_pos = 0;

	// save code in inverted order
	while(node->parent != NULL)
	{
		bit = node == node->parent->right;
		SET_BIT(code, bit, code_pos);
		code_pos++;

		node = node->parent;
	}

	// reverse code to correct order and write it out
	while(code_pos > 0)
	{
		code_pos--;
		bit = GET_BIT(code, code_pos);
		SET_BIT(buffer->buff, bit, buffer->pos);
		buffer->pos++;

		if(buffer->pos == 8)
		{
			fprintf(outputFile, "%c", buffer->buff);
			buffer->pos = 0;
			buffer->buff = 0;
			buffer->counter++;
		}
	}
}

void plain_symbol(FILE * outputFile, char c, t_buffer * buffer)
{
	bool bit;

	for(int i = 0; i < 8; ++i)
	{
		bit = GET_BIT(c, i);
		SET_BIT(buffer->buff, bit, buffer->pos);
		buffer->pos++;


		if(buffer->pos == 8)
		{
			fprintf(outputFile, "%c", buffer->buff);
			buffer->pos = 0;
			buffer->counter++;
			buffer->buff = 0;
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

	tree_node * symbol_array[SYMBOL_COUNT] = {NULL};
	tree_node * symbol_tree = NULL;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	while((c = fgetc(inputFile)) != EOF)
	{
		if(symbol_array[c] == NULL) // new symbol
		{

			if(ahed->uncodedSize != 0) // first delimiter
				encode_symbol(outputFile, symbol_array[DELIMITER], &code_buffer);
			
			if(!construct_subtree(symbol_array, c)) // insert char to tree
			{
				destroy_tree(symbol_tree);
				return AHEDFail;
			}

			// then plain symbol
			plain_symbol(outputFile, c, &code_buffer);
		}
		else // symbol already has code
			encode_symbol(outputFile, symbol_array[c], &code_buffer);

		symbol_array[c]->weight++;
		ahed->uncodedSize++;
		adapt_tree(symbol_array[c]->parent);
	}

	// delimiter is used as EOF in this case
	encode_symbol(outputFile, symbol_array[DELIMITER], &code_buffer);

	// print delimiter as EOF + garbage if wasnt printed in encode_symbol
	if(code_buffer.pos != 0)
	{
		fprintf(outputFile, "%c", code_buffer.buff);
		code_buffer.counter++;
	}

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

	tree_node * symbol_array[SYMBOL_COUNT] = {NULL};
	tree_node * symbol_tree = NULL;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	bool bit;
	bool delimiter = true;

	tree_node * node = symbol_tree;

	while((c = fgetc(inputFile)) != EOF)
	{
		for(int i = 0; i < 8; ++i)
		{
			bit = GET_BIT(c, i);

			if(delimiter == true)
			{
				SET_BIT(char_buffer.buff, bit, char_buffer.pos);
				char_buffer.pos++;

				if(char_buffer.pos == 8)
				{
					if(!construct_subtree(symbol_array, char_buffer.buff))
					{
						destroy_tree(symbol_tree);
						return AHEDFail;
					}

					fprintf(outputFile, "%c", char_buffer.buff);
					symbol_array[char_buffer.buff]->weight++;
					adapt_tree(symbol_array[char_buffer.buff]->parent);
					char_buffer.pos = 0;
					char_buffer.buff = 0;
					delimiter = false;
					ahed->uncodedSize++;
				}

				continue;
			}

			node = bit == 1 ? node->right : node->left;
			if(node == NULL)
			{
				destroy_tree(symbol_tree);
				return AHEDFail;
			}
			if(node->symbol == DELIMITER)
			{
				delimiter = true;
				node = symbol_tree;
			}
			else if(node->symbol < DELIMITER)
			{
				ahed->uncodedSize++;
				fprintf(outputFile, "%c", node->symbol);
				node->weight++;
				adapt_tree(node->parent);
				node = symbol_tree;
			}
		}

		ahed->codedSize++;
	}

	destroy_tree(symbol_tree);
	// delimiter was used as EOF
	return delimiter == true ? AHEDOK : AHEDFail;
}



