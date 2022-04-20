#include <stdio.h>

#include "print.h"

void
print_err(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

void
print_verb(const char *msg)
{
	if (VERB_OUTPUT) {
		printf("%s\n", msg);
	}
}