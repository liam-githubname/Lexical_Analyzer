/* $Id: lexer.h,v 1.5 2023/10/06 08:56:20 leavens Exp $ */
#ifndef _LEXER_H
#define _LEXER_H
#include <stdbool.h>

// Have any error messages been printed?
extern bool errors_noted;

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
extern void lexer_init(char *fname);

// Return the name of the current file
extern const char *lexer_filename();

// Return the line number of the next token
extern unsigned int lexer_line();

// On standard output:
// Print a message about the file name of the lexer's input
// and then print a heading for the lexer's output.
extern void lexer_print_output_header();

// Print information about the token t to stdout
// followed by a newline
extern void lexer_print_token(int t, unsigned int tline,
			      const char *txt);

/* Read all the tokens from the input file
 * and print each token on standard output
 * using the format in lexer_print_token */
extern void lexer_output();

#endif
