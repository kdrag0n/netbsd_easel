/*	$NetBSD: options.c,v 1.45 2002/10/15 14:51:04 christos Exp $	*/

/*-
 * Copyright (c) 1992 Keith Muller.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Keith Muller of the University of California, San Diego.
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
#if defined(__RCSID) && !defined(lint)
#if 0
static char sccsid[] = "@(#)options.c	8.2 (Berkeley) 4/18/94";
#else
__RCSID("$NetBSD: options.c,v 1.45 2002/10/15 14:51:04 christos Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mtio.h>
#include <sys/param.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <paths.h>
#include "pax.h"
#include "options.h"
#include "cpio.h"
#include "tar.h"
#include "extern.h"
#ifndef SMALL
#include "mtree.h"
#endif	/* SMALL */

/*
 * Routines which handle command line options
 */

static int nopids;		/* tar mode: suppress "pids" for -p option */
static char *flgch = FLGCH;	/* list of all possible flags (pax) */
static OPLIST *ophead = NULL;	/* head for format specific options -x */
static OPLIST *optail = NULL;	/* option tail */

static int no_op(void);
static void printflg(unsigned int);
static int c_frmt(const void *, const void *);
static off_t str_offt(char *);
static char *getline(FILE *fp);
static void pax_options(int, char **);
static void pax_usage(void);
static void tar_options(int, char **);
static void tar_usage(void);
static void cpio_options(int, char **);
static void cpio_usage(void);

/* errors from getline */
#define GETLINE_FILE_CORRUPT 1
#define GETLINE_OUT_OF_MEM 2
static int getline_error;

#define GZIP_CMD	"gzip"		/* command to run as gzip */
#define COMPRESS_CMD	"compress"	/* command to run as compress */

/*
 *	Format specific routine table - MUST BE IN SORTED ORDER BY NAME
 *	(see pax.h for description of each function)
 *
 *	name, blksz, hdsz, udev, hlk, blkagn, inhead, id, st_read,
 *	read, end_read, st_write, write, end_write, trail,
 *	rd_data, wr_data, options
 */

FSUB fsub[] = {
/* 0: OLD BINARY CPIO */
	{ "bcpio", 5120, sizeof(HD_BCPIO), 1, 0, 0, 1, bcpio_id, cpio_strd,
	bcpio_rd, bcpio_endrd, cpio_stwr, bcpio_wr, cpio_endwr, NULL,
	cpio_subtrail, rd_wrfile, wr_rdfile, bad_opt },

/* 1: OLD OCTAL CHARACTER CPIO */
	{ "cpio", 5120, sizeof(HD_CPIO), 1, 0, 0, 1, cpio_id, cpio_strd,
	cpio_rd, cpio_endrd, cpio_stwr, cpio_wr, cpio_endwr, NULL,
	cpio_subtrail, rd_wrfile, wr_rdfile, bad_opt },

/* 2: SVR4 HEX CPIO */
	{ "sv4cpio", 5120, sizeof(HD_VCPIO), 1, 0, 0, 1, vcpio_id, cpio_strd,
	vcpio_rd, vcpio_endrd, cpio_stwr, vcpio_wr, cpio_endwr, NULL,
	cpio_subtrail, rd_wrfile, wr_rdfile, bad_opt },

/* 3: SVR4 HEX CPIO WITH CRC */
	{ "sv4crc", 5120, sizeof(HD_VCPIO), 1, 0, 0, 1, crc_id, crc_strd,
	vcpio_rd, vcpio_endrd, crc_stwr, vcpio_wr, cpio_endwr, NULL,
	cpio_subtrail, rd_wrfile, wr_rdfile, bad_opt },

/* 4: OLD TAR */
	{ "tar", 10240, BLKMULT, 0, 1, BLKMULT, 0, tar_id, no_op,
	tar_rd, tar_endrd, no_op, tar_wr, tar_endwr, tar_trail,
	NULL, rd_wrfile, wr_rdfile, tar_opt },

/* 5: POSIX USTAR */
	{ "ustar", 10240, BLKMULT, 0, 1, BLKMULT, 0, ustar_id, ustar_strd,
	ustar_rd, tar_endrd, ustar_stwr, ustar_wr, tar_endwr, tar_trail,
	NULL, rd_wrfile, wr_rdfile, bad_opt }
};
#define F_BCPIO		0	/* old binary cpio format */
#define F_CPIO		1	/* old octal character cpio format */
#define F_SV4CPIO	2	/* SVR4 hex cpio format */
#define F_SV4CRC	3	/* SVR4 hex with crc cpio format */
#define F_TAR		4	/* old V7 UNIX tar format */
#define F_USTAR		5	/* ustar format */
#define DEFLT		F_USTAR	/* default write format from list above */

/*
 * ford is the archive search order used by get_arc() to determine what kind
 * of archive we are dealing with. This helps to properly id  archive formats
 * some formats may be subsets of others....
 */
int ford[] = {F_USTAR, F_TAR, F_SV4CRC, F_SV4CPIO, F_CPIO, F_BCPIO, -1};

/*
 * options()
 *	figure out if we are pax, tar or cpio. Call the appropriate options
 *	parser
 */

void
options(int argc, char **argv)
{

	/*
	 * Are we acting like pax, tar or cpio (based on argv[0])
	 */
	if ((argv0 = strrchr(argv[0], '/')) != NULL)
		argv0++;
	else
		argv0 = argv[0];

	if (strcmp(NM_TAR, argv0) == 0)
		return(tar_options(argc, argv));
	else if (strcmp(NM_CPIO, argv0) == 0)
		return(cpio_options(argc, argv));
	else {
		argv0 = NM_PAX;
		return(pax_options(argc, argv));
	}
}

/*
 * pax_options()
 *	look at the user specified flags. set globals as required and check if
 *	the user specified a legal set of flags. If not, complain and exit
 */

static void
pax_options(int argc, char **argv)
{
	int c;
	int i;
	unsigned int flg = 0;
	unsigned int bflg = 0;
	char *pt;
	FSUB tmp;

	/*
	 * process option flags
	 */
	while ((c = getopt(argc, argv,
	    "ab:cdf:iklno:p:rs:tuvwx:zAB:DE:G:HLMN:OPT:U:XYZ")) != -1) {
		switch (c) {
		case 'a':
			/*
			 * append
			 */
			flg |= AF;
			break;
		case 'b':
			/*
			 * specify blocksize
			 */
			flg |= BF;
			if ((wrblksz = (int)str_offt(optarg)) <= 0) {
				tty_warn(1, "Invalid block size %s", optarg);
				pax_usage();
			}
			break;
		case 'c':
			/*
			 * inverse match on patterns
			 */
			cflag = 1;
			flg |= CF;
			break;
		case 'd':
			/*
			 * match only dir on extract, not the subtree at dir
			 */
			dflag = 1;
			flg |= DF;
			break;
		case 'f':
			/*
			 * filename where the archive is stored
			 */
			arcname = optarg;
			flg |= FF;
			break;
		case 'i':
			/*
			 * interactive file rename
			 */
			iflag = 1;
			flg |= IF;
			break;
		case 'k':
			/*
			 * do not clobber files that exist
			 */
			kflag = 1;
			flg |= KF;
			break;
		case 'l':
			/*
			 * try to link src to dest with copy (-rw)
			 */
			lflag = 1;
			flg |= LF;
			break;
		case 'n':
			/*
			 * select first match for a pattern only
			 */
			nflag = 1;
			flg |= NF;
			break;
		case 'o':
			/*
			 * pass format specific options
			 */
			flg |= OF;
			if (opt_add(optarg) < 0)
				pax_usage();
			break;
		case 'p':
			/*
			 * specify file characteristic options
			 */
			for (pt = optarg; *pt != '\0'; ++pt) {
				switch(*pt) {
				case 'a':
					/*
					 * do not preserve access time
					 */
					patime = 0;
					break;
				case 'e':
					/*
					 * preserve user id, group id, file
					 * mode, access/modification times
					 * and file flags.
					 */
					pids = 1;
					pmode = 1;
					patime = 1;
					pmtime = 1;
					pfflags = 1;
					break;
#if 0
				case 'f':
					/*
					 * do not preserve file flags
					 */
					pfflags = 0;
					break;
#endif
				case 'm':
					/*
					 * do not preserve modification time
					 */
					pmtime = 0;
					break;
				case 'o':
					/*
					 * preserve uid/gid
					 */
					pids = 1;
					break;
				case 'p':
					/*
					 * preserve file mode bits
					 */
					pmode = 1;
					break;
				default:
					tty_warn(1, "Invalid -p string: %c",
					    *pt);
					pax_usage();
					break;
				}
			}
			flg |= PF;
			break;
		case 'r':
			/*
			 * read the archive
			 */
			flg |= RF;
			break;
		case 's':
			/*
			 * file name substitution name pattern
			 */
			if (rep_add(optarg) < 0) {
				pax_usage();
				break;
			}
			flg |= SF;
			break;
		case 't':
			/*
			 * preserve access time on filesystem nodes we read
			 */
			tflag = 1;
			flg |= TF;
			break;
		case 'u':
			/*
			 * ignore those older files
			 */
			uflag = 1;
			flg |= UF;
			break;
		case 'v':
			/*
			 * verbose operation mode
			 */
			vflag = 1;
			flg |= VF;
			break;
		case 'w':
			/*
			 * write an archive
			 */
			flg |= WF;
			break;
		case 'x':
			/*
			 * specify an archive format on write
			 */
			tmp.name = optarg;
			frmt = (FSUB *)bsearch((void *)&tmp, (void *)fsub,
			    sizeof(fsub)/sizeof(FSUB), sizeof(FSUB), c_frmt);
			if (frmt != NULL) {
				flg |= XF;
				break;
			}
			tty_warn(1, "Unknown -x format: %s", optarg);
			(void)fputs("pax: Known -x formats are:", stderr);
			for (i = 0; i < (sizeof(fsub)/sizeof(FSUB)); ++i)
				(void)fprintf(stderr, " %s", fsub[i].name);
			(void)fputs("\n\n", stderr);
			pax_usage();
			break;
		case 'z':
			/*
			 * use gzip.  Non standard option.
			 */
			gzip_program = GZIP_CMD;
			break;
		case 'A':
			Aflag = 1;
			flg |= CAF;
			break;
		case 'B':
			/*
			 * non-standard option on number of bytes written on a
			 * single archive volume.
			 */
			if ((wrlimit = str_offt(optarg)) <= 0) {
				tty_warn(1, "Invalid write limit %s", optarg);
				pax_usage();
			}
			if (wrlimit % BLKMULT) {
				tty_warn(1,
				    "Write limit is not a %d byte multiple",
				    BLKMULT);
				pax_usage();
			}
			flg |= CBF;
			break;
		case 'D':
			/*
			 * On extraction check file inode change time before the
			 * modification of the file name. Non standard option.
			 */
			Dflag = 1;
			flg |= CDF;
			break;
		case 'E':
			/*
			 * non-standard limit on read faults
			 * 0 indicates stop after first error, values
			 * indicate a limit, "NONE" try forever
			 */
			flg |= CEF;
			if (strcmp(NONE, optarg) == 0)
				maxflt = -1;
			else if ((maxflt = atoi(optarg)) < 0) {
				tty_warn(1,
				    "Error count value must be positive");
				pax_usage();
			}
			break;
		case 'G':
			/*
			 * non-standard option for selecting files within an
			 * archive by group (gid or name)
			 */
			if (grp_add(optarg) < 0) {
				pax_usage();
				break;
			}
			flg |= CGF;
			break;
		case 'H':
			/*
			 * follow command line symlinks only
			 */
			Hflag = 1;
			flg |= CHF;
			break;
		case 'L':
			/*
			 * follow symlinks
			 */
			Lflag = 1;
			flg |= CLF;
			break;
#ifdef SMALL
		case 'M':
		case 'N':
			tty_warn(1, "Support for -%c is not compiled in", c);
			exit(1);
#else	/* !SMALL */
		case 'M':
			/*
			 * Treat list of filenames on stdin as an
			 * mtree(8) specfile.  Non standard option.
			 */
			Mflag = 1;
			flg |= CMF;
			break;
		case 'N':
			/*
			 * Use alternative directory for user db lookups.
			 */
			if (!setup_getid(optarg)) {
				tty_warn(1,
			    "Unable to use user and group databases in `%s'",
				    optarg);
				pax_usage();
			}
			break;
#endif	/* !SMALL */
		case 'O':
			/*
			 * Force one volume.  Non standard option.
			 */
			force_one_volume = 1;
			break;
		case 'P':
			/*
			 * do NOT follow symlinks (default)
			 */
			Lflag = 0;
			flg |= CPF;
			break;
		case 'T':
			/*
			 * non-standard option for selecting files within an
			 * archive by modification time range (lower,upper)
			 */
			if (trng_add(optarg) < 0) {
				pax_usage();
				break;
			}
			flg |= CTF;
			break;
		case 'U':
			/*
			 * non-standard option for selecting files within an
			 * archive by user (uid or name)
			 */
			if (usr_add(optarg) < 0) {
				pax_usage();
				break;
			}
			flg |= CUF;
			break;
		case 'X':
			/*
			 * do not pass over mount points in the file system
			 */
			Xflag = 1;
			flg |= CXF;
			break;
		case 'Y':
			/*
			 * On extraction check file inode change time after the
			 * modification of the file name. Non standard option.
			 */
			Yflag = 1;
			flg |= CYF;
			break;
		case 'Z':
			/*
			 * On extraction check modification time after the
			 * modification of the file name. Non standard option.
			 */
			Zflag = 1;
			flg |= CZF;
			break;
		case '?':
		default:
			pax_usage();
			break;
		}
	}

	/*
	 * figure out the operation mode of pax read,write,extract,copy,append
	 * or list. check that we have not been given a bogus set of flags
	 * for the operation mode.
	 */
	if (ISLIST(flg)) {
		act = LIST;
		listf = stdout;
		bflg = flg & BDLIST;
	} else if (ISEXTRACT(flg)) {
		act = EXTRACT;
		bflg = flg & BDEXTR;
	} else if (ISARCHIVE(flg)) {
		act = ARCHIVE;
		bflg = flg & BDARCH;
	} else if (ISAPPND(flg)) {
		act = APPND;
		bflg = flg & BDARCH;
	} else if (ISCOPY(flg)) {
		act = COPY;
		bflg = flg & BDCOPY;
	} else
		pax_usage();
	if (bflg) {
		printflg(flg);
		pax_usage();
	}

	/*
	 * if we are writing (ARCHIVE) we use the default format if the user
	 * did not specify a format. when we write during an APPEND, we will
	 * adopt the format of the existing archive if none was supplied.
	 */
	if (!(flg & XF) && (act == ARCHIVE))
		frmt = &(fsub[DEFLT]);

	/*
	 * process the args as they are interpreted by the operation mode
	 */
	switch (act) {
	case LIST:
	case EXTRACT:
		for (; optind < argc; optind++)
			if (pat_add(argv[optind], NULL) < 0)
				pax_usage();
		break;
	case COPY:
		if (optind >= argc) {
			tty_warn(0, "Destination directory was not supplied");
			pax_usage();
		}
		--argc;
		dirptr = argv[argc];
		if (mkpath(dirptr) < 0)
			cpio_usage();
		/* FALLTHROUGH */
	case ARCHIVE:
	case APPND:
		for (; optind < argc; optind++)
			if (ftree_add(argv[optind], 0) < 0)
				pax_usage();
		/*
		 * no read errors allowed on updates/append operation!
		 */
		maxflt = 0;
		break;
	}
}


/*
 * tar_options()
 *	look at the user specified flags. set globals as required and check if
 *	the user specified a legal set of flags. If not, complain and exit
 */

#define	OPT_USE_COMPRESS_PROGRAM	0
#define	OPT_CHECKPOINT			1
#define	OPT_UNLINK			2
#define	OPT_HELP			3
#define	OPT_ATIME_PRESERVE		4
#define	OPT_FAST_READ			5
#define	OPT_IGNORE_FAILED_READ		6
#define	OPT_REMOVE_FILES		7
#define	OPT_NULL			8
#define	OPT_TOTALS			9
#define	OPT_VERSION			10
#define	OPT_EXCLUDE			11
#define	OPT_BLOCK_COMPRESS		12
#define	OPT_NORECURSE			13
#define	OPT_FORCE_LOCAL			14

struct option tar_longopts[] = {
	{ "block-size",		required_argument,	0,	'b' },
	{ "create",		no_argument,		0,	'c' },	/* F */
	/* -e -- no corresponding long option */
	{ "file",		required_argument,	0,	'f' },
	{ "dereference",	no_argument,		0,	'h' },
	{ "one-file-system",	no_argument,		0,	'l' },
	{ "modification-time",	no_argument,		0,	'm' },
	{ "old-archive",	no_argument,		0,	'o' },
	{ "portability",	no_argument,		0,	'o' },
	{ "same-permissions",	no_argument,		0,	'p' },
	{ "preserve-permissions", no_argument,		0,	'p' },
	{ "preserve",		no_argument,		0,	'p' },
	{ "append",		no_argument,		0,	'r' },	/* F */
	{ "update",		no_argument,		0,	'u' },	/* F */
	{ "list",		no_argument,		0,	't' },	/* F */
	{ "verbose",		no_argument,		0,	'v' },
	{ "interactive",	no_argument,		0,	'w' },
	{ "confirmation",	no_argument,		0,	'w' },
	{ "extract",		no_argument,		0,	'x' },	/* F */
	{ "get",		no_argument,		0,	'x' },	/* F */
	{ "gzip",		no_argument,		0,	'z' },
	{ "gunzip",		no_argument,		0,	'z' },
	{ "read-full-blocks",	no_argument,		0,	'B' },
	{ "directory",		required_argument,	0,	'C' },
	{ "tape-length",	required_argument,	0,	'L' },
	{ "absolute-paths",	no_argument,		0,	'P' },
	{ "files-from",		required_argument,	0,	'T' },
	{ "exclude-from",	required_argument,	0,	'X' },
	{ "compress",		no_argument,		0,	'Z' },
	{ "uncompress",		no_argument,		0,	'Z' },
	{ "atime-preserve",	no_argument,		0,
						OPT_ATIME_PRESERVE },
	{ "unlink",		no_argument,		0,
						OPT_UNLINK },
	{ "use-compress-program", required_argument,	0,
						OPT_USE_COMPRESS_PROGRAM },
	{ "force-local",	no_argument,		0,
						OPT_FORCE_LOCAL },
#if 0 /* Not implemented */
	{ "catenate",		no_argument,		0,	'A' },	/* F */
	{ "concatenate",	no_argument,		0,	'A' },	/* F */
	{ "diff",		no_argument,		0,	'd' },	/* F */
	{ "compare",		no_argument,		0,	'd' },	/* F */
	{ "checkpoint",		no_argument,		0,
						OPT_CHECKPOINT },
	{ "help",		no_argument,		0,
						OPT_HELP },
	{ "info-script",	required_argument,	0,	'F' },
	{ "new-volume-script",	required_argument,	0,	'F' },
	{ "fast-read",		no_argument,		0,
						OPT_FAST_READ },
	{ "incremental",	no_argument,		0,	'G' },
	{ "listed-incremental",	required_argument,	0,	'g' },
	{ "ignore-zeros",	no_argument,		0,	'i' },
	{ "ignore-failed-read",	no_argument,		0,
						OPT_IGNORE_FAILED_READ },
	{ "keep-old-files",	no_argument,		0,	'k' },
	{ "starting-file",	no_argument,		0,	'K' },
	{ "multi-volume",	no_argument,		0,	'M' },
	{ "after-date",		required_argument,	0,	'N' },
	{ "newer",		required_argument,	0,	'N' },
	{ "to-stdout",		no_argument,		0,	'O' },
	{ "record-number",	no_argument,		0,	'R' },
	{ "remove-files",	no_argument,		0,
						OPT_REMOVE_FILES },
	{ "same-order",		no_argument,		0,	's' },
	{ "preserve-order",	no_argument,		0,	's' },
	{ "sparse",		no_argument,		0,	'S' },
	{ "null",		no_argument,		0,
						OPT_NULL },
	{ "totals",		no_argument,		0,
						OPT_TOTALS },
	{ "volume-name",	required_argument,	0,	'V' },
	{ "label",		required_argument,	0,	'V' },
	{ "version",		no_argument,		0,
						OPT_VERSION },
	{ "verify",		no_argument,		0,	'W' },
	{ "exclude",		required_argument,	0,
						OPT_EXCLUDE },
	{ "block-compress",	no_argument,		0,
						OPT_BLOCK_COMPRESS },
	{ "norecurse",		no_argument,		0,
						OPT_NORECURSE },
#endif
	{ 0,			0,			0,	0 },
};

static void
tar_options(int argc, char **argv)
{
	int c;
	int fstdin = 0;
	int Oflag = 0;
	int nincfiles = 0;
	int incfiles_max = 0;
	struct incfile {
		char *file;
		char *dir;
	};
	struct incfile *incfiles = NULL;

	/*
	 * Set default values.
	 */
	rmleadslash = 1;

	/*
	 * process option flags
	 */
	while ((c = getoldopt(argc, argv,
	    "+b:cef:hlmopqrstuvwxzBC:HI:LOPT:X:Z014578",
	    tar_longopts, NULL))
	    != -1)  {
		switch(c) {
		case 'b':
			/*
			 * specify blocksize in 512-byte blocks
			 */
			if ((wrblksz = (int)str_offt(optarg)) <= 0) {
				tty_warn(1, "Invalid block size %s", optarg);
				tar_usage();
			}
			wrblksz *= 512;		/* XXX - check for int oflow */
			break;
		case 'c':
			/*
			 * create an archive
			 */
			act = ARCHIVE;
			break;
		case 'e':
			/*
			 * stop after first error
			 */
			maxflt = 0;
			break;
		case 'f':
			/*
			 * filename where the archive is stored
			 */
			if ((optarg[0] == '-') && (optarg[1]== '\0')) {
				/*
				 * treat a - as stdin
				 */
				fstdin = 1;
				arcname = NULL;
				break;
			}
			fstdin = 0;
			arcname = optarg;
			break;
		case 'h':
			/*
			 * follow command line symlinks only
			 */
			Hflag = 1;
			break;
		case 'l':
			/*
			 * do not pass over mount points in the file system
			 */
			Xflag = 1;
			break;
		case 'm':
			/*
			 * do not preserve modification time
			 */
			pmtime = 0;
			break;
		case 'o':
			/*
			 * This option does several things based on whether
			 * this is a create or extract operation.
			 */
			if (act == ARCHIVE) {
				/* 4.2BSD: don't add directory entries. */
				if (opt_add("write_opt=nodir") < 0)
					tar_usage();

				/* GNU tar: write V7 format archives. */
				frmt = &(fsub[F_TAR]);
			} else {
				/* SUS: don't preserve owner/group. */
				pids = 0;
				nopids = 1;
			}
			break;
		case 'O':
			Oflag = 1;
			break;
		case 'p':
			/*
			 * preserve user id, group id, file
			 * mode, access/modification times
			 */
			if (!nopids)
				pids = 1;
			pmode = 1;
			patime = 1;
			pmtime = 1;
			break;
		case 'q':
			/*
			 * select first match for a pattern only
			 */
			nflag = 1;
			break;
		case 'r':
		case 'u':
			/*
			 * append to the archive
			 */
			act = APPND;
			break;
		case 's':
			/*
			 * file name substitution name pattern
			 */
			if (rep_add(optarg) < 0) {
				tar_usage();
				break;
			}
			break;
		case 't':
			/*
			 * list contents of the tape
			 */
			act = LIST;
			break;
		case 'v':
			/*
			 * verbose operation mode
			 */
			vflag = 1;
			break;
		case 'w':
			/*
			 * interactive file rename
			 */
			iflag = 1;
			break;
		case 'x':
			/*
			 * extract an archive, preserving mode,
			 * and mtime if possible.
			 */
			act = EXTRACT;
			pmtime = 1;
			break;
		case 'z':
			/*
			 * use gzip.  Non standard option.
			 */
			zflag = 1;
			gzip_program = GZIP_CMD;
			break;
		case 'B':
			/*
			 * Nothing to do here, this is pax default
			 */
			break;
		case 'C':
			chdname = optarg;
			break;
		case 'H':
			/*
			 * follow command line symlinks only
			 */
			Hflag = 1;
			break;
		case 'I':
		case 'T':
			if (++nincfiles > incfiles_max) {
				incfiles_max = nincfiles + 3;
				incfiles = realloc(incfiles,
				    sizeof(*incfiles) * incfiles_max);
				if (incfiles == NULL) {
					tty_warn(0, "Unable to allocate space "
					    "for option list");
					exit(1);
				}
			}
			incfiles[nincfiles - 1].file = optarg;
			incfiles[nincfiles - 1].dir = chdname;
			break;
		case 'L':
			/*
			 * follow symlinks
			 */
			Lflag = 1;
			break;
		case 'P':
			/*
			 * do not remove leading '/' from pathnames
			 */
			rmleadslash = 0;
			Aflag = 1;
			break;
		case 'X':
			/*
			 * GNU tar compat: exclude the files listed in optarg
			 */
			if (tar_gnutar_X_compat(optarg) != 0)
				tar_usage();
			break;
		case 'Z':
			/*
			 * use compress.
			 */
			zflag = 1;
			gzip_program = COMPRESS_CMD;
			break;
		case '0':
			arcname = DEV_0;
			break;
		case '1':
			arcname = DEV_1;
			break;
		case '4':
			arcname = DEV_4;
			break;
		case '5':
			arcname = DEV_5;
			break;
		case '7':
			arcname = DEV_7;
			break;
		case '8':
			arcname = DEV_8;
			break;
		case OPT_ATIME_PRESERVE:
			patime = 1;
			break;
		case OPT_UNLINK:
			/* Just ignore -- we always unlink first. */
			break;
		case OPT_USE_COMPRESS_PROGRAM:
			zflag = 1;
			gzip_program = optarg;
			break;
		case OPT_FORCE_LOCAL:
			forcelocal = 1;
			break;
		default:
			tar_usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	/* Traditional tar behaviour (pax uses stderr unless in list mode) */
	if (fstdin == 1 && act == ARCHIVE)
		listf = stderr;
	else
		listf = stdout;

	/* Traditional tar behaviour (pax wants to read file list from stdin) */
	if ((act == ARCHIVE || act == APPND) && argc == 0 && nincfiles == 0)
		exit(0);
	/*
	 * if we are writing (ARCHIVE) specify tar, otherwise run like pax
	 * (unless -o specified)
	 */
	if (act == ARCHIVE || act == APPND)
		frmt = &(fsub[Oflag ? F_TAR : F_USTAR]);
	else if (Oflag) {
		tty_warn(1, "The -O/-o options are only valid when writing an archive");
		tar_usage();		/* only valid when writing */
	}

	/*
	 * process the args as they are interpreted by the operation mode
	 */
	switch (act) {
	case LIST:
	case EXTRACT:
	default:
		{
			int sawpat = 0;
			char *file, *dir;

			while (nincfiles || *argv != NULL) {
				/*
				 * If we queued up any include files,
				 * pull them in now.  Otherwise, check
				 * for -I and -C positional flags.
				 * Anything else must be a file to
				 * extract.
				 */
				if (nincfiles) {
					file = incfiles->file;
					dir = incfiles->dir;
					incfiles++;
					nincfiles--;
				} else if (strcmp(*argv, "-I") == 0) {
					if (*++argv == NULL)
						break;
					file = *argv++;
					dir = chdname;
				} else
					file = NULL;
				if (file != NULL) {
					FILE *fp;
					char *str;

					if (strcmp(file, "-") == 0)
						fp = stdin;
					else if ((fp = fopen(file, "r")) == NULL) {
						tty_warn(1, "Unable to open file '%s' for read", file);
						tar_usage();
					}
					while ((str = getline(fp)) != NULL) {
						if (pat_add(str, dir) < 0)
							tar_usage();
						sawpat = 1;
					}
					if (strcmp(file, "-") != 0)
						fclose(fp);
					if (getline_error) {
						tty_warn(1, "Problem with file '%s'", file);
						tar_usage();
					}
				} else if (strcmp(*argv, "-C") == 0) {
					if (*++argv == NULL)
 						break;
					chdname = *argv++;
				} else if (pat_add(*argv++, chdname) < 0)
					tar_usage();
				else
					sawpat = 1;
			}
			/*
			 * if patterns were added, we are doing	chdir()
			 * on a file-by-file basis, else, just one
			 * global chdir (if any) after opening input.
			 */
			if (sawpat > 0)
				chdname = NULL;
		}
		break;
	case ARCHIVE:
	case APPND:
		if (chdname != NULL) {	/* initial chdir() */
			if (ftree_add(chdname, 1) < 0)
				tar_usage();
		}

		while (nincfiles || *argv != NULL) {
			char *file, *dir;

			/*
			 * If we queued up any include files, pull them in
			 * now.  Otherwise, check for -I and -C positional
			 * flags.  Anything else must be a file to include
			 * in the archive.
			 */
			if (nincfiles) {
				file = incfiles->file;
				dir = incfiles->dir;
				incfiles++;
				nincfiles--;
			} else if (strcmp(*argv, "-I") == 0) {
				if (*++argv == NULL)
					break;
				file = *argv++;
				dir = NULL;
			} else
				file = NULL;
			if (file != NULL) {
				FILE *fp;
				char *str;

				/* Set directory if needed */
				if (dir) {
					if (ftree_add(dir, 1) < 0)
						tar_usage();
				}

				if (strcmp(file, "-") == 0)
					fp = stdin;
				else if ((fp = fopen(file, "r")) == NULL) {
					tty_warn(1, "Unable to open file '%s' for read", file);
					tar_usage();
				}
				while ((str = getline(fp)) != NULL) {
					if (ftree_add(str, 0) < 0)
						tar_usage();
				}
				if (strcmp(file, "-") != 0)
					fclose(fp);
				if (getline_error) {
					tty_warn(1, "Problem with file '%s'",
					    file);
					tar_usage();
				}
			} else if (strcmp(*argv, "-C") == 0) {
				if (*++argv == NULL)
					break;
				if (ftree_add(*argv++, 1) < 0)
					tar_usage();
			} else if (ftree_add(*argv++, 0) < 0)
				tar_usage();
		}
		/*
		 * no read errors allowed on updates/append operation!
		 */
		maxflt = 0;
		break;
	}
	if (!fstdin && ((arcname == (char *)NULL) || (*arcname == '\0'))) {
		arcname = getenv("TAPE");
		if ((arcname == NULL) || (*arcname == '\0'))
			arcname = _PATH_DEFTAPE;
	}
}

int
mkpath(path)
	char *path;
{
	struct stat sb;
	char *slash;
	int done = 0;

	slash = path;

	while (!done) {
		slash += strspn(slash, "/");
		slash += strcspn(slash, "/");

		done = (*slash == '\0');
		*slash = '\0';

		if (stat(path, &sb)) {
			if (errno != ENOENT || mkdir(path, 0777)) {
				tty_warn(1, "%s", path);
				return (-1);
			}
		} else if (!S_ISDIR(sb.st_mode)) {
			syswarn(1, ENOTDIR, "%s", path);
			return (-1);
		}

		if (!done)
			*slash = '/';
	}

	return (0);
}


struct option cpio_longopts[] = {
	{ "reset-access-time",	no_argument,		0,	'a' },
	{ "make-directories",	no_argument,		0, 	'd' },
	{ "nonmatching",	no_argument,		0,	'f' },
	{ "extract",		no_argument,		0,	'i' },
	{ "link",		no_argument,		0,	'l' },
	{ "preserve-modification-time", no_argument,	0,	'm' },
	{ "create",		no_argument,		0,	'o' },
	{ "pass-through",	no_argument,		0,	'p' },
	{ "rename",		no_argument,		0,	'r' },
	{ "list",		no_argument,		0,	't' },
	{ "unconditional",	no_argument,		0,	'u' },
	{ "verbose",		no_argument,		0,	'v' },
	{ "append",		no_argument,		0,	'A' },
	{ "pattern-file",	required_argument,	0,	'E' },
	{ "file",		required_argument,	0,	'F' },
	{ "force-local",	no_argument,		0,
						    OPT_FORCE_LOCAL },
	{ "format",		required_argument,	0,	'H' },
	{ "dereference",	no_argument,		0,	'L' },
	{ "swap-halfwords",	no_argument,		0,	'S' },

#ifdef notyet
/* Not implemented */
	{ "null",		no_argument,		0,	'0' },
	{ "swap",		no_argument,		0,	'b' },
	{ "numeric-uid-gid",	no_argument,		0,	'n' },
	{ "swap-bytes",		no_argument,		0,	's' },
	{ "message",		required_argument,	0,	'M' },
	{ "owner",		required_argument,	0	'R' },
	{ "dot",		no_argument,		0,	'V' },
	{ "block-size",		required_argument,	0,
						    OPT_BLOCK_SIZE },
	{ "no-absolute-pathnames", no_argument,		0,
						    OPT_NO_ABSOLUTE_PATHNAMES },
	{ "no-preserve-owner",	no_argument,		0,
						    OPT_NO_PRESERVE_OWNER },
	{ "only-verify-crc",	no_argument,		0,
						    OPT_ONLY_VERIFY_CRC },
	{ "rsh-command",	required_argument,	0,
						    OPT_RSH_COMMAND },
	{ "sparce",		no_argument,		0,
						    OPT_SPARSE },
	{ "version",		no_argument,		0,
						    OPT_VERSION },
#endif
};

/*
 * cpio_options()
 *	look at the user specified flags. set globals as required and check if
 *	the user specified a legal set of flags. If not, complain and exit
 */

static void
cpio_options(int argc, char **argv)
{
	FSUB tmp;
	unsigned int flg = 0;
	unsigned int bflg = 0;
	int c, i;
	FILE *fp;
	char *str;

	uflag = 1;
	kflag = 1;
	pids = 1;
	pmode = 1;
	pmtime = 0;
	arcname = NULL;
	dflag = 1;
	act = -1;
	nodirs = 1;
	/*
	 * process option flags
	 */
	while ((c = getoldopt(argc, argv,
	    "abcdfiklmoprstuvzABC:E:F:H:I:LM:O:R:SVZ6",
	    cpio_longopts, NULL)) != -1) {
		switch(c) {
		case 'a':
			/*
			 * preserve access time on filesystem nodes we read
			 */
			tflag = 1;
			flg |= TF;
			break;
#ifdef notyet
		case 'b':
			/*
			 * swap bytes and half-words when reading data
			 */
			break;
#endif
		case 'c':
			/*
			 * ASCII cpio header
			 */
			frmt = &fsub[F_SV4CPIO];
			break;
		case 'd':
			/*
			 * create directories as needed
			 * pax does this by default ..
			 */
			nodirs = 0;
			flg |= RF;
			break;
		case 'f':
			/*
			 * inverse match on patterns
			 */
			cflag = 1;
			flg |= CF;
			break;
		case 'i':
			/*
			 * read the archive
			 */
			act = EXTRACT;
			flg |= RF;
			break;
#ifdef notyet
		case 'k':
			break;
#endif
		case 'l':
			/*
			 * try to link src to dest with copy (-rw)
			 */
			lflag = 1;
			flg |= LF;
			break;
		case 'm':
			/*
			 * preserve mtime
			 */
			flg |= PF;
			pmtime = 1;
			break;
		case 'o':
			/*
			 * write an archive
			 */
			act = ARCHIVE;
			frmt = &(fsub[F_SV4CRC]);
			flg |= WF;
			break;
		case 'p':
			/*
			 * cpio -p is like pax -rw
			 */
			act = COPY;
			flg |= RF | WF;
			break;
		case 'r':
			/*
			 * interactive file rename
			 */
			iflag = 1;
			flg |= IF;
			break;
#ifdef notyet
		case 's':
			/*
			 * swap bytes after reading data
			 */
			break;
#endif
		case 't':
			/*
			 * list contents of archive
			 */
			act = LIST;
			listf = stdout;
			break;
		case 'u':
			/*
			 * don't ignore those older files
			 */
			uflag = 0;
			kflag = 0;
			flg |= UF;
			break;
		case 'v':
			/*
			 * verbose operation mode
			 */
			vflag = 1;
			flg |= VF;
			break;
		case 'z':
			/*
			 * use gzip.  Non standard option.
			 */
			gzip_program = GZIP_CMD;
			break;
		case 'A':
			/*
			 * append to an archive
			 */
			act = APPND;
			flg |= AF;
			break;
		case 'B':
			/*
			 * set blocksize to 5120
			 */
			blksz = 5120;
			break;
		case 'C':
			/*
			 * specify blocksize
			 */
			if ((blksz = (int)str_offt(optarg)) <= 0) {
				tty_warn(1, "Invalid block size %s", optarg);
				tar_usage();
			}
			break;
		case 'E':
			/*
			 * file with patterns to extract or list
			 */
			if ((fp = fopen(optarg, "r")) == NULL) {
				tty_warn(1, "Unable to open file '%s' for read",
				    optarg);
				cpio_usage();
			}
			while ((str = getline(fp)) != NULL) {
				pat_add(str, NULL);
			}
			fclose(fp);
			if (getline_error) {
				tty_warn(1, "Problem with file '%s'", optarg);
				cpio_usage();
			}
			break;
		case 'H':
			/*
			 * specify an archive format on write
			 */
			tmp.name = optarg;
			frmt = (FSUB *)bsearch((void *)&tmp, (void *)fsub,
			    sizeof(fsub)/sizeof(FSUB), sizeof(FSUB), c_frmt);
			if (frmt != NULL) {
				flg |= XF;
				break;
			}
			tty_warn(1, "Unknown -H format: %s", optarg);
			(void)fputs("cpio: Known -H formats are:", stderr);
			for (i = 0; i < (sizeof(fsub)/sizeof(FSUB)); ++i)
				(void)fprintf(stderr, " %s", fsub[i].name);
			(void)fputs("\n\n", stderr);
			tar_usage();
			break;
		case 'I':
		case 'O':
			/*
			 * filename where the archive is stored
			 */
			if ((optarg[0] == '-') && (optarg[1]== '\0')) {
				/*
				 * treat a - as stdin
				 */
				arcname = NULL;
				break;
			}
			arcname = optarg;
			break;
		case 'L':
			/*
			 * follow symlinks
			 */
			Lflag = 1;
			flg |= CLF;
			break;
#ifdef notyet
		case 'M':
			arg = optarg;
			break;
		case 'R':
			arg = optarg;
			break;
#endif
		case 'S':
			/*
			 * swap halfwords after reading data
			 */
			cpio_swp_head = 1;
			break;
#ifdef notyet
		case 'V':
			break;
#endif
		case 'Z':
			/*
			 * use compress.  Non standard option.
			 */
			gzip_program = COMPRESS_CMD;
			break;
		case '6':
			/*
			 * process Version 6 cpio format
			 */
			frmt = &(fsub[F_BCPIO]);
		case OPT_FORCE_LOCAL:
			forcelocal = 1;
			break;
		default:
			cpio_usage();
			break;
		}
	}

	/*
	 * figure out the operation mode of cpio. check that we have not been
	 * given a bogus set of flags for the operation mode.
	 */
	if (ISLIST(flg)) {
		act = LIST;
		bflg = flg & BDLIST;
	} else if (ISEXTRACT(flg)) {
		act = EXTRACT;
		bflg = flg & BDEXTR;
	} else if (ISARCHIVE(flg)) {
		act = ARCHIVE;
		bflg = flg & BDARCH;
	} else if (ISAPPND(flg)) {
		act = APPND;
		bflg = flg & BDARCH;
	} else if (ISCOPY(flg)) {
		act = COPY;
		bflg = flg & BDCOPY;
	} else
		cpio_usage();
	if (bflg) {
		cpio_usage();
	}

	/*
	 * if we are writing (ARCHIVE) we use the default format if the user
	 * did not specify a format. when we write during an APPEND, we will
	 * adopt the format of the existing archive if none was supplied.
	 */
	if (!(flg & XF) && (act == ARCHIVE))
		frmt = &(fsub[F_BCPIO]);

	/*
	 * process the args as they are interpreted by the operation mode
	 */
	switch (act) {
	case LIST:
	case EXTRACT:
		for (; optind < argc; optind++)
			if (pat_add(argv[optind], 0) < 0)
				cpio_usage();
		break;
	case COPY:
		if (optind >= argc) {
			tty_warn(0, "Destination directory was not supplied");
			cpio_usage();
		}
		--argc;
		dirptr = argv[argc];
		/* FALLTHROUGH */
	case ARCHIVE:
	case APPND:
		if (argc != optind) {
			for (; optind < argc; optind++)
				if (ftree_add(argv[optind], 0) < 0)
					cpio_usage();
			break;
		}
		/*
		 * no read errors allowed on updates/append operation!
		 */
		maxflt = 0;
		while ((str = getline(stdin)) != NULL) {
			ftree_add(str, NULL);
		}
		if (getline_error) {
			tty_warn(1, "Problem while reading stdin");
			cpio_usage();
		}
		break;
	default:
		cpio_usage();
		break;
	}
}

/*
 * printflg()
 *	print out those invalid flag sets found to the user
 */

static void
printflg(unsigned int flg)
{
	int nxt;
	int pos = 0;

	(void)fprintf(stderr,"%s: Invalid combination of options:", argv0);
	while ((nxt = ffs(flg)) != 0) {
		flg = flg >> nxt;
		pos += nxt;
		(void)fprintf(stderr, " -%c", flgch[pos-1]);
	}
	(void)putc('\n', stderr);
}

/*
 * c_frmt()
 *	comparison routine used by bsearch to find the format specified
 *	by the user
 */

static int
c_frmt(const void *a, const void *b)
{
	return(strcmp(((FSUB *)a)->name, ((FSUB *)b)->name));
}

/*
 * opt_next()
 *	called by format specific options routines to get each format specific
 *	flag and value specified with -o
 * Return:
 *	pointer to next OPLIST entry or NULL (end of list).
 */

OPLIST *
opt_next(void)
{
	OPLIST *opt;

	if ((opt = ophead) != NULL)
		ophead = ophead->fow;
	return(opt);
}

/*
 * bad_opt()
 *	generic routine used to complain about a format specific options
 *	when the format does not support options.
 */

int
bad_opt(void)
{
	OPLIST *opt;

	if (ophead == NULL)
		return(0);
	/*
	 * print all we were given
	 */
	tty_warn(1,"These format options are not supported");
	while ((opt = opt_next()) != NULL)
		(void)fprintf(stderr, "\t%s = %s\n", opt->name, opt->value);
	pax_usage();
	return(0);
}

/*
 * opt_add()
 *	breaks the value supplied to -o into a option name and value. options
 *	are given to -o in the form -o name-value,name=value
 *	multiple -o may be specified.
 * Return:
 *	0 if format in name=value format, -1 if -o is passed junk
 */

int
opt_add(const char *str)
{
	OPLIST *opt;
	char *frpt;
	char *pt;
	char *endpt;
	char *dstr;

	if ((str == NULL) || (*str == '\0')) {
		tty_warn(0, "Invalid option name");
		return(-1);
	}
	if ((dstr = strdup(str)) == NULL) {
		tty_warn(0, "Unable to allocate space for option list");
		return(-1);
	}
	frpt = endpt = dstr;

	/*
	 * break into name and values pieces and stuff each one into a
	 * OPLIST structure. When we know the format, the format specific
	 * option function will go through this list
	 */
	while ((frpt != NULL) && (*frpt != '\0')) {
		if ((endpt = strchr(frpt, ',')) != NULL)
			*endpt = '\0';
		if ((pt = strchr(frpt, '=')) == NULL) {
			tty_warn(0, "Invalid options format");
			free(dstr);
			return(-1);
		}
		if ((opt = (OPLIST *)malloc(sizeof(OPLIST))) == NULL) {
			tty_warn(0, "Unable to allocate space for option list");
			free(dstr);
			return(-1);
		}
		*pt++ = '\0';
		opt->name = frpt;
		opt->value = pt;
		opt->fow = NULL;
		if (endpt != NULL)
			frpt = endpt + 1;
		else
			frpt = NULL;
		if (ophead == NULL) {
			optail = ophead = opt;
			continue;
		}
		optail->fow = opt;
		optail = opt;
	}
	return(0);
}

/*
 * str_offt()
 *	Convert an expression of the following forms to an off_t > 0.
 *	1) A positive decimal number.
 *	2) A positive decimal number followed by a b (mult by 512).
 *	3) A positive decimal number followed by a k (mult by 1024).
 *	4) A positive decimal number followed by a m (mult by 512).
 *	5) A positive decimal number followed by a w (mult by sizeof int)
 *	6) Two or more positive decimal numbers (with/without k,b or w).
 *	   separated by x (also * for backwards compatibility), specifying
 *	   the product of the indicated values.
 * Return:
 *	0 for an error, a positive value o.w.
 */

static off_t
str_offt(char *val)
{
	char *expr;
	off_t num, t;

	num = STRTOOFFT(val, &expr, 0);
	if ((num == OFFT_MAX) || (num <= 0) || (expr == val))
		return(0);

	switch(*expr) {
	case 'b':
		t = num;
		num *= 512;
		if (t > num)
			return(0);
		++expr;
		break;
	case 'k':
		t = num;
		num *= 1024;
		if (t > num)
			return(0);
		++expr;
		break;
	case 'm':
		t = num;
		num *= 1048576;
		if (t > num)
			return(0);
		++expr;
		break;
	case 'w':
		t = num;
		num *= sizeof(int);
		if (t > num)
			return(0);
		++expr;
		break;
	}

	switch(*expr) {
		case '\0':
			break;
		case '*':
		case 'x':
			t = num;
			num *= str_offt(expr + 1);
			if (t > num)
				return(0);
			break;
		default:
			return(0);
	}
	return(num);
}

char *
getline(FILE *f)
{
	char *name, *temp;
	size_t len;

	name = fgetln(f, &len);
	if (!name) {
		getline_error = ferror(f) ? GETLINE_FILE_CORRUPT : 0;
		return(0);
	}
	if (name[len-1] != '\n')
		len++;
	temp = malloc(len);
	if (!temp) {
		getline_error = GETLINE_OUT_OF_MEM;
		return(0);
	}
	memcpy(temp, name, len-1);
	temp[len-1] = 0;
	return(temp);
}

/*
 * no_op()
 *	for those option functions where the archive format has nothing to do.
 * Return:
 *	0
 */

static int
no_op(void)
{
	return(0);
}

/*
 * pax_usage()
 *	print the usage summary to the user
 */

void
pax_usage(void)
{
	fprintf(stderr,
"usage: pax [-cdnvzO] [-E limit] [-f archive] [-N dbdir] [-s replstr] ...\n"
"           [-U user] ... [-G group] ... [-T [from_date][,to_date]] ...\n"
"           [pattern ...]\n");
	fprintf(stderr,
"       pax -r [-cdiknuvzADOYZ] [-E limit] [-f archive] [-N dbdir]\n"
"           [-o options] ... [-p string] ... [-s replstr] ... [-U user] ...\n"
"           [-G group] ... [-T [from_date][,to_date]] ... [pattern ...]\n");
	fprintf(stderr,
"       pax -w [-dituvzAHLMOPX] [-b blocksize] [[-a] [-f archive]] [-x format]\n"
"           [-B bytes] [-N dbdir] [-o options] ... [-s replstr] ...\n"
"           [-U user] ... [-G group] ...\n"
"           [-T [from_date][,to_date][/[c][m]]] ... [file ...]\n");
	fprintf(stderr,
"       pax -r -w [-diklntuvzADHLMOPXYZ] [-N dbdir] [-p string] ...\n"
"           [-s replstr] ... [-U user] ... [-G group] ...\n"
"           [-T [from_date][,to_date][/[c][m]]] ... [file ...] directory\n");
	exit(1);
	/* NOTREACHED */
}

/*
 * tar_usage()
 *	print the usage summary to the user
 */

void
tar_usage(void)
{
	(void)fputs("usage: tar -{crtux}[-befhmopqvwzHLOPXZ014578] [archive] "
		    "[blocksize]\n"
		    "           [-C directory] [-T file] [-s replstr] "
		    "[file ...]\n", stderr);
	exit(1);
	/* NOTREACHED */
}

/*
 * cpio_usage()
 *	print the usage summary to the user
 */

void
cpio_usage(void)
{

#if 1
	(void)fputs(
	    "usage: cpio -i [-BcdfmrStuv] [ -C blksize ] [ -H header ]\n",
	    stderr);
	(void)fputs("  [ -I file ] [ pattern ... ]\n", stderr);
	(void)fputs("usage: cpio -o [-aABcLv] [ -C bufsize ] [ -H header ]\n",
	    stderr);
	(void)fputs("  [ -O file ]\n", stderr);
	(void)fputs("usage: cpio -p [ adlLmuv ] directory\n", stderr);
#else
	/* no E, M, R, V, b, k or s */
	(void)fputs("usage: cpio -i [-bBcdfkmrsStuvV] [ -C bufsize ]\n", stderr);
	(void)fputs("  [ -E file ] [ -H header ] [ -I file [ -M message ] ]\n",
	    stderr);
	(void)fputs("  [ -R id ] [ pattern ... ]\n", stderr);
	(void)fputs("usage: cpio -o [-aABcLvV] [ -C bufsize ] [ -H header ]\n",
	    stderr);
	(void)fputs("  [ -O file [ -M message ] ]\n", stderr);
	(void)fputs("usage: cpio -p [ adlLmuvV ] [ -R id ] directory\n", stderr);
#endif
	exit(1);
	/* NOTREACHED */
}
