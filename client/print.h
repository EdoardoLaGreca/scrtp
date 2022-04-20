#pragma once

/*
 * Verbose output, 1 if verbose, 0 otherwise.
 */
extern int VERB_OUTPUT;

/*
 * Print an error message.
 */
void print_err(const char *msg);

/*
 * Print a verbose message.
 */
void print_verb(const char *msg);