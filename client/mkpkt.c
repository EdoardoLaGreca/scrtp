/*
Make packet.

Usage (with file): mkpkt /path/to/file
Usage (with stdin): printf "my data" | mkpkt

Values for flags, idx, n, m, key, value are given respectively, separated by space.
The flags field must be written in caps hex value of 2 digits.
The value field must also be written in caps hex;
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netinet/in.h>

/* read values from stdout and place them in the fields */
void
readvals(char* progname, FILE* f, char* flags, unsigned short* idx, unsigned short* n, unsigned short* m, char** key, unsigned char** value)
{
	int i;
	unsigned short tmp;

	/* get flags (tmp), idx, n, and m */
	/* use h for flags because it's the smallest integer readable value in C89's scanf */
	if (fscanf(f, "%hX %hu %hu %hu ", &tmp, idx, n, m) != 4) {
		fprintf(stderr, "%s: missing packet params\n", progname);
		exit(EXIT_FAILURE);
	}

	/* check if there are 1 bits in the other half */
	if ((tmp & 0x00FF) != tmp) {
		/* it doesn't fit */
		fprintf(stderr, "%s: flags field too big\n", progname);
		exit(EXIT_FAILURE);
	}

	*flags = tmp;
	*key = calloc(*n, 1);
	*value = calloc(*m, 1);

	/* read key, space and value */
	for (i = 0; i < *n+1+*m; i++) {
		char c;

		if (feof(f)) {
			fprintf(stderr, "%s: eof reached\n", progname);
			exit(EXIT_FAILURE);
		}

		if (ferror(f)) {
			fprintf(stderr, "%s: ferror set\n", progname);
			exit(EXIT_FAILURE);
		}

		c = fgetc(f);

		/* assign the character to the right array, note how the space is skipped by excluding the value i = n */
		if (i < *n) {
			*key[i] = c;
		} else if (i > *n && i < *n+*m+1) {
			*value[i-*n-1] = c;
		}
	}
}

/* encode packet and print it to stdout, do not check for feof and ferror */
void
encode(char flags, unsigned short idx, unsigned short n, unsigned short m, char* key, unsigned char* value)
{
	unsigned short netidx, netn, netm;

	/* convert to network endianness */
	netidx = htons(idx);
	netn = htons(n);
	netm = htons(m);

	/* print everything to stdout */
	fwrite(&flags, sizeof(flags), 1, stdout);
	fwrite(&netidx, sizeof(idx), 1, stdout);
	fwrite(&netn, sizeof(n), 1, stdout);
	fwrite(&netm, sizeof(m), 1, stdout);
	fwrite(&key, 1, n, stdout);
	fwrite(&value, 1, m, stdout);
}

int
main(int argc, char** argv)
{
	FILE* f;

	char flags;
	unsigned short idx, n, m;
	char* key;
	unsigned char* value;

	/* read argument */
	if (argc == 2) {
		/* read from file */
		f = fopen(argv[1], "r");

		if (f == NULL) {
			fprintf(stderr, "%s: unable to open file %s\n", argv[0], argv[1]);
			exit(EXIT_FAILURE);
		}
	} else if (argc > 2) {
		/* too many non-option arguments */
		fprintf(stderr, "%s: too many arguments (%d > 2)", argv[0], argc);
		exit(EXIT_FAILURE);
	} else {
		/* no file, read from stdin */
		f = stdin;
	}

	while (!feof(f)) {
		/* read input and produce output */
		readvals(argv[0], f, &flags, &idx, &n, &m, &key, &value);
		encode(flags, idx, n, m, key, value);
		fflush(stdout);
	}

	return 0;
}
