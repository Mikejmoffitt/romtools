#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char buf[64];
	int traces[0x100000];

	if (argc < 2)
	{
		printf("Usage: %s [tracelog]\n", argv[0]);
		return 0;
	}

	memset((void *)traces, 0, 0x100000);

	FILE *fin = fopen(argv[1], "r");
	if (!fin)
	{
		return 1;
	}

	while (!feof(fin))
	{
		fgets(buf, 64, fin);
		buf[5] = '\0';
		if (buf[0] != ' ')
		{
			int idx;
			idx = (int)strtol(buf, NULL, 16);
			traces[idx] = 1;
		}
	}

	fclose(fin);

	for (int i = 0; i < 0x100000; i++)
	{
		if (traces[i])
		{
			printf("%08X\n", i);
		}
	}

	return 0;
}
