// BScoop
// Tiny utility for scooping bytes out of a file into another.
// Created because using `dd` for this is clumsy, for it does not accept
// numeric arguments in hexidecimal.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 5)
	{
		printf("usage: %s in_fname offset bytes out_fname\n", argv[0]);
		return 0;
	}

	const char *in_fname_arg = argv[1];
	const char *offset_arg = argv[2];
	const char *bytes_arg = argv[3];
	const char *out_fname_arg = argv[4];
	const size_t offset = strtoul(offset_arg, NULL, 0);
	const size_t bytes = strtoul(bytes_arg, NULL, 0);
	if (bytes == 0) return 0;

	FILE *fi = fopen(in_fname_arg, "rb");
	if (!fi)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", in_fname_arg);
		return 1;
	}

	FILE *fo = fopen(out_fname_arg, "wb");
	if (!fo)
	{
		fprintf(stderr, "Couldn't open %s for writing.\n", out_fname_arg);
		fclose(fi);
		return 1;
	}

	fseek(fi, offset, SEEK_SET);
	for (size_t i = 0; i < bytes; i++)
	{
		if (feof(fi)) break;
		fputc(fgetc(fi), fo);
	}

	const size_t bytes_read = ftell(fo);

	if (bytes_read != bytes)
	{
		fprintf(stderr, "Warning: Actual bytes read does not match expectation"
		        "(actual: $%X expected $%x)\n",
		        (int)bytes_read, (int)bytes);
	}
	else
	{
		printf("Copied %d ($%X) bytes to %s.\n",
		       (int)bytes_read, (int)bytes_read, out_fname_arg);
	}

	fclose(fi);
	fclose(fo);

	return 0;
}
