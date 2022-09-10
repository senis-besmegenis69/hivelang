
/**
 * @file lex.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-10
 */

#include <lex.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>



#ifndef setupToken
#	define setupToken(_type, _storage, _next) \
		{ \
			token->type = _type; \
			token->storage = _storage; \
			token->loc = lexer->loc; \
			token->next = _next; \
		}
#endif



void Lex_destroyToken(
	struct Lex_Token* const token)
{
	assert(token != NULL);
	
	switch (token->storage)
	{
		case STORAGE_STRING:
		{
			switch (token->type)
			{
				case TYPE_IDENTIFIER:
				case TYPE_LITERAL:
				{
					free(token->value.str);
				} break;

				default:
				{
				} break;
			}
		} break;
	
		default:
		{
		} break;
	}

	free(token);
}

const char* stringifiedTypes[] =
{
	[TYPE_ADDITION] = "+", // +
	[TYPE_SUBTRACTION] = "-", // -
	[TYPE_MULTIPLICATION] = "*", // *
	[TYPE_DIVISION] = "/", // /

	[TYPE_EQUAL] = "==", // ==
	[TYPE_NOT_EQUAL] = "!=", // !=
	[TYPE_MORE_THAN] = ">", // >
	[TYPE_MORE_THAN_OR_EQUAL] = ">=", // >=
	[TYPE_LESS_THAN] = "<", // <
	[TYPE_LESS_THAN_OR_EQUAL] = "<=", // <=

	[TYPE_LEFT_PARENTHESIS] = "(", // (
	[TYPE_RIGHT_PARENTHESIS] = ")", // )
	[TYPE_LEFT_BRACKET] = "[", // [
	[TYPE_RIGHT_BRACKET] = "]", // ]
	[TYPE_COLUMN] = ":", // :

	[TYPE_IF] = "if", // if
	[TYPE_END] = "end", // end
	[TYPE_USE] = "use", // use
	[TYPE_IMPORT] = "import", // import
	[TYPE_PROC] = "proc", // proc
};

const char* stringifiedStorages[] =
{
	[STORAGE_I64] = "i64",
	[STORAGE_U64] = "u64",
	[STORAGE_F64] = "f64",
	[STORAGE_STRING] = "string",
};

void Lex_printToken(
	const struct Lex_Token* const token)
{
	assert(token != NULL);

	assert((long unsigned int)(token->type) >= 0 && (long unsigned int)(token->type) < (sizeof(stringifiedTypes) / sizeof(const char*)));
	const char* stringifiedType = stringifiedTypes[token->type];

	assert((long unsigned int)(token->storage) >= 0 && (long unsigned int)(token->storage) < (sizeof(stringifiedStorages) / sizeof(const char*)));
	const char* stringifiedStorage = stringifiedStorages[token->storage];

	fprintf(stdout, "type=%s storage=%s value=[] location=[file=%s line=%lld column=%lld]\n",
		stringifiedType, stringifiedStorage, token->loc.file, token->loc.line, token->loc.column);
}



struct Lex_Lexer Lex_createLexer(
	const char* file)
{
	struct Lex_Lexer lexer = {0};
	assert(file != NULL);

	FILE* stream = fopen(file, "r");
	assert(stream != NULL);

	fseek(stream, 0, SEEK_END);
	long fsize = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	lexer.buffer = malloc((fsize + 1) * sizeof(char));
	fread(lexer.buffer, fsize, 1, stream);
	fclose(stream);

	lexer.buffer[fsize] = 0;

	lexer.current = lexer.buffer;
	lexer.loc = (struct Lex_Location) { .file = file, .line = 1, .column = 1 };
	return lexer;
}

void Lex_destroyLexer(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	free(lexer->buffer);
}

static void Lex_moveBy(
	struct Lex_Lexer* const lexer,
	signed long long amount)
{
	assert(lexer != NULL);

	for (signed long long i = 0; i < amount; ++i)
	{
		char current = *(lexer->current + i);

		if (current == '\n')
		{
			++lexer->loc.line;
			lexer->loc.column = 1;
		}
		else
		{
			++lexer->loc.column;
		}
	}

	lexer->current += amount;
}

static void Lex_skipWhitespaces(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	signed long long i = 0;

	while (1)
	{
		char current = *(lexer->current + i);

		if (current == ' ' || current == '\t' || current == '\n')
		{
			break;
		}

		++i;
	}

	Lex_moveBy(lexer, i);
}

static void Lex_skipSingleLineComment(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '/');

	Lex_skipWhitespaces(lexer);
	signed long long i = 0;

	while (1)
	{
		if (*(lexer->current + i) == '\n')
		{
			break;
		}

		++i;
	}

	Lex_moveBy(lexer, i);
}

static void Lex_skipMultiLineComment(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '*');

	Lex_skipWhitespaces(lexer);
	signed long long i = 0;

	for (; *(lexer->current + i) != '*' && *(lexer->current + i + 1) != '/'; ++i)
	Lex_moveBy(lexer, i);
}

static int Lex_isIdentifierFirstChar(
	const char c)
{
	return c == '_'
		|| c == '?'
		|| c == '@'
		|| (c >= 65 /* A */ && c <= 90 /* Z */)
		|| (c >= 97 /* a */ && c <= 122 /* z */);
}

static int Lex_isIdentifierAnyButFirstChar(
	const char c)
{
	return c == '_'
		|| c == '?'
		|| c == '@'
		|| (c >= 65 /* A */ && c <= 90 /* Z */)
		|| (c >= 97 /* a */ && c <= 122 /* z */)
		|| (c >= 48 /* 0 */ && c <= 57 /* 9 */);
}

static int Lex_tryParseIdentifier(
	struct Lex_Lexer* const lexer,
	struct Lex_Token* const token)
{
	assert(lexer != NULL);

	if (!Lex_isIdentifierFirstChar(*lexer->current))
	{
		setupToken(TYPE_ERROR, STORAGE_NONE, NULL);
		Lex_moveBy(lexer, 1);
		return 0;
	}

	assert(token != NULL);
	signed long long i = 1;

	for (; Lex_isIdentifierAnyButFirstChar(*(lexer->current + i)); ++i);

	setupToken(TYPE_IDENTIFIER, STORAGE_STRING, NULL);
	token->value.str = (char*)malloc((i + 1) * sizeof(char));
	assert(token->value.str != NULL);
	memcpy(token->value.str, lexer->current, i);
	token->value.str[i] = 0;
	Lex_moveBy(lexer, i);
	return 1;
}

static int Lex_tryParseNumericLiteral(
	struct Lex_Lexer* const lexer,
	struct Lex_Token* const token)
{
	assert(lexer != NULL);

	if (!isdigit(*lexer->current))
	{
		return 0;
	}

	assert(token != NULL);
	signed long long i = 1;
	signed long long dots = 0;

	for (; isdigit(*(lexer->current + i)) || *(lexer->current + i) == '.'; ++i)
	{
		if (*(lexer->current + i) == '.')
		{
			++dots;
		}
	}

	if (dots == 1)
	{
		// float
		setupToken(TYPE_LITERAL, STORAGE_F64, NULL);

		char* buffer = (char*)malloc((i + 1) * sizeof(char));
		assert(buffer != NULL);
		memcpy(buffer, lexer->current, i);
		buffer[i] = 0;

		long double value = 0;
		sscanf(buffer, "%Lf", &value);

		token->value.f64 = value;
		Lex_moveBy(lexer, i);
	}
	else if (dots <= 0)
	{
		// int
		setupToken(TYPE_LITERAL, STORAGE_I64, NULL);

		char* buffer = (char*)malloc((i + 1) * sizeof(char));
		assert(buffer != NULL);
		memcpy(buffer, lexer->current, i);
		buffer[i] = 0;

		signed long long value = 0;
		sscanf(buffer, "%lld", &value);

		token->value.i64 = value;
		Lex_moveBy(lexer, i);
	}
	else
	{
		// Error
		setupToken(TYPE_ERROR, STORAGE_NONE, NULL);
		Lex_moveBy(lexer, i);
	}
	
	return 1;
}

struct Lex_Token* Lex_nextToken(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);

	struct Lex_Token* token = (struct Lex_Token*)malloc(sizeof(struct Lex_Token));
	assert(token != NULL);

begining:
	Lex_skipWhitespaces(lexer);

	switch (*lexer->current)
	{
		case '+':
		{
			setupToken(TYPE_ADDITION, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case '-':
		{
			setupToken(TYPE_SUBTRACTION, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case '*':
		{
			setupToken(TYPE_MULTIPLICATION, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case '/':
		{
			switch (*(lexer->current + 1))
			{
				case '/':
				{
					Lex_skipSingleLineComment(lexer);
					goto begining;
				} break;

				case '*':
				{
					Lex_skipMultiLineComment(lexer);
					goto begining;
				} break;

				default:
				{
					setupToken(TYPE_DIVISION, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '=':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					setupToken(TYPE_EQUAL, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{

				} break;
			}
		} break;

		case '!':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					setupToken(TYPE_NOT_EQUAL, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{

				} break;
			}
		} break;

		case '>':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					setupToken(TYPE_MORE_THAN_OR_EQUAL, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					setupToken(TYPE_MORE_THAN, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '<':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					setupToken(TYPE_LESS_THAN_OR_EQUAL, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					setupToken(TYPE_LESS_THAN, STORAGE_NONE, NULL);
					Lex_moveBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '(':
		{
			setupToken(TYPE_LEFT_PARENTHESIS, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case ')':
		{
			setupToken(TYPE_RIGHT_PARENTHESIS, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case '[':
		{
			setupToken(TYPE_LEFT_BRACKET, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case ']':
		{
			setupToken(TYPE_RIGHT_BRACKET, STORAGE_NONE, NULL);
			Lex_moveBy(lexer, 1);
			goto end;
		} break;

		case '"':
		{
			signed long long i = 1;
			for (; *(lexer->current + i) != '"'; ++i);

			setupToken(TYPE_LITERAL, STORAGE_STRING, NULL);
			token->value.str = (char*)malloc((i + 1) * sizeof(char));
			assert(token->value.str != NULL);
			memcpy(token->value.str, lexer->current, i);
			token->value.str[i] = 0;

			Lex_moveBy(lexer, i + 1);
		} break;

		default:
		{
			if (Lex_tryParseIdentifier(lexer, token))
			{
				goto end;
			}

			if (Lex_tryParseNumericLiteral(lexer, token))
			{
				goto end;
			}
		} break;
	}

end:
	return token;
}
