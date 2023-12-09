// Absolute garbage program just so I could see where Sonic Spinball
// spent the most time
// please don't learn anything about software development from this
// it's in the weird-tools directory for a reason

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char buf[80];

	if (argc < 2)
	{
		printf("Usage: %s [tracelog] [rom_end]\n", argv[0]);
		return 0;
	}

	FILE *fin = fopen(argv[1], "r");
	if (!fin)
	{
		fprintf(stderr, "Couldn't open trace log \"%s\".\n", argv[1]);
		return 1;
	}

	unsigned int rom_end = 0x400000;  // Default to 4MiB.

	if (argc >= 3) rom_end = strtoul(argv[2], NULL, 0);

	unsigned long *loc = calloc(sizeof(unsigned long), rom_end);
	if (!loc)
	{
		fprintf(stderr, "Couldn't allocate location marker array of size %ul\n",
		        rom_end);
		return -1;
	}

	while (!feof(fin))
	{
		// hey kids: this is not how you should really parse text data
		fgets(buf, sizeof(buf), fin);
		buf[6] = '\0';
		if (buf[0] != ' ')
		{
			int idx;
			idx = (int)strtol(buf, NULL, 16);
			if (idx >= rom_end) continue;
			if (loc[idx] < 0xFFFFFFFFFFFFFFFF) loc[idx]++;
		}
	}

	fclose(fin);

	for (int i = 0; i < rom_end; i++)
	{
		if (loc[i])
		{
			printf("$%06X: %lu\n", i, loc[i]);
		}
	}

	free(loc);

	return 0;
}
