// Dummy stupid tool that sticks a smpl chunk on the end of a WAV file to
// indicate loop points.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

void fwrite32le(uint32_t v, FILE *f)
{
	fputc(v & 0xFF, f);
	v = v >> 8;
	fputc(v & 0xFF, f);
	v = v >> 8;
	fputc(v & 0xFF, f);
	v = v >> 8;
	fputc(v & 0xFF, f);
}

// Writes a sample chunk specifying start and end sample.
void write_smpl_chunk(FILE *f, uint32_t start, uint32_t end)
{
	// $00 - $0F
	// The header.
	fputc('s', f);
	fputc('m', f);
	fputc('p', f);
	fputc('l', f);
	// Chunk size minus four
	fwrite32le(0x40 - 4, f);
	// Manufacturer (none)
	fwrite32le(0, f);
	// Product (none in particular)
	fwrite32le(0, f);

	// $10 - $1F
	// Sample period (unspecified)  TODO: Take from incoming wave file?
	fwrite32le(0, f);
	// MIDI unity node (irrelevant)
	fwrite32le(0x40 - 4, f);  // TODO: Isn't this unnecessary?
	// MIDI pitch fraction (n/a)
	fwrite32le(0, f);
	// SMPTE format
	fwrite32le(0, f);

	// $20 - $2B
	// SMPTE offset
	fwrite32le(0, f);
	// Number of sample loops
	fwrite32le(1, f);
	fwrite32le(0, f);

	// The sample loop begins.
	// ID
	fwrite32le(0, f);
	// Type (forward = 0)
	fwrite32le(0, f);
	// Start position
	fwrite32le(start, f);
	// End position
	fwrite32le(end, f);
	// Fraction
	fwrite32le(0, f);
	// Loops (infinity)
	fwrite32le(0, f);
}

void usage(const char *name)
{
	printf("Usage: %s in.wav out.wav start_sample end_sample\n", name);
}

int main(int argc, char **argv)
{
	if (argc < 5)
	{
		usage(argv[0]);
		return 0;
	}

	const char *in_fname = argv[1];
	const char *out_fname = argv[2];
	const uint32_t start = strtoul(argv[3], NULL, 0);
	const uint32_t end = strtoul(argv[4], NULL, 0);

	FILE *fin = fopen(in_fname, "rb");
	if (!fin)
	{
		fprintf(stderr, "Could not open %s for reading.\n", in_fname);
		return -1;
	}

	FILE *fout = fopen(out_fname, "wb");
	if (!fout)
	{
		fprintf(stderr, "Could not open %s for writing.\n", in_fname);
		fclose(fin);
		return -1;
	}

	int fetch = 0;
	while ((fetch = fgetc(fin)) != EOF) fputc(fetch, fout);
	fclose(fin);

	write_smpl_chunk(fout, start, end);
	fclose(fout);
	return 0;
}
