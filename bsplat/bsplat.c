// BSplat
// The opposite of BScoop - overlays binary data on top of a file.
// Created because using `dd` for this is clumsy, as it does not accept
// numeric arguments in hexidecimal.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 5)
	{
		printf("usage: %s base_fname overlay_fname offset out_fname\n", argv[0]);
		return 0;
	}

	const char *base_fname_arg = argv[1];
	const char *overlay_fname_arg = argv[2];
	const char *offset_arg = argv[3];
	const char *out_fname_arg = argv[4];
	const size_t offset = strtoul(offset_arg, NULL, 0);

	FILE *fi = fopen(base_fname_arg, "rb");
	if (!fi)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", base_fname_arg);
		return 1;
	}

	FILE *fs = fopen(overlay_fname_arg, "rb");
	if (!fs)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", overlay_fname_arg);
		fclose(fi);
		return 1;
	}

	FILE *fo = fopen(out_fname_arg, "wb");
	if (!fo)
	{
		fprintf(stderr, "Couldn't open %s for writing.\n", out_fname_arg);
		fclose(fi);
		fclose(fs);
		return 1;
	}

	fseek(fi, 0, SEEK_END);
	const size_t fi_size = ftell(fi);
	fseek(fi, 0, SEEK_SET);
	fseek(fs, 0, SEEK_END);
	const size_t fs_size = ftell(fs);
	fseek(fs, 0, SEEK_SET);

	printf("Base file: %ld ($%lX) bytes.\n", fi_size, fi_size);
	printf("Overlay: %ld ($%lX) bytes @ $%lX.\n", fs_size, fs_size, offset);

	if (offset >= fi_size)
	{
		printf("Warning: Overlay data begins outside original size.\n");
		fclose(fi);
		fclose(fs);
		fclose(fo);
		return 1;
	}
	if (offset + fs_size >= fi_size)
	{
		printf("Warning: Overlay data overruns original size.\n");
		fclose(fi);
		fclose(fs);
		fclose(fo);
		return 1;
	}

	for (size_t i = 0; i < offset; i++)
	{
		fputc(fgetc(fi), fo);
	}
	for (size_t i = 0; i < fs_size; i++)
	{
		fputc(fgetc(fs), fo);
	}
	fseek(fi, offset + fs_size, SEEK_SET);
	for (size_t i = 0; i < fi_size - (offset + fs_size); i++)
	{
		fputc(fgetc(fi), fo);
	}

	fclose(fi);
	fclose(fs);
	fclose(fo);

	return 0;
}
