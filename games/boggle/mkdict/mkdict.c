/*	$NetBSD: mkdict.c,v 1.5 1999/09/18 20:50:28 jsm Exp $	*/

/*-
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Barry Brachman.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1993\n\
	The Regents of the University of California.  All rights reserved.\n");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)mkdict.c	8.1 (Berkeley) 6/11/93";
#else
__RCSID("$NetBSD: mkdict.c,v 1.5 1999/09/18 20:50:28 jsm Exp $");
#endif
#endif /* not lint */

/*
 * Filter out words that:
 *	1) Are not completely made up of lower case letters
 *	2) Contain a 'q' not immediately followed by a 'u'
 *	3) Are less that 3 characters long
 *	4) Are greater than MAXWORDLEN characters long
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "bog.h"

int main __P((int, char *[]));

int
main(argc, argv)
	int argc;
	char *argv[];
{
	char *p, *q;
	int ch, common, n, nwords;
	int current, len, prev, qcount;
	char buf[2][MAXWORDLEN + 1];

	prev = 0;
	current = 1;
	buf[prev][0] = '\0';
	if (argc == 2)
		n = atoi(argv[1]);

	for (nwords = 1;
	    fgets(buf[current], MAXWORDLEN + 1, stdin) != NULL; ++nwords) {
		if ((p = strchr(buf[current], '\n')) == NULL) {
			warnx("word too long: %s", buf[current]);
			while ((ch = getc(stdin)) != EOF && ch != '\n')
				;
			if (ch == EOF)
				break;
			continue;
		}
		len = 0;
		for (p = buf[current]; *p != '\n'; p++) {
			if (!islower(*p))
				break;
			if (*p == 'q') {
				q = p + 1;
				if (*q != 'u')
					break;
				else {
					while ((*q = *(q + 1)))
						q++;
				}
				len++;
			}
			len++;
		}
		if (*p != '\n' || len < 3 || len > MAXWORDLEN)
			continue;
		if (argc == 2 && nwords % n)
			continue;

		*p = '\0';
		p = buf[current];
		q = buf[prev];
		qcount = 0;
		while ((ch = *p++) == *q++ && ch != '\0')
			if (ch == 'q')
				qcount++;
		common = p - buf[current] - 1;
		printf("%c%s", common + qcount, p - 1);
		prev = !prev;
		current = !current;
	}
	warnx("%d words", nwords);
	fflush(stdout);
	if (ferror(stdout))
		err(1, "writing standard output");
	exit(0);
}
