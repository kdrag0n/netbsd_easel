/*	$NetBSD: dosfile.c,v 1.8 2003/08/18 15:47:41 dsl Exp $	 */

/*
 * Copyright (c) 1996
 *	Matthias Drochner.  All rights reserved.
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
 *	This product includes software developed for the NetBSD Project
 *	by Matthias Drochner.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * DOS filesystem for libsa
 * standalone - uses no device, works only with DOS running
 * needs lowlevel parts from dos_file.S
 */

#include <lib/libsa/stand.h>

#include "diskbuf.h"
#include "dosfile.h"

extern int dosopen __P((const char *));
extern void dosclose __P((int));
extern int dosread __P((int, char *, int));
extern int dosseek __P((int, int, int));

struct dosfile {
	int doshandle, off;
};

extern int doserrno;	/* in dos_file.S */

static int dos2errno __P((void));

static int
dos2errno()
{
	int err;

	switch (doserrno) {
	    case 1:
	    case 4:
	    case 12:
	    default:
		err = EIO;
	    case 2:
	    case 3:
		err = ENOENT;
	    case 5:
		err = EPERM;
	    case 6:
		err = EINVAL;
	}
	return (err);
}

int 
dos_open(path, f)
	const char           *path;
	struct open_file *f;
{
	struct dosfile *df;

	df = (struct dosfile *) alloc(sizeof(*df));
	if (!df)
		return (-1);

	df->off = 0;
	df->doshandle = dosopen(path);
	if (df->doshandle < 0) {
#ifdef DEBUG
		printf("DOS error %d\n", doserrno);
#endif
		free(df, sizeof(*df));
		return (dos2errno());
	}
	f->f_fsdata = (void *) df;
	return (0);
}

int 
dos_read(f, addr, size, resid)
	struct open_file *f;
	void           *addr;
	size_t         size;
	size_t         *resid;	/* out */
{
	struct dosfile *df;
	int             got;
	static int      tc = 0;

	df = (struct dosfile *) f->f_fsdata;

	if (!(tc++ % 4))
		twiddle();

	if ((unsigned long) addr >= 0x10000) {
		u_int           lsize = size;

		while (lsize > 0) {
			u_int           tsize;
			int             tgot;

			tsize = lsize;

			if (tsize > DISKBUFSIZE)
				tsize = DISKBUFSIZE;

			alloc_diskbuf(dos_read);

			tgot = dosread(df->doshandle, diskbufp, tsize);
			if (tgot < 0) {
#ifdef DEBUG
				printf("DOS error %d\n", doserrno);
#endif
				return (dos2errno());
			}
			memcpy(addr, diskbufp, tgot);

			(unsigned long)addr += tgot;
			lsize -= tgot;

			if (tgot != tsize)
				break;	/* EOF */
		}
		got = size - lsize;
	} else {
		got = dosread(df->doshandle, addr, size);

		if (got < 0) {
#ifdef DEBUG
			printf("DOS error %d\n", doserrno);
#endif
			return (dos2errno());
		}
	}

	df->off += got;
	size -= got;

	if (resid)
		*resid = size;
	return (0);
}

int 
dos_close(f)
	struct open_file *f;
{
	struct dosfile *df;
	df = (struct dosfile *) f->f_fsdata;

	dosclose(df->doshandle);

	if (df)
		free(df, sizeof(*df));
	return (0);
}

int 
dos_write(f, start, size, resid)
	struct open_file *f;
	void           *start;
	size_t          size;
	size_t         *resid;	/* out */
{
	return (EROFS);
}

int 
dos_stat(f, sb)
	struct open_file *f;
	struct stat    *sb;
{
	struct dosfile *df;
	df = (struct dosfile *) f->f_fsdata;

	sb->st_mode = 0444;
	sb->st_nlink = 1;
	sb->st_uid = 0;
	sb->st_gid = 0;
	sb->st_size = -1;
	return (0);
}

off_t 
dos_seek(f, offset, where)
	struct open_file *f;
	off_t           offset;
	int             where;
{
	struct dosfile *df;
	int             doswhence, res;
#ifdef DOS_CHECK
	int             checkoffs;
#endif
	df = (struct dosfile *) f->f_fsdata;

	switch (where) {
	case SEEK_SET:
		doswhence = 0;
#ifdef DOS_CHECK
		checkoffs = offset;	/* don't trust DOS */
#endif
		break;
	case SEEK_CUR:
		doswhence = 1;
#ifdef DOS_CHECK
		checkoffs = df->off + offset;
#endif
		break;
	case SEEK_END:
		doswhence = 2;
#ifdef DOS_CHECK
		checkoffs = -1;	/* we dont know len */
#endif
		break;
	default:
		errno = EOFFSET;
		return (-1);
	}
	res = dosseek(df->doshandle, offset, doswhence);
	if (res == -1) {
		errno = dos2errno();
		return (-1);
	}
#ifdef DOS_CHECK
	if ((checkoffs != -1) && (res != checkoffs)) {
		printf("dosfile: unexpected seek result (%d+%d(%d)=%d)\n",
		       df->off, offset, where, res);
		errno = EOFFSET;
		return (-1);
	}
#endif
	df->off = res;
	return (res);
}
