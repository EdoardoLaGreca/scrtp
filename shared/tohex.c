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
		printf("%02X", s[i]);
	}
}

/* print number as big-endian hex, where tsize is the total size in bytes of the type */
void
printnum(unsigned long num, int tsize)
{
	int i;
	unsigned char p;

	for (i = tsize - 1; i >= 0; i--) {
		p = (num & ((long) 0xFF << (i*8))) / ((long) 0x1 << (i*8));
		printf("%02X", p);
	}
}

void
printhex(char* progname, char type, char* data)
{
	unsigned long num;
	int size;

	if (data == NULL) {
		goto end;
	}

	if (type == 'S') {
		printstr(data);
		goto end;
	}

	switch (type) {
	case 's':
		size = sizeof(unsigned short);
		break;
	case 'i':
		size = sizeof(unsigned int);
		break;
	case 'l':
		size = sizeof(unsigned long);
		break;
	default:
		fprintf(stderr, "%s: unknown type %c\n", progname, type);
		return;
	}

	/* even if the format specifier is %u (unsigned integer), it can still reads signed integers. I'm going to ignore this. */
	if (sscanf(data, "%lu", &num) < 1) {
		fprintf(stderr, "%s: invalid data\n", progname);
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
