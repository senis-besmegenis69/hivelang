
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

#include <lexer.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

const char* Location_stringify(
	const struct Location* const location)
{
	assert(location != NULL);
	#define bufferCapcity ((signed int)512)
	static char buffer[bufferCapcity];
	snprintf(buffer, bufferCapcity, "%s:%lld:%lld", location->file, location->line, location->column);
	#undef bufferCapcity
	return buffer;
}

const char* Type_stringify(
	const enum Type type)
{
	static const char* stringifiedTypes[] =
	{
		[TYPE_ERROR] = "error",

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
		[TYPE_ELSE] = "else", // else
		[TYPE_END] = "end", // end
		[TYPE_USE] = "use", // use
		[TYPE_IMPORT] = "import", // import
		[TYPE_PROC] = "proc", // proc

		[TYPE_IDENTIFIER] = "identifier",
		[TYPE_LITERAL] = "literal",
		[TYPE_EOF] = "eof",
	};

	static_assert((long unsigned int)(TYPES_COUNT) == (sizeof(stringifiedTypes) / sizeof(const char*)));
	assert((long unsigned int)(type) < (sizeof(stringifiedTypes) / sizeof(const char*)));
	return stringifiedTypes[type];
}

const char* Storage_stringify(
	const enum Storage storage)
{
	static const char* stringifiedStorages[] =
	{
		[STORAGE_NONE] = "none",

		[STORAGE_I64] = "i64",
		[STORAGE_U64] = "u64",
		[STORAGE_F64] = "f64",
		[STORAGE_STRING] = "string",
	};

	static_assert((long unsigned int)(STORAGES_COUNT) == (sizeof(stringifiedStorages) / sizeof(const char*)));
	assert((long unsigned int)(storage) < (sizeof(stringifiedStorages) / sizeof(const char*)));
	return stringifiedStorages[storage];
}

const char* Value_stringify(
	const enum Storage storage,
	const union Value value)
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

static struct Token* Token_create(
	void)
{
	struct Token* token = (struct Token*)malloc(sizeof(struct Token));
	assert(token != NULL);
	return token;
}

void Token_destroy(
	struct Token* const token)
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

static void Token_setup(
	struct Token* const token,
	const enum Type type,
	const enum Storage storage,
	const struct Location location)
{
	assert(token != NULL);
	token->type = type;
	token->storage = storage;
	token->location = location;
}

const char* Token_stringify(
	const struct Token* const token)
{
	#define bufferCapcity ((signed int)1536)
	static char buffer[bufferCapcity];

	snprintf(buffer, bufferCapcity, "type=`%s` storage=`%s` value=`%s` location=`%s`",
		Type_stringify(token->type),
		Storage_stringify(token->storage),
		Value_stringify(token->storage, token->value),
		Location_stringify(&token->location));

	#undef bufferCapcity
	return buffer;
}

static void Lexer_moveBy(
	struct Lexer* const lexer,
	signed long long amount)
{
	assert(lexer != NULL);

	for (signed long long i = 0; i < amount && i < lexer->length; ++i)
	{
		char current = *(lexer->current + i);

		if (current == '\n')
		{
			++lexer->location.line;
			lexer->location.column = 1;
		}
		else
		{
			++lexer->location.column;
		}
	}

	lexer->current += amount;
}

static void Lexer_skipWhitespaces(
	struct Lexer* const lexer)
{
	assert(lexer != NULL);
	while (*lexer->current == ' ' || *lexer->current == '\t' || *lexer->current == '\n' || *lexer->current == '\r')
		Lexer_moveBy(lexer, 1);
}

static void Lexer_skipSingleLineComment(
	struct Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '/');
	Lexer_moveBy(lexer, 2);

	while (*lexer->current != '\n')
		Lexer_moveBy(lexer, 1);
	
	Lexer_moveBy(lexer, 1);
}

static void Lexer_skipMultiLineComment(
	struct Lexer* const lexer)
{
	assert(lexer != NULL);
	assert(*lexer->current == '/' && *(lexer->current + 1) == '*');
	Lexer_moveBy(lexer, 2);

	while (*lexer->current != '*' && *(lexer->current + 1) != '/')
		Lexer_moveBy(lexer, 1);

	Lexer_moveBy(lexer, 2);
}

static signed int Lexer_tryParseKeyword(
	struct Lexer* const lexer,
	struct Token* const token)
{
	assert(lexer != NULL);
	assert(token != NULL);

	const char* keywords[] =
	{ "if", "end", "use", "import", "proc" };

	enum Type types[] =
	{ TYPE_IF, TYPE_END, TYPE_USE, TYPE_IMPORT, TYPE_PROC };

	for (signed int i = 0; i < 5; ++i)
	{
		if (strncmp(lexer->current, keywords[i], strlen(keywords[i])) == 0)
		{
			Token_setup(token, types[i], STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, strlen(keywords[i]));
			return 1;
		}
	}

	return 0;
}

static signed int Lexer_isIdentifierChar(
	const char c)
{
	return c == '_'
		|| c == '?'
		|| c == '@'
		|| (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| isdigit(c);
}

static signed int Lexer_tryParseIdentifier(
	struct Lexer* const lexer,
	struct Token* const token)
{
	assert(lexer != NULL);
	assert(token != NULL);

	if (!(Lexer_isIdentifierChar(*lexer->current) && !(isdigit(*lexer->current))))
	{
		return 0;
	}

	Token_setup(token, TYPE_IDENTIFIER, STORAGE_STRING, lexer->location);
	signed long long i = 1;

	for (; Lexer_isIdentifierChar(*(lexer->current + i)); ++i);

	token->value.str = (char*)malloc((i + 1) * sizeof(char));
	assert(token->value.str != NULL);
	memcpy(token->value.str, lexer->current, i);
	token->value.str[i] = 0;

	Lexer_moveBy(lexer, i);
	return 1;
}

static signed int Lexer_tryParseNumericLiteral(
	struct Lexer* const lexer,
	struct Token* const token)
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
		Token_setup(token, TYPE_LITERAL, STORAGE_I64, lexer->location);
		signed long long value;
		sscanf(buffer, "%lld", &value);
		token->value.i64 = value;
	}
	else if (dots == 1)
	{
		Token_setup(token, TYPE_LITERAL, STORAGE_F64, lexer->location);
		long double value;
		sscanf(buffer, "%Lf", &value);
		token->value.f64 = value;
	}
	else
	{
		Token_setup(token, TYPE_ERROR, STORAGE_NONE, lexer->location);
	}

	Lexer_moveBy(lexer, i);
	return 1;
}

struct Lexer Lexer_create(
	const char* file)
{
	struct Lexer lexer = {0};
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

	lexer.length = fsize;
	lexer.current = lexer.buffer;
	lexer.location = (struct Location) { .file = file, .line = 1, .column = 1 };
	return lexer;
}

void Lexer_destroy(
	struct Lexer* const lexer)
{
	assert(lexer != NULL);
	free(lexer->buffer);
}

struct Token* Lexer_nextToken(
	struct Lexer* const lexer)
{
	assert(lexer != NULL);
	struct Token* token = Token_create();

begining:
	Lexer_skipWhitespaces(lexer);

	switch (*lexer->current)
	{
		case '+':
		{
			Token_setup(token, TYPE_ADDITION, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case '-':
		{
			Token_setup(token, TYPE_SUBTRACTION, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case '*':
		{
			Token_setup(token, TYPE_MULTIPLICATION, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case '/':
		{
			switch (*(lexer->current + 1))
			{
				case '/':
				{
					Lexer_skipSingleLineComment(lexer);
					goto begining;
				} break;

				case '*':
				{
					Lexer_skipMultiLineComment(lexer);
					goto begining;
				} break;

				default:
				{
					Token_setup(token, TYPE_DIVISION, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 1);
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
					Token_setup(token, TYPE_EQUAL, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Token_setup(token, TYPE_ERROR, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 1);
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
					Token_setup(token, TYPE_NOT_EQUAL, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Token_setup(token, TYPE_ERROR, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 1);
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
					Token_setup(token, TYPE_MORE_THAN_OR_EQUAL, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Token_setup(token, TYPE_MORE_THAN, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 1);
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
					Token_setup(token, TYPE_LESS_THAN_OR_EQUAL, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 2);
					goto end;
				} break;

				default:
				{
					Token_setup(token, TYPE_LESS_THAN, STORAGE_NONE, lexer->location);
					Lexer_moveBy(lexer, 1);
					goto end;
				} break;
			}
		} break;

		case '(':
		{
			Token_setup(token, TYPE_LEFT_PARENTHESIS, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case ')':
		{
			Token_setup(token, TYPE_RIGHT_PARENTHESIS, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case '[':
		{
			Token_setup(token, TYPE_LEFT_BRACKET, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case ']':
		{
			Token_setup(token, TYPE_RIGHT_BRACKET, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case ':':
		{
			Token_setup(token, TYPE_COLUMN, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		case '"':
		{
			signed long long i = 1;
			for (; *(lexer->current + i) != '"' && lexer->current + i < lexer->buffer + lexer->length; ++i);

			Token_setup(token, TYPE_LITERAL, STORAGE_STRING, lexer->location);
			token->value.str = (char*)malloc((i + 1) * sizeof(char));
			assert(token->value.str != NULL);
			memcpy(token->value.str, lexer->current + 1, i - 1);
			token->value.str[i] = 0;

			Lexer_moveBy(lexer, i + 1);
		} break;

		case '\0':
		{
			Token_setup(token, TYPE_EOF, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
			goto end;
		} break;

		default:
		{
			if (Lexer_tryParseKeyword(lexer, token))
			{
				goto end;
			}

			if (Lexer_tryParseIdentifier(lexer, token))
			{
				goto end;
			}

			if (Lexer_tryParseNumericLiteral(lexer, token))
			{
				goto end;
			}

			Token_setup(token, TYPE_ERROR, STORAGE_NONE, lexer->location);
			Lexer_moveBy(lexer, 1);
		} break;
	}

end:
	return token;
}
