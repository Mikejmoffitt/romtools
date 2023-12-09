// I wrote this when messing around with CPS1 a while ago. This predates
// the other rom tools in this repo and is likely obsolete; a short script
// and use of bsplit can probably do everything this crummy tool does.
//
// This is in the weird-tools directory for a reason, so please do not regard
// it as anything but a janky little tool that you do not need.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
	size_t bytes_written;
	FILE *f[4];
	FILE *fout;
	int c;
	if (argc < 6)
	{
		printf("usage: %s rom0 rom2 rom4 rom6 out\n", argv[0]);
		return 0;
	}

	bytes_written = 0;

	memset(f, 0, sizeof(FILE *) * 4);

	f[0] = fopen(argv[1], "rb");
	f[1] = fopen(argv[2], "rb");
	f[2] = fopen(argv[3], "rb");
	f[3] = fopen(argv[4], "rb");
	if (!f[0] || !f[1] || !f[2] || !f[3])
	{
		fprintf(stderr, "Error opening file for reading.\n");
		goto end;
	}
	fout = fopen(argv[5], "wb");
	if (!fout)
	{
		fprintf(stderr, "Error opening file for writing.\n");
		goto end;
	}

	c = 0;

	do
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				c = fgetc(f[i]);
				if (c == EOF)
				{
					goto end;
				}
				fputc(c, fout);
				bytes_written++;
			}
		}
	} while (c != EOF);

end:
	for (int i = 0; i < 4; i++)
	{
		if (f[i])
		{
			fclose(f[i]);
			f[i] = NULL;
		}
	}

	if (fout)
	{
		fclose(fout);
		fout = NULL;
	}
	printf("%zu bytes written to %s\n", bytes_written, argv[5]);
	return 0;
}
