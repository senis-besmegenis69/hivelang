
/**
 * @file main.c
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-05
 */

#include <lex.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void usage(
	FILE* const stream,
	const char* argv0)
{
	fprintf(stream,
		"Usage: %s [Options] sources...\n"
		"Options:\n"
		"    [ --output | -o ] <path>         Set output path for the target\n"
		"    [ --help   | -h ]                Print usage message\n",
		argv0);
}

static const char* shift(
	int* const argc,
	char*** const argv)
{
	const char* current = NULL;

	if (*argc > 0 && *argv != NULL)
	{
		current = **argv;
		*argc -= 1;
		*argv += 1;
	}

	return current;
}

int main(int argc, char** argv)
{
	// Fetching program name from argc[0]
	const char* arg0 = shift(&argc, &argv);

	// Checking if user provided arguments to parse
	if (argc <= 0)
	{
		fprintf(stderr, "Error: no command-line arguments were provided!\n");
		usage(stderr, arg0);
		return 1;
	}

	// Setting up arguments and configs
	const char* output = NULL;

	#define sourcesCapacity ((signed int)2048)
	const char* sources[sourcesCapacity];
	signed int sourcesCount = 0;

	// Parsing command-line arguments
	while (argc > 0)
	{
		const char* flag = shift(&argc, &argv);

		if (flag == NULL)
		{
			fprintf(stderr, "Error: encountered undefined flag!\n");
			usage(stderr, arg0);
			return 1;
		}

		if (strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0)
		{
			usage(stdout, arg0);
			return 0;
		}
		else if (strcmp(flag, "--output") == 0 || strcmp(flag, "-o") == 0)
		{
			if (argc <= 0)
			{
				fprintf(stderr, "Error: failed to find value for flag: %s!\n", flag);
				usage(stderr, arg0);
				return 1;
			}

			flag = shift(&argc, &argv);

			if (flag == NULL)
			{
				fprintf(stderr, "Error: encountered undefined flag!\n");
				usage(stderr, arg0);
				return 1;
			}

			output = flag;
		}
		else
		{
			if (sourcesCount < sourcesCapacity)
			{
				sources[sourcesCount++] = flag;
			}
			else
			{
				fprintf(stderr, "Error: compiler supports up to %d source files!\n", sourcesCapacity);
				usage(stderr, arg0);
				return 1;
			}
		}
	}

	// Validating command-line arguments
	if (sources == NULL || sourcesCount <= 0)
	{
		fprintf(stderr, "Error: no sources were provided!\n");
		usage(stderr, arg0);
		return 1;
	}

	if (output == NULL || (output != NULL && strlen(output) <= 0))
	{
		output = "a.txt";
	}

	// Parsing the source and compiling the target
	for (signed int index = 0; index < sourcesCount; ++index)
	{
		fprintf(stdout, "%s\n", sources[index]);
		
		struct Lex_Lexer lexer = Lex_createLexer(sources[index]);
		struct Lex_Token* token = NULL;

		while ((token = Lex_nextToken(&lexer))->type != TYPE_EOF)
		{
			if (token->type == TYPE_ERROR)
			{
				fprintf(stdout, "Warning: encountered an error token at file: %s line: %lld column: %lld!\n", token->loc.file, token->loc.line, token->loc.column);
			}
		}

		Lex_destroyLexer(&lexer);
	}

	return 0;
}
