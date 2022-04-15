#include <stdio.h>

#include "print.h"

void
perr(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

void
pverb(const char *msg)
{
	printf(msg);
}