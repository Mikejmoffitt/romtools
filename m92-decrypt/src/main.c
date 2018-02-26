#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "irem_cpu.h"

#define ADDR_MAP 0x1FFFF

const char *machine_map[] =
{
	"null",
	"gunforce",
	"bomberman",
	"lethalth",
	"dynablaster",
	"mysticri",
	"majtitl2",
	"hook",
	"rtypeleo",
	"inthunt",
	"gussun",
	"leagueman",
	"psoldier",
	"dsoccr94",
	"matchit2",
	"test"
};

static const uint8_t *key_table[] =
{
	null_decryption_table,
	gunforce_decryption_table,
	bomberman_decryption_table,
	lethalth_decryption_table,
	dynablaster_decryption_table,
	mysticri_decryption_table,
	majtitl2_decryption_table,
	hook_decryption_table,
	rtypeleo_decryption_table,
	inthunt_decryption_table,
	gussun_decryption_table,
	leagueman_decryption_table,
	psoldier_decryption_table,
	dsoccr94_decryption_table,
	matchit2_decryption_table,
	test_decryption_table
};

void create_tracemap(const char *trace_fname, int *traces)
{
	memset((void *)traces, 0, sizeof(int) * (ADDR_MAP+1));

	FILE *fin = fopen(trace_fname, "r");
	if (!fin)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", trace_fname);
		return;
	}

	while (!feof(fin))
	{
		char buf[64];
		fgets(buf, 64, fin);
		// TODO: Make a real regex so this is more robust.
		buf[5] = '\0'; // Naive line truncation to isolate address
		if (buf[0] != ' ')
		{
			int idx;
			idx = (int)strtol(buf, NULL, 16);
			traces[idx & ADDR_MAP] = 1;
		}
	}
}

void apply_table(const char *prg_fname,
                 const char *out_fname,
                 int *traceloc,
                 const uint8_t *table)
{
	unsigned int pos = 0;
	FILE *fin = fopen(prg_fname, "rb");
	if (!fin)
	{
		fprintf(stderr, "Couldn't open %s for reading.\n", prg_fname);
		return;
	}

	FILE *fout = fopen(out_fname, "wb");
	if (!fout)
	{
		fprintf(stderr, "Couldn't open %s for writing.\n", out_fname);
		fclose(fin);
		return;
	}

	while (!feof(fin))
	{
		int fetch = fgetc(fin);
		if (fetch == EOF)
		{
			break;
		}
		if (traceloc[pos])
		{
			// Handle prefixes by marking the following byte for decryption as well
			int decrypted = table[fetch];
			if (decrypted == 0xF2 || // repnz
			    decrypted == 0xF3 || // repz
			    decrypted == 0x2E || // cs:
			    decrypted == 0x3E || // ds:
			    decrypted == 0x26 || // es:
			    decrypted == 0x36) // ss:
			{
				traceloc[pos + 1] = 1;
			}
			printf("%05X !!: %02X --> %02X\n",
			       pos, fetch, decrypted);
			fputc(decrypted, fout);
		}
		else
		{
			printf("%05X --: %02X --> %02X\n",
			       pos, fetch, fetch);
			fputc(fetch, fout);
		}
		pos++;
	}

	printf("%d bytes writen to %s.\n", pos, out_fname);

	fclose(fin);
	fclose(fout);
}

void decrypt(const char *trace_fname,
             const char *prg_fname,
             const char *out_fname,
             const uint8_t *table)
{
	int traceloc[ADDR_MAP+1];
	printf("Generating trace map from %s\n", trace_fname);
	create_tracemap(trace_fname, traceloc);
	printf("Applying decryption table to %s\n", prg_fname);
	apply_table(prg_fname, out_fname, traceloc, table);
	printf("Created flat binary %s\n", out_fname);

}

static void list_machines(void)
{
	printf("Available machine types:\n");
	for (int i = 0; i < sizeof(machine_map) / sizeof(char *); i++)
	{
		printf("\t%s\n", machine_map[i]);
	}
}

int main(int argc, char **argv)
{
	int machine_choice = 0;
	if (argc < 5)
	{
		printf("Usage: %s machine trace prg_in prg_out\n\n",
		       argv[0]);
		printf("Supply a trace for the encrypted sound CPU from MAME, or "
		       "a list of known program\ncounter locations separated by"
		       " line breaks.\n");
		printf("Provide a flat 16-bit sound engine binary as well.\n");
		printf("prg_out specifies the output filename.\n\n");
		list_machines();
		return 0;
	}

	for (int i = 0; i < sizeof(machine_map) / sizeof(char *); i++)
	{
		if(strncmp(machine_map[i], argv[1], 32) == 0)
		{
			machine_choice = i;
			break;
		}
	}

	if (!machine_choice)
	{
		printf("Choice %s for machine type is invalid.\n", argv[1]);
		list_machines();
		return 0;
	}

	printf("\n\tMachine: %s\n\tTrace: %s\n\tSound CPU bin:"
	       " %s\n\tOut bin: %s\n", machine_map[machine_choice], argv[2],
	       argv[3], argv[4]);
	decrypt(argv[2], argv[3], argv[4], key_table[machine_choice]);

	return 0;
}
