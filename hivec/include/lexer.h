
/**
 * @file lexer.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-10
 */

#ifndef _LEXER_H_
#define _LEXER_H_

/**
 * @addtogroup lexer
 * 
 * @{
 */

/**
 * Location used to track lexical token's position in source file for debugging and better error
 * messages.
 */
struct Location
{
	const char* file;
	signed long long line;
	signed long long column;
};

/**
 * Stringify location structure (file:line:column).
 */
const char* Location_stringify(
	const struct Location* const location);

/**
 * All possible lexical tokens.
 */
enum Type
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
	TYPE_THEN, // then
	TYPE_ELSE, // else
	TYPE_END, // end
	TYPE_USE, // use
	TYPE_IMPORT, // import
	TYPE_PROC, // proc

	TYPE_IDENTIFIER,
	TYPE_LITERAL,
	TYPE_EOF, // \0

	TYPES_COUNT,
};

/**
 * Stringify token's type.
 */
const char* Type_stringify(
	const enum Type type);

/**
 * All possible storage types for lexical tokens (such as string literal, numeric values, and
 * identifiers).
 */
enum Storage
{
	STORAGE_NONE = 0,

	STORAGE_I64,
	STORAGE_U64,
	STORAGE_F64,
	STORAGE_STRING,

	STORAGES_COUNT,
};

/**
 * Stringify token's storage.
 */
const char* Storage_stringify(
	const enum Storage storage);

/**
 * Lexical token value structure. Should be used only by @ref Token structure. Since a
 * union might go wrong while using unions, the memory management is set in @ref Lexer_destroyToken
 * function and no additional memory management is required!
 */
union Value
{
	signed long long i64;
	unsigned long long u64;
	long double f64;
	char* str;
};

/**
 * Stringify token's value.
 */
const char* Value_stringify(
	const enum Storage storage,
	const union Value value);

/**
 * A lexical token structure.
 */
struct Token
{
	enum Type type;
	enum Storage storage;
	union Value value;
	struct Location location;
};

/**
 * Destroy token and deallocate all dynamic resources allocated to it.
 */
void Token_destroy(
	struct Token* const token);

/**
 * Stringify token with all it's fields.
 */
const char* Token_stringify(
	const struct Token* const token);

/**
 * TODO: rework lexer to read the file while lexing tokens and not reading all the contents of a
 * file into a char buffer. Let's try to preserve memory, shall we?
 */
struct Lexer
{
	char* buffer;
	signed long long length;
	char* current;
	struct Location location;
};

/**
 * Create a lexer for a specified file path.
 */
struct Lexer Lexer_create(
	const char* file);

/**
 * Destroy lexer and release all it's dynamic resources.
 */
void Lexer_destroy(
	struct Lexer* const lexer);

/**
 * Lex next lexical token with the specified lexer.
 */
struct Token* Lexer_nextToken(
	struct Lexer* const lexer);

/**
 * @}
 */

#endif
