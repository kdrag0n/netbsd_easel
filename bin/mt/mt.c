/*	$NetBSD: mt.c,v 1.18 1997/01/09 16:39:53 tls Exp $	*/

/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
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

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1980, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)mt.c	8.2 (Berkeley) 6/6/93";
#else
static char rcsid[] = "$NetBSD: mt.c,v 1.18 1997/01/09 16:39:53 tls Exp $";
#endif
#endif /* not lint */

/*
 * mt --
 *   magnetic tape manipulation program
 */
#include <rmt.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mtio.h>
#include <sys/stat.h>

#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* pseudo ioctl constants */
#define MTASF	100

struct commands {
	const char *c_name;
	int c_code;
	int c_ronly;
};

const struct commands com[] = {
	{ "asf",	MTASF,  1 },
	{ "blocksize",	MTSETBSIZ, 1 },
	{ "bsf",	MTBSF,	1 },
	{ "bsr",	MTBSR,	1 },
	{ "density",	MTSETDNSTY, 1 },
	{ "eof",	MTWEOF,	0 },
	{ "eom",	MTEOM,	1 },
	{ "erase",	MTERASE, 0 },
	{ "fsf",	MTFSF,	1 },
	{ "fsr",	MTFSR,	1 },
	{ "offline",	MTOFFL,	1 },
	{ "rewind",	MTREW,	1 },
	{ "rewoffl",	MTOFFL,	1 },
	{ "status",	MTNOP,	1 },
	{ "retension",	MTRETEN, 1 },
	{ "weof",	MTWEOF,	0 },
	{ NULL }
};

void printreg __P((char *, u_int, char *));
void status __P((struct mtget *));
void usage __P((void));

int
main(argc, argv)
	int argc;
	char *argv[];
{
	const struct commands *comp;
	struct mtget mt_status;
	struct mtop mt_com;
	int ch, len, mtfd, flags;
	char *p, *tape;
	int count;

	if ((tape = getenv("TAPE")) == NULL)
		tape = DEFTAPE;

	while ((ch = getopt(argc, argv, "f:t:")) != -1)
		switch (ch) {
		case 'f':
		case 't':
			tape = optarg;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc < 1 || argc > 2)
		usage();

	len = strlen(p = *argv++);
	for (comp = com;; comp++) {
		if (comp->c_name == NULL)
			errx(1, "%s: unknown command", p);
		if (strncmp(p, comp->c_name, len) == 0)
			break;
	}

	flags = comp->c_ronly ? O_RDONLY : O_WRONLY;
	if ((mtfd = open(tape, flags)) < 0)
		err(2, "%s", tape);

	if (comp->c_code == MTASF) {
		/* If mtget.mt_fileno was implemented, We could
		   compute the minimal seek needed to position
		   the tape.  Until then, rewind and seek from
		   begining-of-tape */

		/* zero is a valid count */
		if (*argv) {
			count = strtol(*argv, &p, 10);
			if (count < 0 || *p)
				errx(2, "%s: illegal count", *argv);
		}
		else
			count = 1;

		mt_com.mt_op = MTREW;
		mt_com.mt_count = 1;
		if (ioctl(mtfd, MTIOCTOP, &mt_com) < 0)
			err(2, "%s", tape);
		
		mt_com.mt_op = MTFSF;
		mt_com.mt_count = count;
		if (ioctl(mtfd, MTIOCTOP, &mt_com) < 0)
			err(2, "%s", tape);

	} else if (comp->c_code != MTNOP) {

		/* zero is *not* a valid count */
		if (*argv) {
			count = strtol(*argv, &p, 10);
			if (count <= 0 || *p)
				errx(2, "%s: illegal count", *argv);
		}
		else
			count = 1;

		mt_com.mt_op = comp->c_code;
		mt_com.mt_count = count;

		if (ioctl(mtfd, MTIOCTOP, &mt_com) < 0)
			err(2, "%s: %s", tape, comp->c_name);

	} else {
		if (ioctl(mtfd, MTIOCGET, &mt_status) < 0)
			err(2, "%s: %s", tape, comp->c_name);
		status(&mt_status);
	}

	exit(0);
	/* NOTREACHED */
}

#ifdef sun
#include <sundev/tmreg.h>
#include <sundev/arreg.h>
#endif

#ifdef tahoe
#include <tahoe/vba/cyreg.h>
#endif

struct tape_desc {
	short	t_type;		/* type of magtape device */
	char	*t_name;	/* printing name */
	char	*t_dsbits;	/* "drive status" register */
	char	*t_erbits;	/* "error" register */
} tapes[] = {
#ifdef sun
	{ MT_ISCPC,	"TapeMaster",	TMS_BITS,	0 },
	{ MT_ISAR,	"Archive",	ARCH_CTRL_BITS,	ARCH_BITS },
#endif
#ifdef tahoe
	{ MT_ISCY,	"cipher",	CYS_BITS,	CYCW_BITS },
#endif
	{ 0x7,		"SCSI",		"76543210",	"76543210" },
	{ 0 }
};

/*
 * Interpret the status buffer returned
 */
void
status(bp)
	struct mtget *bp;
{
	struct tape_desc *mt;

	for (mt = tapes;; mt++) {
		if (mt->t_type == 0) {
			(void)printf("%d: unknown tape drive type\n",
			    bp->mt_type);
			return;
		}
		if (mt->t_type == bp->mt_type)
			break;
	}
	(void)printf("%s tape drive, residual=%d\n", mt->t_name, bp->mt_resid);
	printreg("ds", bp->mt_dsreg, mt->t_dsbits);
	printreg("\ner", bp->mt_erreg, mt->t_erbits);
	(void)putchar('\n');
	(void)printf("blocksize: %ld (%ld, %ld, %ld, %ld)\n",
		bp->mt_blksiz, bp->mt_mblksiz[0], bp->mt_mblksiz[1],
		bp->mt_mblksiz[2], bp->mt_mblksiz[3]);
	(void)printf("density: %ld (%ld, %ld, %ld, %ld)\n",
		bp->mt_density, bp->mt_mdensity[0], bp->mt_mdensity[1],
		bp->mt_mdensity[2], bp->mt_mdensity[3]);
}

/*
 * Print a register a la the %b format of the kernel's printf.
 */
void
printreg(s, v, bits)
	char *s;
	u_int v;
	char *bits;
{
	int i, any = 0;
	char c;

	if (bits && *bits == 8)
		printf("%s=%o", s, v);
	else
		printf("%s=%x", s, v);
	bits++;
	if (v && bits) {
		putchar('<');
		while ((i = *bits++)) {
			if (v & (1 << (i-1))) {
				if (any)
					putchar(',');
				any = 1;
				for (; (c = *bits) > 32; bits++)
					putchar(c);
			} else
				for (; *bits > 32; bits++)
					;
		}
		putchar('>');
	}
}

void
usage()
{
	(void)fprintf(stderr, "usage: mt [-f device] command [ count ]\n");
	exit(1);
}
