// Places checksum compensation byte at end of 1K boundaries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ROM_BOUND_SIZE 0x1000

int main(int argc, char **argv)
{
	int ret = -1;
	uint8_t *data = NULL;
	FILE *f = NULL;
	if (argc < 3)
	{
		printf("Usage: %s prg.bin rom_size <desired value>\n", argv[0]);
		ret = 0;
		goto done;
	}
	const char *fname = argv[1];
	const size_t rom_size = strtoul(argv[2], NULL, 0);
	uint8_t desired_value = 0;
	if (argc >= 4)
	{
		desired_value = strtoul(argv[3], NULL, 0);
	}
	// set up buffer
	f = fopen(fname, "rb");
	if (!f)
	{
		fprintf(stderr, "Couldn't open %s.\n", fname);
		goto done;
	}
	fseek(f, 0, SEEK_END);
	const size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = malloc(len);
	if (!data)
	{
		fprintf(stderr, "Couldn't malloc %d bytes\n", (int)len);
		goto done;
	}
	// read the data
	const size_t read_bytes = fread(data, 1, len, f);
	if (read_bytes != len)
	{
		fprintf(stderr, "Read %d bytes but expected %d\n",
		        (int)read_bytes, (int)len);
		goto done;
	}
	fclose(f);
	f = NULL;
	// mark checksums at 1K boundaries
	uint8_t sum = 0;
	for (size_t i = 0; i < len; i++)
	{
		const int rom_idx = (i % rom_size);
		if (rom_idx == 0) sum = 0;
		if (rom_idx == rom_size - 1)
		{
			data[i] = (0x100 + desired_value - sum) & 0xFF;
			printf("$%X := $%02X\n", (int)i, data[i]);
		}
		else
		{
			sum += data[i];
		}
	}
	// write the file back
	f = fopen(fname, "wb");
	if (!f)
	{
		fprintf(stderr, "Couldn't open %s for writing.\n", fname);
		goto done;
	}
	const size_t written_bytes = fwrite(data, 1, len, f);
	fclose(f);
	f = NULL;
	if (written_bytes != len)
	{
		fprintf(stderr, "Wrote %d bytes but expected %d\n",
		        (int)written_bytes, (int)len);
		goto done;
	}
	ret = 0;
done:
	if (data) free(data);
	if (f) fclose(f);
	return ret;
}
