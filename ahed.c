/*
 * Autor: Lukáš Vokráčko (xvokra00)
 * Datum: 11.2
 * Soubor: ahed.c
 * Komentar: Funkce pro kodovani a dekodovani adaptivniho huffmanova kodu
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

/**
 * \brief Construct root node for tree
 * \param tree Where to save pointer to root
 * \param symbol_array array of symbols, insert NYT to correct position
*/
bool construct_tree(tree_node ** tree, tree_node * symbol_array[])
{
	*tree = malloc(sizeof(tree_node));

	if(*tree == NULL)
		return false;

	(*tree)->weight = 0;
	(*tree)->symbol = NYT;
	(*tree)->left = (*tree)->right = (*tree)->parent = NULL;
	(*tree)->number = 0;
	symbol_array[NYT] = *tree;

	return true;
}

/**
 * \brief Construct subtree for new symbol in place of NYT and move NYT to new position
 * \param symbol_array
 * \param symbol
*/
bool construct_subtree(tree_node * symbol_array[], uint16_t symbol)
{
	tree_node * nyt_node = symbol_array[NYT]; // create subtree in NYT node;
	tree_node * left = malloc(sizeof(tree_node));
	tree_node * right = malloc(sizeof(tree_node));

	if(left == NULL || right == NULL)
	{
		free(left);
		free(right);
		return false;
	}

	nyt_node->left = left;
	nyt_node->right = right;
	nyt_node->symbol = NOT_SYMBOL;

	left->parent = right->parent = nyt_node;
	left->left = right->left = right->right = left->right = NULL;
	left->symbol = NYT;
	left->weight = 0;
	left->number = nyt_node->number + 2;

	right->symbol = symbol;
	right->weight = 0;
	right->number = nyt_node->number + 1;

	symbol_array[NYT] = left;
	symbol_array[symbol] = right;
	symbol_array[SYMBOL_COUNT + nyt_node->number/2] = nyt_node;

	return true;
}

/**
 * \brief Adapt tree if it is not huffman anymore and increment weight of c node
 * \param symbol_array
 * \param c symbol received from inputFile
*/
void adapt_tree(tree_node * symbol_array[], uint16_t c)
{
	tree_node * updated = symbol_array[c];

	while(updated != NULL)
	{
		tree_node * smallest_node = updated;

		// find node in same block with smallest number
		for(int i = 0; i < 2 * SYMBOL_COUNT; ++i)
		{
			tree_node * current = symbol_array[i];
			if(current != NULL && current->weight == updated->weight && current->number < smallest_node->number)
			{
				smallest_node = current;
			}
		}


		// if not in position with smallest number
		if(smallest_node != updated && smallest_node != updated->parent)
		{
			tree_node * tmp_parent = updated->parent;
			uint16_t tmp_number = updated->number;

			if(updated->parent->right == updated)
				updated->parent->right = smallest_node;
			else
				updated->parent->left = smallest_node;

			if(smallest_node->parent->right == smallest_node)
				smallest_node->parent->right = updated;
			else
				smallest_node->parent->left = updated;

			updated->parent = smallest_node->parent;
			updated->number = smallest_node->number;
			smallest_node->parent = tmp_parent;
			smallest_node->number = tmp_number;
		}

		updated->weight++;
		updated = updated->parent;
	}
}

/**
 * \brief Print character in buffer to output file
 * \param buffer
*/
void flush_buffer(t_buffer * buffer)
{
	fprintf(buffer->outputFile, "%c", buffer->buff);
	buffer->pos = 0;
	buffer->buff = 0;
	buffer->counter++;
}

/**
 * \brief Construct symbol code from tree
 * \param node node containing received symbol
 * \param buffer where to write the code
*/
void encode_symbol(tree_node * node, t_buffer  * buffer)
{
	bool bit;
	char code[SYMBOL_COUNT / 8 + 1] = {0};
	int code_pos = 0;

	// save code in inverted order
	while(node->parent != NULL)
	{
		bit = node == node->parent->right;
		SET_BIT(code[code_pos / 8], bit, code_pos % 8);
		code_pos++;
		node = node->parent;
	}

	while(code_pos > 0)
	{
		code_pos--;
		bit = GET_BIT(code[code_pos / 8], code_pos % 8);
		SET_BIT(buffer->buff, bit, buffer->pos);
		buffer->pos++;

		if(buffer->pos == 8)
			flush_buffer(buffer);
	}
}

/**
 * \brief Print plain symbol into the buffer
 * \param c
 * \param buffer
*/
void plain_symbol(char c, t_buffer * buffer)
{
	bool bit;

	for(int i = 0; i < 8; ++i)
	{
		bit = GET_BIT(c, i);
		SET_BIT(buffer->buff, bit, buffer->pos);
		buffer->pos++;


		if(buffer->pos == 8)
			flush_buffer(buffer);
	}
}


int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
	ahed->codedSize = 0;
	ahed->uncodedSize = 0;
	int16_t c;
	t_buffer code_buffer;

	code_buffer.pos = 0;
	code_buffer.counter = 0;
	code_buffer.buff = 0;
	code_buffer.outputFile = outputFile;

	tree_node * symbol_array[2 * SYMBOL_COUNT] = {NULL};
	tree_node * symbol_tree = NULL;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	while((c = fgetc(inputFile)) != EOF)
	{
		if(symbol_array[c] == NULL) // new symbol
		{
			if(ahed->uncodedSize != 0) // not first symbol, write out NYT
				encode_symbol(symbol_array[NYT], &code_buffer);

			if(!construct_subtree(symbol_array, c)) // insert char to tree
			{
				destroy_tree(symbol_tree);
				return AHEDFail;
			}

			// then plain symbol
			plain_symbol(c, &code_buffer);
		}
		else // symbol already has code
			encode_symbol(symbol_array[c], &code_buffer);

		ahed->uncodedSize++;
		adapt_tree(symbol_array, c);
	}

	// NYT is used as EOF in this case
	encode_symbol(symbol_array[NYT], &code_buffer);

	// print NYT as EOF + garbage if wasnt printed in encode_symbol
	if(code_buffer.pos != 0)
	{
		fprintf(outputFile, "%c", code_buffer.buff);
		code_buffer.counter++;
	}

	destroy_tree(symbol_tree);

	ahed->codedSize = code_buffer.counter;

	return AHEDOK;
}

int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
	ahed->codedSize = 0;
	ahed->uncodedSize = 0;
	int16_t c;
	t_buffer char_buffer;

	char_buffer.pos = 0;
	char_buffer.counter = 0;
	char_buffer.buff = 0;
	char_buffer.outputFile = outputFile;

	tree_node * symbol_array[2 * SYMBOL_COUNT] = {NULL};
	tree_node * symbol_tree = NULL;

	if(!construct_tree(&symbol_tree, symbol_array))
		return AHEDFail;

	bool bit;
	bool nyt = true;

	tree_node * node = symbol_tree;

	while((c = fgetc(inputFile)) != EOF)
	{
		for(int i = 0; i < 8; ++i) // for every bit
		{
			bit = GET_BIT(c, i);

			if(nyt == true) // if previous symbol was NYT receive plain symbol
			{
				SET_BIT(char_buffer.buff, bit, char_buffer.pos);
				char_buffer.pos++;

				if(char_buffer.pos == 8) // we have whole symbol, create its node in tree
				{
					if(!construct_subtree(symbol_array, char_buffer.buff))
					{
						destroy_tree(symbol_tree);
						return AHEDFail;
					}

					adapt_tree(symbol_array, char_buffer.buff);
					flush_buffer(&char_buffer);
					nyt = false;
					ahed->uncodedSize++;
				}

				continue; // do not traverse tree
			}

			// traverse tree by bits until node with symbol, the print its symbol
			node = bit == 1 ? node->right : node->left;
			if(node == NULL)
			{
				destroy_tree(symbol_tree);
				return AHEDFail;
			}
			if(node->symbol == NYT)
			{
				nyt = true;
				node = symbol_tree;
			}
			else if(node->symbol < NYT)
			{
				ahed->uncodedSize++;
				fprintf(outputFile, "%c", node->symbol);
				adapt_tree(symbol_array, node->symbol);
				node = symbol_tree;
			}
		}

		ahed->codedSize++;
	}

	destroy_tree(symbol_tree);
	// NYT was used as EOF
	return nyt == true ? AHEDOK : AHEDFail;
}



