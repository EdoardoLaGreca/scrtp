/*
Make packet.

Usage (with file): mkpkt [option] /path/to/file
Usage (with stdin): printf "my data" | mkpkt [option]
where option can only be "-c" for "continuous mode", in which the program continues its functionality after the first line of input.

Values for flags, idx, n, m, key, value are given respectively, separated by space.
The flags field must be written in caps hex value of 2 digits.
The value field must also be written in caps hex;
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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
	int i;

	fputc(flags, stdout);
	fputc(idx, stdout);
	fputc(n, stdout);
	fputc(m, stdout);

	for (i = 0; i < n; i++) {
		fputc(key[i], stdout);
	}

	for (i = 0; i < m; i++) {
		fputc(value[i], stdout);
	}
}

int
main(int argc, char** argv)
{
	FILE* f;
	int o = 0, cont = 0;

	char flags;
	unsigned short idx, n, m;
	char* key;
	unsigned char* value;

	while (o != -1) {
		o = getopt(argc, argv, "c");

		switch (o) {
		case 'c':
			cont = 1;
			break;
		case '?':
			fprintf(stderr, "%s: invalid option -%c", argv[0], o);
			break;
		}
	}

	/* check args */
	if (argc > 2) {
		fprintf(stderr, "%s: too many arguments provided\n", argv[0]);
		exit(EXIT_FAILURE);
	} else if (argc == 2) {

		/* read from file */
		f = fopen(argv[1], "r");

		if (f == NULL) {
			fprintf(stderr, "%s: unable to open file %s\n", argv[0], argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
		/* read from stdin */
		f = stdin;
	}

	/* read input and produce output */
	do {
		readvals(argv[0], f, &flags, &idx, &n, &m, &key, &value);
		encode(flags, idx, n, m, key, value);
	} while (cont != 1 && !feof(f));

	return 0;
}
