/*
Read packet.
Usage (with file): readpkt /path/to/file
Usage (with stdin): printf "mypayload" | readpkt
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

typedef struct {
	unsigned char flags;
	unsigned short idx;
	unsigned short n;
	unsigned short m;
	char* key;
	void* value;
} packet;

/* format packet */
packet
topkt(FILE* f)
{
	packet p;

	fread(&p.flags, sizeof(unsigned char), 1, f);
	fread(&p.idx, sizeof(unsigned short), 1, f);
	fread(&p.n, sizeof(unsigned short), 1, f);
	fread(&p.m, sizeof(unsigned short), 1, f);

	/* switch endianness */
	p.idx = ntohs(p.idx);
	p.n = ntohs(p.n);
	p.m = ntohs(p.m);

	p.key = calloc(p.n, 1);
	fread(p.key, 1, p.n, f);
	p.value = malloc(p.m);
	fread(p.value, p.m, 1, f);

	return p;
}

/* print packet */
void
printpkt(packet p)
{
	int i;

	printf("%04X %d %d %d %s ", p.flags, p.idx, p.n, p.m, p.key);

	for (i = 0; i < p.m; i++) {
		printf("%02X", ((unsigned char*) p.value)[i]);
	}

	printf("\n");
}

int
main(int argc, char** argv)
{
	FILE* f;

	if (argc == 2) {
		/* read from file */
		f = fopen(argv[1], "r");

		if (f == NULL) {
			fprintf(stderr, "%s: unable to open file %s\n", argv[0], argv[1]);
			exit(EXIT_FAILURE);
		}
	} else if (argc > 2) {
		fprintf(stderr, "%s: too many arguments provided\n", argv[0]);
		exit(EXIT_FAILURE);
	} else {
		/* read from stdin */
		f = stdin;
	}

	fputs("flags idx n m key value\n", stderr);

	while (!feof(f)) {
		printpkt(topkt(f));
	}

	return 0;
}
