/*	$NetBSD: inttoa.c,v 1.1.1.1 2000/03/29 12:38:50 simonb Exp $	*/

/*
 * inttoa - return an asciized signed integer
 */
#include <stdio.h>

#include "lib_strbuf.h"
#include "ntp_stdlib.h"

char *
inttoa(
	long ival
	)
{
	register char *buf;

	LIB_GETBUF(buf);

	(void) sprintf(buf, "%ld", (long)ival);
	return buf;
}
