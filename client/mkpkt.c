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
#include <string.h>

/* encode packet and put it into buffer of buflen size, may crash if buffer is not big enough */
void
encode(unsigned char* buffer, char flags, unsigned short idx, unsigned short n, unsigned short m, char* key, unsigned char* value)
{
	unsigned char* ptr = buffer;

	memcpy(ptr, &flags, sizeof(flags));
	ptr += sizeof(flags);
	memcpy(ptr, &idx, sizeof(idx));
	ptr += sizeof(idx);
	memcpy(ptr, &n, sizeof(n));
	ptr += sizeof(n);
	memcpy(ptr, &m, sizeof(m));
	ptr += sizeof(m);
	memcpy(ptr, key, n);
	ptr += n;
	memcpy(ptr, value, m);
	ptr += m;
}

int
main(int argc, char** argv)
{
	FILE* f;
	int i, buflen;
	unsigned char* buf;

	char flags;
	unsigned short idx, n, m, tmp;
	char* key;
	unsigned char* value;

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

	/* get flags (tmp), idx, n, and m */
	/* use h for flags because it's the smallest integer readable value in C89's scanf */
	if (fscanf(f, "%hX %hu %hu %hu ", &tmp, &idx, &n, &m) != 4) {
		fprintf(stderr, "%s: missing packet params\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* check if there are 1 bits in the other half */
	if (tmp & 0x00FF != tmp) {
		/* it doesn't fit */
		fprintf(stderr, "%s: flags field too big\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	flags = tmp;
	buflen = sizeof(flags) + sizeof(idx) + sizeof(n) + sizeof(m) + n + m;
	key = calloc(n, 1);
	value = calloc(m, 1);

	/* read key, space and value */
	for (i = 0; i < n+1+m; i++) {
		if (feof(f)) {
			fprintf(stderr, "%s: eof reached\n", argv[0]);
			exit(EXIT_FAILURE);
		}

		if (ferror(f)) {
			fprintf(stderr, "%s: ferror set\n", argv[0]);
			exit(EXIT_FAILURE);
		}

		char c = fgetc(f);

		/* assign the character to the right array, note how the space is skipped by excluding the value i = n */
		if (i < n) {
			key[i] = c;
		} else if (i > n && i < n+m+1) {
			value[i-n-1] = c;
		}
	}

	buf = malloc(buflen);

	encode(buf, flags, idx, n, m, key, value);

	for (i = 0; i < buflen; i++) {
		fputc(buf[i], stdout);
	}

	return 0;
}
