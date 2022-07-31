#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const int MAX_ITEMS_PER_LINE = 16;

int main(int argc, char **argv)
{
	int max_items_per_line = MAX_ITEMS_PER_LINE;
	size_t bytes_written = 0;
	int out_count_per_line = 0;
	char *out_fpath;
	char *symbol_name;
	FILE *fin, *fout;
	if (argc < 3)
	{
		fprintf(stderr, "Usage:\n%s <binary file> <symbol name> "
		                "[items per line]\n", argv[0]);
		return -1;
	}

	if (argc > 3)
	{
		max_items_per_line = atoi(argv[3]);
		if (max_items_per_line < 1)
		{
			fprintf(stderr, "Warning: Invalid value \"%s\" provided for \
			                 max items per line. Defaulting to %d.\n",
			                 argv[3], MAX_ITEMS_PER_LINE);
			max_items_per_line = MAX_ITEMS_PER_LINE;
		}
	}

	symbol_name = argv[2];

	fin = fopen(argv[1], "rb");
	if (!fin)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", argv[1]);
		return -1;
	}

	out_fpath = malloc(sizeof(char) * strnlen(argv[2], 4096));
	if (!out_fpath)
	{
		fprintf(stderr, "Couldn't allocate file output path...\n");
		fclose(fin);
		return -1;
	}

	snprintf(out_fpath, 4096, "%s.c", symbol_name);

	fout = fopen(out_fpath, "w");
	if (!fout)
	{
		fprintf(stderr, "Couldn't open %s for writing.\n", out_fpath);
		fclose(fin);
		free(out_fpath);
		return -1;
	}

	// Write const array to C file
	fprintf(fout, "#include <stdint.h>\nconst uint8_t %s[] =\n{",
	        symbol_name);

	while (!feof(fin))
	{
		if (out_count_per_line == 0)
		{
			fprintf(fout, "\n");
			out_count_per_line = max_items_per_line;
		}

		out_count_per_line--;

		int fetch = fgetc(fin);
		if (fetch == EOF)
		{
			break;
		}
		if (out_count_per_line >= max_items_per_line - 1)
		{
			fprintf(fout, "\t");
		}
		fprintf(fout, "0x%02X,", fetch);
		bytes_written++;
	}
	fprintf(fout, "\n};\n\n");
	fclose(fout);
	fclose(fin);

	printf("Wrote %d bytes to \"%s\".\n", bytes_written, out_fpath);

	// Write header file
	snprintf(out_fpath, 4096, "%s.h", symbol_name);
	fout = fopen(out_fpath, "w");
	if (!fout)
	{
		fprintf(stderr, "Error: Couldn't open %s for writing.\n", out_fpath);
		free(out_fpath);
		return -1;
	}

	fprintf(fout, "#ifndef _BIN_%s_H\n#define _BIN_%s_H\n", symbol_name, symbol_name);
	fprintf(fout, "#include <stdint.h>\n");
	fprintf(fout, "extern const uint8_t %s[];\n", symbol_name);
	fprintf(fout, "#endif // _BIN_%s_H\n", symbol_name);
	fclose(fout);

	free(out_fpath);
	return 0;
}
