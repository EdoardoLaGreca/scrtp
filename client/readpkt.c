/*
Read packet.
Usage (with file): readpkt /path/to/file
Usage (with stdin): printf "mypayload" | readpkt
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

/* return 0 on success, 1 on failure */
int
decode(char* progname, FILE* f)
{
	int i;
	unsigned char flags;
	unsigned short idx, n, m;
	char* key;
	unsigned char* value;

	if (fread(&flags, sizeof(unsigned char), 1, f) < 1) {
		/* eof reached (probably), exit normally */
		return 1;
	}

	if (fread(&idx, sizeof(unsigned short), 1, f) < 1
		|| fread(&n, sizeof(unsigned short), 1, f) < 1
		|| fread(&m, sizeof(unsigned short), 1, f) < 1) {

		fprintf(stderr, "%s: unfinished packet\n", progname);
		return 1;
	}

	/* switch endianness */
	idx = ntohs(idx);
	n = ntohs(n);
	m = ntohs(m);

	key = calloc(n, 1);
	value = malloc(m);

	if (fread(key, 1, n, f) < n
		|| fread(value, m, 1, f) < 1) {

		fprintf(stderr, "%s: unfinished packet\n", progname);
		return 1;
	}

	printf("%04X %d %d %d %s ", flags, idx, n, m, key);

	for (i = 0; i < m; i++) {
		printf("%02X", ((unsigned char*) value)[i]);
	}

	printf("\n");

	return 0;
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

	/*fputs("flags idx n m key value\n", stderr);*/

	/* although the use of `feof` to control a loop is discouraged (see ref), in this
	   case it is acceptable because it is used together with `!ferror` and the `decode`
	   function which also detects some possible EOF by reading the return value of
	   the read functions
	   (ref: http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?id=1043284351&answer=1046476070)
	*/
	while (!feof(f) && !ferror(f)) {
		if (decode(argv[0], f)) {
			break;
		}
	}

	if (ferror(f)) {
		fprintf(stderr, "%s: ferror set\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return 0;
}
