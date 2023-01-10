/*
Convert string to hex.

Usage: str2h "my string"
*/

#include <stdio.h>
#include <stdlib.h>

void
printhex(char* s)
{
	int i;

	if (s == NULL) {
		goto end;
	}

	for (i = 0; s[i] != '\0'; i++) {
		printf("%X", s[i]);
	}

end:
	printf("\n");
	return;
}

int
main(int argc, char** argv)
{
	printhex(argv[1]);

	return 0;
}
