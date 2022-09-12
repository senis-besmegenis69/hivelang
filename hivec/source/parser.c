
/**
 * @file parser.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-11
 */

#include <parser.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void Parser_parse(
	const char* path)
{
	struct Lexer lexer = Lexer_create(path);
	struct Token* token = NULL;

	while ((token = Lexer_nextToken(&lexer))->type != TYPE_EOF)
	{
		if (token->type == TYPE_ERROR)
		{
			fprintf(stdout, "%s: error: encountered unknown token!\n", Location_stringify(&token->location));
		}

		fprintf(stdout, "%s\n", Token_stringify(token));
		Token_destroy(token);
	}

	Lexer_destroy(&lexer);
}
