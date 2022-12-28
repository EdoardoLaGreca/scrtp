/*
Make packet.
Usage (with file): mkpkt /path/to/file
Usage (with stdin): printf "my data" | mkpkt
Values for flags, idx, n, m, key, value are given respectively, separated by space.
The flags field must be written in caps hex value of 2 digits.
The value field must also be written in caps hex;
*/

#include <stdio.h>

/* encode packet and put it into buffer of buflen size */
int
encode(unsigned char* buffer, int buflen, char flags, unsigned short idx, unsigned short n, unsigned short m, char* key, unsigned char* value)
{
	/* TODO */
}

int
main(int argc, char** argv)
{
	FILE* f;
	int i;
	char* buf;

	char flags;
	unsigned short idx, n, m, tmp;
	char* key;
	unsigned char* value;

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

	/* get flags (tmp), idx, n, and m */
	/* use h for flags because it's the smallest readable value in C89's scanf */
	if (sscanf(f, "%Xh %uh %uh %uh ", tmp, idx, n, m) != 4) {
		fprintf(stderr, "%s: missing packet params\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* check if there are 1 bits in the other half */
	if (tmp & 0x00FF != tmp) {
		/* it doesn't fit */
		fprintf(stderr, "%s: flags field too big\n", argv[0]);
	}

	flags = tmp;

	/* read key */
	for (i = 0; i < n; i++) {
		/* TODO */
	}

	/* read space */
	fgetc(f);

	/* read value (hex) */
	for (i = 0; i < m; i++) {
		/* TODO */
	}


	return 0;
}
