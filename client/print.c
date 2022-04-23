#include <stdio.h>

#include "print.h"

int VERB_OUTPUT = 0;

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

int
print_windows(char* windows)
{
	int window_idx = 0, char_idx = 0;

	printf("0 - entire desktop\n");
	window_idx = 1;

	for ( ; windows[char_idx] != 0; window_idx++) {
		printf("%d - ", window_idx);
		for ( ; windows[char_idx] != '\n' && windows[char_idx] != 0; char_idx++) {
			printf("%c", windows[char_idx]);
		}
		printf("\n");

		if (windows[char_idx] == '\n') {
			char_idx++;
		}
	}

	window_idx--;

	return window_idx;
}
