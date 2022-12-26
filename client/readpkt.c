/* Read packet. */

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned char flags;
	unsigned short idx;
	unsigned short n;
	char* key;
	unsigned short m;
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
	p.key = calloc(p.n, 1);
	fread(p.key, 1, p.n, f);
	fread(&p.m, sizeof(unsigned short), 1, f);
	p.value = malloc(p.m);
	fread(p.value, p.m, 1, f);
	
	return p;
}

/* print packet */
void
printpkt(packet p)
{
	int i;

	printf("flags %04X", p.flags);
	printf(" idx %d", p.idx);
	printf(" n %d", p.n);
	printf(" key %s", p.key);
	printf(" m %d", p.m);

	printf(" value ");
	for (i = 0; i < p.m; i++) {
		printf("%X", ((unsigned char*) p.value)[i]);
	}

	printf("\n");
}

int
main(int argc, char** argv)
{
	FILE* f;

	if (argc > 1) {
		if (argc > 2) {
			fprintf(stderr, "%s: too many arguments provided", argv[0]);
			exit(EXIT_FAILURE);
		}

		/* read from file */
		f = fopen(argv[1], "r");

		if (f == NULL) {
			fprintf(stderr, "%s: unable to open file %s", argv[0], argv[1]);
		}
	} else {
		/* read from stdin */
		f = stdin;
	}

	printpkt(topkt(f));

	return 0;
}
