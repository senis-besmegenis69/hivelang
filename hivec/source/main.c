
/**
 * @file main.c
 * 
 * @copyright This file is a part of the project beelang and is distributed under MIT license which
 * should have been included with the project. If not see: https://choosealicense.com/licenses/mit/
 * 
 * @author jorisb
 * 
 * @date 2022-09-03
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void _usage(
	FILE* const stream,
	const char* argv0)
{
	assert(argv0 != NULL);
	assert(stream != NULL);
	fprintf(
		stream,
		"Usage: %s [Options] sources...\n"
		"\n"
		"Options:\n"
		"    [ --help   | -h ]                        Print usage message\n"
		"    [ --output | -o ] <path>                 Set output/build path\n",
		argv0
	);
}

static const char* _shift(
	int* const argc,
	char*** const argv)
{
	const char* current = NULL;

	if (*argc > 0)
	{
		current = **argv;
		*argc -= 1;
		*argv += 1;
	}

	return current;
}

static int _main(
	const char* argv0,
	int argc,
	char** argv)
{
	#define sourcesCapacity (signed int)1024
	const char* sources[sourcesCapacity];
	signed int sourcesCount = 0;

	const char* output = NULL;

	// Parsing command-line arguments and setting up flags.
	while (argc > 0)
	{
		const char* flag = _shift(&argc, &argv);

		if (flag == NULL)
		{
			fprintf(stderr, "Failed to parse next cmd flag!\n");
			_usage(stderr, argv0);
			exit(1);
		}

		if (strcmp(flag, "--help") == 0 || strcmp(flag, "-h") == 0)
		{
			_usage(stdout, argv0);
			exit(0);
		}
		else if (strcmp(flag, "--output") == 0 || strcmp(flag, "-o") == 0)
		{
			if (argc <= 0)
			{
				fprintf(stderr, "Failed to find a value for the flag %s!\n", flag);
				_usage(stderr, argv0);
				exit(1);
			}

			output = _shift(&argc, &argv);
		}
		else
		{
			if (sourcesCount < sourcesCapacity)
			{
				sources[sourcesCount++] = flag;
			}
			else
			{
				fprintf(stderr, "Warning! Source files limit is %d files!\n", sourcesCapacity);
			}
		}
	}

	// Validating sources cache
	if (sources == NULL || sourcesCount <= 0)
	{
		fprintf(stderr, "No sources were provided!\n");
		_usage(stderr, argv0);
		exit(1);
	}

	// Validating output flag
	if (output == NULL)
	{
		// TODO: Should be set to the file name which has the main() function!
		output = "./build.out";
	}

	return 0;
}

int main(
	int argc,
	char** argv)
{
	const char* argv0 = _shift(&argc, &argv);

	if (argc <= 0)
	{
		fprintf(stderr, "Failed to find any sources! No command-line arguments were provided!\n");
		_usage(stderr, argv0);
		exit(1);
	}

	return _main(argv0, argc, argv);
}
