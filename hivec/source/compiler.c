
/**
 * @file compiler.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-14
 */

#include <compiler.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define writeTo(stream, string) \
	{ \
		assert(stream != NULL); \
		fprintf(stream, "%s", string); \
		fprintf(stream, "\n"); \
	}

#define writeToWith(stream, format, ...) \
	{ \
		assert(stream != NULL); \
		fprintf(stream, format, __VA_ARGS__); \
		fprintf(stream, "\n"); \
	}

#define errorOf(stream, token, format, ...) \
	{ \
		assert(stream != NULL); \
		assert(token != NULL); \
		fprintf(stream, "%s: ", Location_stringify(&token->location)); \
		fprintf(stream, format, __VA_ARGS__); \
		fprintf(stream, "\n"); \
	}

void Compiler_compileToNASM(
	const char* path,
	const char* target)
{
	FILE* stream = fopen(target, "w");
	assert(stream != NULL);

	writeTo(stream, "");
	writeTo(stream, "BITS 64");
	writeTo(stream, "");
	writeTo(stream, "segment .text");
	writeTo(stream, "global _start");
	writeTo(stream, "_start:");

	struct Lexer lexer = Lexer_create(path);
	struct Token* token = NULL;

	while ((token = Lexer_nextToken(&lexer))->type != TYPE_EOF)
	{
		if (token->type == TYPE_ERROR)
		{
			fprintf(stdout, "%s: error: encountered unknown token!\n", Location_stringify(&token->location));
			continue;
		}

		switch (token->type)
		{
			case TYPE_ADD:
			{
				writeTo(stream, "    ;; -- add -- ");
				writeTo(stream, "    pop rax");
				writeTo(stream, "    pop rbx");
				writeTo(stream, "    add rax, rbx");
				writeTo(stream, "    push rax");
			} break;

			case TYPE_SUBTRACT:
			{
				writeTo(stream, "    ;; -- subtract -- ");
				writeTo(stream, "    pop rax");
				writeTo(stream, "    pop rbx");
				writeTo(stream, "    sub rax, rbx");
				writeTo(stream, "    push rax");
			} break;

			case TYPE_MULTIPLY:
			{
				writeTo(stream, "    ;; -- multiply -- ");
				writeTo(stream, "    ;; TODO: implement the `multiply` operation!");
			} break;

			case TYPE_DIVIDE:
			{
				writeTo(stream, "    ;; -- divide -- ");
				writeTo(stream, "    ;; TODO: implement the `divide` operation!");
			} break;

			case TYPE_LITERAL:
			{
				switch (token->storage)
				{
					case STORAGE_I64:
					{
						writeTo(stream, "    ;; -- push -- ");
						writeToWith(stream, "    push %lld", token->value.i64);
					} break;
				
					default:
					{
						errorOf(stderr, token, "%s", "cannot push token of 'unknown' or 'none' type of storage!");
						continue;
					} break;
				}
			} break;

			default:
			{
				errorOf(stderr, token, "%s", "unimplemented parsing for this token!");
				continue;
			} break;
		}

		Token_destroy(token);
	}

	Lexer_destroy(&lexer);

	writeTo(stream, "    ;; -- Exiting the program --");
	writeTo(stream, "    mov rax, 60");
	writeTo(stream, "    mov rdi, 0");
	writeTo(stream, "    syscall");
	fclose(stream);
}
