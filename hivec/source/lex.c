
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

const char* Lex_stringifyLocation(
	const struct Lex_Location* const loc)
{
	assert(loc != NULL);
	#define bufferCapcity ((signed int)512)
	static char buffer[bufferCapcity];
	snprintf(buffer, bufferCapcity, "%s:%lld:%lld", loc->file, loc->line, loc->column);
	#undef bufferCapcity
	return buffer;
}

const char* Lex_stringifyType(
	const enum Lex_Type type)
{
	static const char* stringifiedTypes[] =
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

		[TYPE_IDENTIFIER] = "identifier",
		[TYPE_LITERAL] = "literal",
	};

	assert((long unsigned int)(type) < (sizeof(stringifiedTypes) / sizeof(const char*)));
	return stringifiedTypes[type];
}

const char* Lex_stringifyStorage(
	const enum Lex_Storage storage)
{
	static const char* stringifiedStorages[] =
	{
		[STORAGE_NONE] = "none",
		[STORAGE_I64] = "i64",
		[STORAGE_U64] = "u64",
		[STORAGE_F64] = "f64",
		[STORAGE_STRING] = "string",
	};

	assert((long unsigned int)(storage) < (sizeof(stringifiedStorages) / sizeof(const char*)));
	return stringifiedStorages[storage];
}

const char* Lex_stringifyValue(
	const enum Lex_Storage storage,
	const union Lex_Value value)
{
	#define bufferCapcity ((signed int)512)
	static char buffer[bufferCapcity];

	switch (storage)
	{
		case STORAGE_NONE:
		{
			snprintf(buffer, bufferCapcity, "%s", "none");
		} break;

		case STORAGE_I64:
		{
			snprintf(buffer, bufferCapcity, "%lld", value.i64);
		} break;

		case STORAGE_U64:
		{
			snprintf(buffer, bufferCapcity, "%llu", value.u64);

		} break;

		case STORAGE_F64:
		{
			snprintf(buffer, bufferCapcity, "%Lf", value.f64);
		} break;

		case STORAGE_STRING:
		{
			snprintf(buffer, bufferCapcity, "%s", value.str);
		} break;

		default:
		{
			snprintf(buffer, bufferCapcity, "%s", "unknown");
		} break;
	}

	#undef bufferCapcity
	return buffer;
}

static struct Lex_Token* Lex_createToken(
	void)
{
	struct Lex_Token* token = (struct Lex_Token*)malloc(sizeof(struct Lex_Token));
	assert(token != NULL);
	return token;
}

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

static void Lex_setupToken(
	struct Lex_Token* const token,
	const enum Lex_Type type,
	const enum Lex_Storage storage,
	const struct Lex_Location loc)
{
	assert(token != NULL);
	token->type = type;
	token->storage = storage;
	token->loc = loc;
}

const char* Lex_stringifyToken(
	const struct Lex_Token* const token)
{
	#define bufferCapcity ((signed int)1536)
	static char buffer[bufferCapcity];

	snprintf(buffer, bufferCapcity, "type=%s storage=%s value=%s location=%s",
		Lex_stringifyType(token->type),
		Lex_stringifyStorage(token->storage),
		Lex_stringifyValue(token->storage, token->value),
		Lex_stringifyLocation(&token->loc));

	#undef bufferCapcity
	return buffer;
}

static void Lex_moveLexerBy(
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
	while (*lexer->current == ' ' || *lexer->current == '\t' || *lexer->current == '\n' || *lexer->current == '\r')
		Lex_moveLexerBy(lexer, 1);
}

static void Lex_skipSingleLineComment(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '/');
	Lex_moveLexerBy(lexer, 2);

	while (*lexer->current != '\n')
		Lex_moveLexerBy(lexer, 1);
	
	Lex_moveLexerBy(lexer, 1);
}

static void Lex_skipMultiLineComment(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '*');
	Lex_moveLexerBy(lexer, 2);

	while (*lexer->current != '*' && *(lexer->current + 1) != '/')
		Lex_moveLexerBy(lexer, 1);

	Lex_moveLexerBy(lexer, 2);
}

static signed int Lex_tryParseKeyword(
	struct Lex_Lexer* const lexer,
	struct Lex_Token* const token)
{
	assert(lexer != NULL);
	assert(token != NULL);

	const char* keywords[] =
	{ "if", "end", "use", "import", "proc" };

	enum Lex_Type types[] =
	{ TYPE_IF, TYPE_END, TYPE_USE, TYPE_IMPORT, TYPE_PROC };

	for (signed int i = 0; i < 5; ++i)
	{
		if (strncmp(lexer->current, keywords[i], strlen(keywords[i])) == 0)
		{
			Lex_setupToken(token, types[i], STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, strlen(keywords[i]));
			return 1;
		}
	}

	return 0;
}

static signed int Lex_isIdentifierChar(
	const char c)
{
	return c == '_'
		|| c == '?'
		|| c == '@'
		|| (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| isdigit(c);
}

static signed int Lex_tryParseIdentifier(
	struct Lex_Lexer* const lexer,
	struct Lex_Token* const token)
{
	assert(lexer != NULL);
	assert(token != NULL);

	if (!(Lex_isIdentifierChar(*lexer->current) && !(isdigit(*lexer->current))))
	{
		return 0;
	}

	Lex_setupToken(token, TYPE_IDENTIFIER, STORAGE_STRING, lexer->loc);
	signed long long i = 1;

	for (; Lex_isIdentifierChar(*(lexer->current + i)); ++i);

	token->value.str = (char*)malloc((i + 1) * sizeof(char));
	assert(token->value.str != NULL);
	memcpy(token->value.str, lexer->current, i);
	token->value.str[i] = 0;

	Lex_moveLexerBy(lexer, i);
	return 1;
}

static signed int Lex_tryParseNumericLiteral(
	struct Lex_Lexer* const lexer,
	struct Lex_Token* const token)
{
	assert(lexer != NULL);
	assert(token != NULL);

	if (!isdigit(*lexer->current))
	{
		return 0;
	}

	signed long long i = 1;
	signed int dots = 0;

	for (; isdigit(*(lexer->current + i)) || *(lexer->current + i) == '.'; ++i)
	{
		if (*(lexer->current + i) == '.')
		{
			++dots;
		}
	}

	char* buffer = (char*)malloc((i + 1) * sizeof(char));
	assert(buffer != NULL);
	memcpy(buffer, lexer->current, i);
	buffer[i] = 0;

	if (dots <= 0)
	{
		Lex_setupToken(token, TYPE_LITERAL, STORAGE_I64, lexer->loc);
		signed long long value;
		sscanf(buffer, "%lld", &value);
		token->value.i64 = value;
	}
	else if (dots == 1)
	{
		Lex_setupToken(token, TYPE_LITERAL, STORAGE_F64, lexer->loc);
		long double value;
		sscanf(buffer, "%Lf", &value);
		token->value.f64 = value;
	}
	else
	{
		Lex_setupToken(token, TYPE_ERROR, STORAGE_NONE, lexer->loc);
	}

	Lex_moveLexerBy(lexer, i);
	return 1;
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

struct Lex_Token* Lex_nextToken(
	struct Lex_Lexer* const lexer)
{
	assert(lexer != NULL);
	struct Lex_Token* token = Lex_createToken();

begining:
	Lex_skipWhitespaces(lexer);

	switch (*lexer->current)
	{
		case '+':
		{
			Lex_setupToken(token, TYPE_ADDITION, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case '-':
		{
			Lex_setupToken(token, TYPE_SUBTRACTION, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case '*':
		{
			Lex_setupToken(token, TYPE_MULTIPLICATION, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
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
					Lex_setupToken(token, TYPE_DIVISION, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 1);
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
					Lex_setupToken(token, TYPE_EQUAL, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Lex_setupToken(token, TYPE_ERROR, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '!':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					Lex_setupToken(token, TYPE_NOT_EQUAL, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Lex_setupToken(token, TYPE_ERROR, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '>':
		{
			switch (*(lexer->current + 1))
			{
				case '=':
				{
					Lex_setupToken(token, TYPE_MORE_THAN_OR_EQUAL, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Lex_setupToken(token, TYPE_MORE_THAN, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 1);
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
					Lex_setupToken(token, TYPE_LESS_THAN_OR_EQUAL, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Lex_setupToken(token, TYPE_LESS_THAN, STORAGE_NONE, lexer->loc);
					Lex_moveLexerBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '(':
		{
			Lex_setupToken(token, TYPE_LEFT_PARENTHESIS, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case ')':
		{
			Lex_setupToken(token, TYPE_RIGHT_PARENTHESIS, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case '[':
		{
			Lex_setupToken(token, TYPE_LEFT_BRACKET, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case ']':
		{
			Lex_setupToken(token, TYPE_RIGHT_BRACKET, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case ':':
		{
			Lex_setupToken(token, TYPE_COLUMN, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		case '"':
		{
			signed long long i = 1;
			for (; *(lexer->current + i) != '"'; ++i);

			Lex_setupToken(token, TYPE_LITERAL, STORAGE_STRING, lexer->loc);
			token->value.str = (char*)malloc((i + 1) * sizeof(char));
			assert(token->value.str != NULL);
			memcpy(token->value.str, lexer->current + 1, i - 1);
			token->value.str[i] = 0;

			Lex_moveLexerBy(lexer, i + 1);
		} break;

		case '\0':
		{
			Lex_setupToken(token, TYPE_EOF, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
			goto end;
		} break;

		default:
		{
			if (Lex_tryParseKeyword(lexer, token))
			{
				goto end;
			}

			if (Lex_tryParseIdentifier(lexer, token))
			{
				goto end;
			}

			if (Lex_tryParseNumericLiteral(lexer, token))
			{
				goto end;
			}

			Lex_setupToken(token, TYPE_ERROR, STORAGE_NONE, lexer->loc);
			Lex_moveLexerBy(lexer, 1);
		} break;
	}

end:
	return token;
}
