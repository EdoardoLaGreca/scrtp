#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "net.h"
#include "args.h"

static void
args_verb() /* -v */
{
	VERB_OUTPUT = 1;
}

/* TODO: shrink this function (split it?) */
int
args_scan(int argc, char** argv)
{
	int i;
	char* host = NULL;
	char* port = NULL;

	if (argc < 3) {
		print_err("Usage: scrtp [OPTIONS]... HOST PORT");
		return 0;
	}

	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			/* it's a flag (one or more) */

			int j;
			char wrong_flag_msg[30]; /* wrong flag message */
			for (j = 1; j < strlen(argv[i]); j++) {
				switch (argv[i][j]) {
				case 'v':
					args_verb();
					break;
				default:
					/* wrong flag, print a message */
					memset(wrong_flag_msg, 0, 20);
					sprintf(wrong_flag_msg, "unknown flag: %c\n", argv[i][j]);
					print_err(wrong_flag_msg);
					return 0;
				}
			}
		} else {
			/* it's an argument (not a flag) */
			if (host == NULL) {
				host = argv[i];
			} else if (port == NULL) {
				port = argv[i];
			} else {
				/* too many arguments */
				print_err("too many arguments");
				return 0;
			}
		}
	}

	if (host == NULL || port == NULL) {
		/* not enough arguments */
		print_err("not enough arguments");
		return 0;
	}

	HOSTNAME = host;
	PORT = port;

	return 1;
}