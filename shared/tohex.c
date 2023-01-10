/*
Convert to hex.

Usage: tohex type data

where type can be:
 - 'S' for string
 - 's' for short
 - 'i' for int
 - 'l' for long
*/

#include <stdio.h>
#include <stdlib.h>

/* print string as hex */
void
printstr(char* s)
{
	int i;
	for (i = 0; s[i] != '\0'; i++) {
		printf("%X", s[i]);
	}
}

/* print number as hex, where size is the total width in bytes of the number */
void
printnum(unsigned long num, int size)
{
	/* TODO */
}

void
printhex(char* progname, char type, char* data)
{
	unsigned long num;
	int size;

	if (data == NULL) {
		goto end;
	}

	switch (type) {
	case 'S':
		printstr(data);
		goto end;
		break;
	case 's':
		sscanf(data, "%hu", (unsigned short*) (&num));
		size = sizeof(unsigned short);
		break;
	case 'i':
		sscanf(data, "%u", (unsigned int*) (&num));
		size = sizeof(unsigned int);
		break;
	case 'l':
		sscanf(data, "%lu", (unsigned long*) (&num));
		size = sizeof(unsigned long);
		break;
	default:
		fprintf(stderr, "%s: unknown type %c", progname, type);
		return;
	}

	printnum(num, size);

end:
	printf("\n");
	return;
}

int
main(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stderr, "%s: not enough arguments", argv[0]);
		exit(EXIT_FAILURE);
	}

	printhex(argv[0], argv[1][0], argv[2]);

	return 0;
}
