#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *fi, *fo;
	size_t fsize;
	char *buf;

	if (argc < 3)
	{
		printf("Usage: %s in_fname out_fname\n", argv[0]);
		return 0;
	}

	fi = fopen(argv[1], "rb");
	if (!fi)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n",
		        argv[1]);
		return 1;
	}

	fseek(fi, 0L, SEEK_END);
	fsize = ftell(fi);
	fseek(fi, 0L, SEEK_SET);

	buf = (char *)malloc(sizeof(char) * fsize);
	if (!buf)
	{
		fprintf(stderr, "Couldn't allocate %zu for buffer.\n", fsize);
		fclose(fi);
		return 1;
	}

	fo = fopen(argv[2], "wb");
	if (!fo)
	{
		fclose(fi);
		free(buf);
		fprintf(stderr, "Couldn't open %s for writing.\n", argv[2]);
		return 1;
	}

	for (size_t i = 0; i < fsize; i++)
	{
		buf[fsize - i - 1] = fgetc(fi);
	}

	fwrite((void *)buf, fsize, 1, fo);

	printf("Wrote %zu ($%X) bytes to %s.\n", fsize, (int)fsize, argv[2]);

	fclose(fi);
	fclose(fo);
	free(buf);
}
