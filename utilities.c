/* $Id: utilities.c,v 1.3 2023/10/06 10:20:09 leavens Exp $ */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "utilities.h"

static void vbail_with_error(const char* fmt, va_list args);

// Format a string error message and print it followed by a newline on stderr
// using perror (for an OS error, if the errno is not 0)
// then exit with a failure code, so a call to this does not return.
void bail_with_error(const char *fmt, ...)
{
    fflush(stdout); // flush so output comes after what has happened already
    va_list(args);
    va_start(args, fmt);
    vbail_with_error(fmt, args);
}

// The variadic version of bail_with_error
static void vbail_with_error(const char* fmt, va_list args)
{
    extern int errno;
    char buff[2048];
    vsprintf(buff, fmt, args);
    if (errno != 0) {
	perror(buff);
    } else {
	fprintf(stderr, "%s\n", buff);
    }
    exit(EXIT_FAILURE);
}
