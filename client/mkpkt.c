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
#include <netinet/in.h>

/* read and parse values from f and place them in the arguments following f */
void
readvals(char* progname, FILE* f, char* flags, unsigned short* idx, unsigned short* n, unsigned short* m, char** key, unsigned char** value)
{
	int i, j;
	fpos_t fpos;
	char c;
	unsigned short tmp;

	if (fgetpos(f, &fpos) != 0) {
		fprintf(stderr, "%s: unable to get stream position\n", progname);
		exit(EXIT_FAILURE);
	}

	/* check for EOF */
	if (fgetc(f) == EOF) {
		/* EOF reached */
		exit(EXIT_SUCCESS);
	}

	/* reset position to before checking the EOF */
	if (fsetpos(f, &fpos) != 0) {
		fprintf(stderr, "%s: unable to set stream position\n", progname);
		exit(EXIT_FAILURE);
	}

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

	/* read key */
	for (i = 0; i < *n; i++) {
		c = fgetc(f);
		(*key)[i] = c;
	}

	/* read space */
	fgetc(f);

	/* read value */
	for (i = 0; i < *m; i += 2) {
		/* pointer to value */
		unsigned char* vptr = &((*value)[i/2]);

		*vptr = 0;

		for (j = 0; j < 2; j++) {
			c = fgetc(f);

			/* convert hex char to number, I used some bit tricks to make it efficient */
			if (c >= 0x30 && c <= 0x39) {
				/* number 0-9 */
				*vptr += (c - 0x30) << ((1-j)*4);
			} else if (c >= 0x41 && c <= 0x46) {
				/* uppercase letter A-F */
				*vptr += (c - 0x41 + 10) << ((1-j)*4);
			} else if (c >= 0x61 && c <= 0x66) {
				/* lowercase letter a-f */
				*vptr += (c - 0x61 + 10) << ((1-j)*4);
			} else {
				fprintf(stderr, "%s: invalid hex character in value (\'%c\')\n", progname, c);
			}
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
		fprintf(stderr, "%s: too many arguments\n", argv[0]);
		exit(EXIT_FAILURE);
	} else {
		/* no file, read from stdin */
		f = stdin;
	}

	while (!feof(f) && !ferror(f)) {
		/* read input and produce output */
		readvals(argv[0], f, &flags, &idx, &n, &m, &key, &value);
		encode(flags, idx, n, m, key, value);
		fflush(stdout);
	}

	if (ferror(f)) {
		fprintf(stderr, "%s: ferror set\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}
