
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

#include <cassert>
#include <cstring>
#include <cstdio>
#include <vector>

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
	std::vector<const char*> sources;

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
			sources.push_back(flag);
		}
	}

	// Validating command-line arguments
	if (sources.size() <= 0)
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
	

	return 0;
}
