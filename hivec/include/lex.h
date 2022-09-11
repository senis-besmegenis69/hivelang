
/**
 * @file lex.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-10
 */

#ifndef _LEX_H_
#define _LEX_H_

struct Lex_Location
{
	const char* file;
	signed long long line;
	signed long long column;
};

enum Lex_Type
{
	TYPE_ERROR = 0,

	TYPE_ADDITION, // +
	TYPE_SUBTRACTION, // -
	TYPE_MULTIPLICATION, // *
	TYPE_DIVISION, // /

	TYPE_EQUAL, // ==
	TYPE_NOT_EQUAL, // !=
	TYPE_MORE_THAN, // >
	TYPE_MORE_THAN_OR_EQUAL, // >=
	TYPE_LESS_THAN, // <
	TYPE_LESS_THAN_OR_EQUAL, // <=

	TYPE_LEFT_PARENTHESIS, // (
	TYPE_RIGHT_PARENTHESIS, // )
	TYPE_LEFT_BRACKET, // [
	TYPE_RIGHT_BRACKET, // ]
	TYPE_COLUMN, // :

	TYPE_IF, // if
	TYPE_END, // end
	TYPE_USE, // use
	TYPE_IMPORT, // import
	TYPE_PROC, // proc

	TYPE_IDENTIFIER,
	TYPE_LITERAL,
	TYPE_EOF, // \0

	OPERATIONS_COUNT,
};

enum Lex_Storage
{
	STORAGE_NONE = 0,

	STORAGE_I64,
	STORAGE_U64,
	STORAGE_F64,
	STORAGE_STRING,

	STORAGES_COUNT,
};

union Lex_Value
{
	signed long long i64;
	unsigned long long u64;
	long double f64;
	char* str;
};

struct Lex_Token
{
	enum Lex_Type type;
	enum Lex_Storage storage;
	union Lex_Value value;
	struct Lex_Location loc;
};

void Lex_destroyToken(
	struct Lex_Token* const token);

const char* Lex_stringifyToken(
	const struct Lex_Token* const token);

struct Lex_Lexer
{
	char* buffer;
	char* current;
	struct Lex_Location loc;
};

struct Lex_Lexer Lex_createLexer(
	const char* file);

void Lex_destroyLexer(
	struct Lex_Lexer* const lexer);

struct Lex_Token* Lex_nextToken(
	struct Lex_Lexer* const lexer);

#endif
