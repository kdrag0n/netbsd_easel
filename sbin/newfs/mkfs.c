/*	$NetBSD: mkfs.c,v 1.70 2003/05/02 03:26:11 atatat Exp $	*/

/*
 * Copyright (c) 2002 Networks Associates Technology, Inc.
 * All rights reserved.
 *
 * This software was developed for the FreeBSD Project by Marshall
 * Kirk McKusick and Network Associates Laboratories, the Security
 * Research Division of Network Associates, Inc. under DARPA/SPAWAR
 * contract N66001-01-C-8035 ("CBOSS"), as part of the DARPA CHATS
 * research program
 *
 * Copyright (c) 1980, 1989, 1993
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

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)mkfs.c	8.11 (Berkeley) 5/3/95";
#else
__RCSID("$NetBSD: mkfs.c,v 1.70 2003/05/02 03:26:11 atatat Exp $");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ufs/ufs/dinode.h>
#include <ufs/ufs/dir.h>
#include <ufs/ufs/ufs_bswap.h>
#include <ufs/ffs/fs.h>
#include <ufs/ffs/ffs_extern.h>
#include <sys/disklabel.h>

#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef STANDALONE
#include <stdio.h>
#endif

#include "extern.h"

union dinode {
	struct ufs1_dinode dp1;
	struct ufs2_dinode dp2;
};

static void initcg(int, const struct timeval *);
static int fsinit(const struct timeval *, mode_t, uid_t, gid_t);
static int makedir(struct direct *, int);
static daddr_t alloc(int, int);
static void iput(union dinode *, ino_t);
static void rdfs(daddr_t, int, void *);
static void wtfs(daddr_t, int, void *);
static int isblock(struct fs *, unsigned char *, int);
static void clrblock(struct fs *, unsigned char *, int);
static void setblock(struct fs *, unsigned char *, int);
static int ilog2(int);
#ifdef MFS
static void calc_memfree(void);
static void *mkfs_malloc(size_t size);
#endif

static int count_digits(int);

/*
 * make file system for cylinder-group style file systems
 */
#define	UMASK		0755
#define	POWEROF2(num)	(((num) & ((num) - 1)) == 0)

union {
	struct fs fs;
	char pad[SBLOCKSIZE];
} fsun;
#define	sblock	fsun.fs
struct	csum *fscs;

union {
	struct cg cg;
	char pad[MAXBSIZE];
} cgun;
#define	acg	cgun.cg

#define DIP(dp, field) \
	((sblock.fs_magic == FS_UFS1_MAGIC) ? \
	(dp)->dp1.di_##field : (dp)->dp2.di_##field)

char *iobuf;
int iobufsize;

char writebuf[MAXBSIZE];

int	fsi, fso;

void
mkfs(struct partition *pp, const char *fsys, int fi, int fo,
    mode_t mfsmode, uid_t mfsuid, gid_t mfsgid)
{
	int fragsperinode, optimalfpg, origdensity, minfpg, lastminfpg;
	int32_t cylno, i, csfrags;
	struct timeval tv;
	long long sizepb;
	char *writebuf2;		/* dynamic buffer */
	int nprintcols, printcolwidth;

#ifndef STANDALONE
	gettimeofday(&tv, NULL);
#endif
#ifdef MFS
	if (mfs) {
		calc_memfree();
		if (fssize * sectorsize > memleft)
			fssize = memleft / sectorsize;
		if ((membase = mkfs_malloc(fssize * sectorsize)) == 0)
			exit(12);
	}
#endif
	fsi = fi;
	fso = fo;
	if (Oflag == 0) {
		sblock.fs_old_inodefmt = FS_42INODEFMT;
		sblock.fs_maxsymlinklen = 0;
		sblock.fs_old_flags = 0;
	} else {
		sblock.fs_old_inodefmt = FS_44INODEFMT;
		sblock.fs_maxsymlinklen = (Oflag == 1 ? MAXSYMLINKLEN_UFS1 :
		    MAXSYMLINKLEN_UFS2);
		sblock.fs_old_flags = FS_FLAGS_UPDATED;
		sblock.fs_flags = 0;
	}
	/*
	 * Validate the given file system size.
	 * Verify that its last block can actually be accessed.
	 * Convert to file system fragment sized units.
	 */
	if (fssize <= 0) {
		printf("preposterous size %lld\n", (long long)fssize);
		exit(13);
	}
	wtfs(fssize - 1, sectorsize, (char *)&sblock);

	if (isappleufs) {
		struct appleufslabel appleufs;
		ffs_appleufs_set(&appleufs,appleufs_volname,tv.tv_sec);
		wtfs(APPLEUFS_LABEL_OFFSET/sectorsize,APPLEUFS_LABEL_SIZE,&appleufs);
	}

	/*
	 * collect and verify the filesystem density info
	 */
	sblock.fs_avgfilesize = avgfilesize;
	sblock.fs_avgfpdir = avgfpdir;
	if (sblock.fs_avgfilesize <= 0)
		printf("illegal expected average file size %d\n",
		    sblock.fs_avgfilesize), exit(14);
	if (sblock.fs_avgfpdir <= 0)
		printf("illegal expected number of files per directory %d\n",
		    sblock.fs_avgfpdir), exit(15);
	/*
	 * collect and verify the block and fragment sizes
	 */
	sblock.fs_bsize = bsize;
	sblock.fs_fsize = fsize;
	if (!POWEROF2(sblock.fs_bsize)) {
		printf("block size must be a power of 2, not %d\n",
		    sblock.fs_bsize);
		exit(16);
	}
	if (!POWEROF2(sblock.fs_fsize)) {
		printf("fragment size must be a power of 2, not %d\n",
		    sblock.fs_fsize);
		exit(17);
	}
	if (sblock.fs_fsize < sectorsize) {
		printf("fragment size %d is too small, minimum is %d\n",
		    sblock.fs_fsize, sectorsize);
		exit(18);
	}
	if (sblock.fs_bsize < MINBSIZE) {
		printf("block size %d is too small, minimum is %d\n",
		    sblock.fs_bsize, MINBSIZE);
		exit(19);
	}
	if (sblock.fs_bsize > MAXBSIZE) {
		printf("block size %d is too large, maximum is %d\n",
		    sblock.fs_bsize, MAXBSIZE);
		exit(19);
	}
	if (sblock.fs_bsize < sblock.fs_fsize) {
		printf("block size (%d) cannot be smaller than fragment size (%d)\n",
		    sblock.fs_bsize, sblock.fs_fsize);
		exit(20);
	}

	if (maxbsize < bsize || !POWEROF2(maxbsize)) {
		sblock.fs_maxbsize = sblock.fs_bsize;
	} else if (sblock.fs_maxbsize > FS_MAXCONTIG * sblock.fs_bsize) {
		sblock.fs_maxbsize = FS_MAXCONTIG * sblock.fs_bsize;
	} else {
		sblock.fs_maxbsize = maxbsize;
	}
	sblock.fs_maxcontig = maxcontig;
	if (sblock.fs_maxcontig < sblock.fs_maxbsize / sblock.fs_bsize) {
		sblock.fs_maxcontig = sblock.fs_maxbsize / sblock.fs_bsize;
		printf("Maxcontig raised to %d\n", sblock.fs_maxbsize);
	}
	if (sblock.fs_maxcontig > 1)
		sblock.fs_contigsumsize = MIN(sblock.fs_maxcontig,FS_MAXCONTIG);

	sblock.fs_bmask = ~(sblock.fs_bsize - 1);
	sblock.fs_fmask = ~(sblock.fs_fsize - 1);
	sblock.fs_qbmask = ~sblock.fs_bmask;
	sblock.fs_qfmask = ~sblock.fs_fmask;
	for (sblock.fs_bshift = 0, i = sblock.fs_bsize; i > 1; i >>= 1)
		sblock.fs_bshift++;
	for (sblock.fs_fshift = 0, i = sblock.fs_fsize; i > 1; i >>= 1)
		sblock.fs_fshift++;
	sblock.fs_frag = numfrags(&sblock, sblock.fs_bsize);
	for (sblock.fs_fragshift = 0, i = sblock.fs_frag; i > 1; i >>= 1)
		sblock.fs_fragshift++;
	if (sblock.fs_frag > MAXFRAG) {
		printf("fragment size %d is too small, "
			"minimum with block size %d is %d\n",
		    sblock.fs_fsize, sblock.fs_bsize,
		    sblock.fs_bsize / MAXFRAG);
		exit(21);
	}
	sblock.fs_fsbtodb = ilog2(sblock.fs_fsize / sectorsize);
	sblock.fs_size = fssize = dbtofsb(&sblock, fssize);
	if (Oflag <= 1) {
		sblock.fs_magic = FS_UFS1_MAGIC;
		sblock.fs_sblockloc = SBLOCK_UFS1;
		sblock.fs_nindir = sblock.fs_bsize / sizeof(int32_t);
		sblock.fs_inopb = sblock.fs_bsize / sizeof(struct ufs1_dinode);
		sblock.fs_maxsymlinklen = ((NDADDR + NIADDR) *
		    sizeof (int32_t));
		sblock.fs_old_inodefmt = FS_44INODEFMT;
		sblock.fs_old_cgoffset = 0;
		sblock.fs_old_cgmask = 0xffffffff;
		sblock.fs_old_size = sblock.fs_size;
		sblock.fs_old_rotdelay = 0;
		sblock.fs_old_rps = 60;
		sblock.fs_old_nspf = sblock.fs_fsize / sectorsize;
		sblock.fs_old_cpg = 1;
		sblock.fs_old_interleave = 1;
		sblock.fs_old_trackskew = 0;
		sblock.fs_old_cpc = 0;
		sblock.fs_old_postblformat = 1;
		sblock.fs_old_nrpos = 1;
	} else {
		sblock.fs_magic = FS_UFS2_MAGIC;
		sblock.fs_sblockloc = SBLOCK_UFS2;
		sblock.fs_nindir = sblock.fs_bsize / sizeof(int64_t);
		sblock.fs_inopb = sblock.fs_bsize / sizeof(struct ufs2_dinode);
		sblock.fs_maxsymlinklen = ((NDADDR + NIADDR) *
		    sizeof (int64_t));
	}

	sblock.fs_sblkno =
	    roundup(howmany(sblock.fs_sblockloc + SBLOCKSIZE, sblock.fs_fsize),
		sblock.fs_frag);
	sblock.fs_cblkno = (daddr_t)(sblock.fs_sblkno +
	    roundup(howmany(SBLOCKSIZE, sblock.fs_fsize), sblock.fs_frag));
	sblock.fs_iblkno = sblock.fs_cblkno + sblock.fs_frag;
	sblock.fs_maxfilesize = sblock.fs_bsize * NDADDR - 1;
	for (sizepb = sblock.fs_bsize, i = 0; i < NIADDR; i++) {
		sizepb *= NINDIR(&sblock);
		sblock.fs_maxfilesize += sizepb;
	}

	/*
	 * Calculate the number of blocks to put into each cylinder group.
	 *
	 * This algorithm selects the number of blocks per cylinder
	 * group. The first goal is to have at least enough data blocks
	 * in each cylinder group to meet the density requirement. Once
	 * this goal is achieved we try to expand to have at least
	 * MINCYLGRPS cylinder groups. Once this goal is achieved, we
	 * pack as many blocks into each cylinder group map as will fit.
	 *
	 * We start by calculating the smallest number of blocks that we
	 * can put into each cylinder group. If this is too big, we reduce
	 * the density until it fits.
	 */
	origdensity = density;
	for (;;) {
		fragsperinode = MAX(numfrags(&sblock, density), 1);
		minfpg = fragsperinode * INOPB(&sblock);
		if (minfpg > sblock.fs_size)
			minfpg = sblock.fs_size;
		sblock.fs_ipg = INOPB(&sblock);
		sblock.fs_fpg = roundup(sblock.fs_iblkno +
		    sblock.fs_ipg / INOPF(&sblock), sblock.fs_frag);
		if (sblock.fs_fpg < minfpg)
			sblock.fs_fpg = minfpg;
		sblock.fs_ipg = roundup(howmany(sblock.fs_fpg, fragsperinode),
		    INOPB(&sblock));
		sblock.fs_fpg = roundup(sblock.fs_iblkno +
		    sblock.fs_ipg / INOPF(&sblock), sblock.fs_frag);
		if (sblock.fs_fpg < minfpg)
			sblock.fs_fpg = minfpg;
		sblock.fs_ipg = roundup(howmany(sblock.fs_fpg, fragsperinode),
		    INOPB(&sblock));
		if (CGSIZE(&sblock) < (unsigned long)sblock.fs_bsize)
			break;
		density -= sblock.fs_fsize;
	}
	if (density != origdensity)
		printf("density reduced from %d to %d\n", origdensity, density);
	/*
	 * Start packing more blocks into the cylinder group until
	 * it cannot grow any larger, the number of cylinder groups
	 * drops below MINCYLGRPS, or we reach the size requested.
	 */
	for ( ; sblock.fs_fpg < maxblkspercg; sblock.fs_fpg += sblock.fs_frag) {
		sblock.fs_ipg = roundup(howmany(sblock.fs_fpg, fragsperinode),
		    INOPB(&sblock));
		if (sblock.fs_size / sblock.fs_fpg < MINCYLGRPS)
			break;
		if (CGSIZE(&sblock) < (unsigned long)sblock.fs_bsize)
			continue;
		if (CGSIZE(&sblock) == (unsigned long)sblock.fs_bsize)
			break;
		sblock.fs_fpg -= sblock.fs_frag;
		sblock.fs_ipg = roundup(howmany(sblock.fs_fpg, fragsperinode),
		    INOPB(&sblock));
		break;
	}
	/*
	 * Check to be sure that the last cylinder group has enough blocks
	 * to be viable. If it is too small, reduce the number of blocks
	 * per cylinder group which will have the effect of moving more
	 * blocks into the last cylinder group.
	 */
	optimalfpg = sblock.fs_fpg;
	for (;;) {
		sblock.fs_ncg = howmany(sblock.fs_size, sblock.fs_fpg);
		lastminfpg = roundup(sblock.fs_iblkno +
		    sblock.fs_ipg / INOPF(&sblock), sblock.fs_frag);
		if (sblock.fs_size < lastminfpg) {
			printf("Filesystem size %lld < minimum size of %d\n",
			    (long long)sblock.fs_size, lastminfpg);
			exit(28);
		}
		if (sblock.fs_size % sblock.fs_fpg >= lastminfpg ||
		    sblock.fs_size % sblock.fs_fpg == 0)
			break;
		sblock.fs_fpg -= sblock.fs_frag;
		sblock.fs_ipg = roundup(howmany(sblock.fs_fpg, fragsperinode),
		    INOPB(&sblock));
	}
	if (optimalfpg != sblock.fs_fpg)
		printf("Reduced frags per cylinder group from %d to %d %s\n",
		   optimalfpg, sblock.fs_fpg, "to enlarge last cyl group");
	sblock.fs_cgsize = fragroundup(&sblock, CGSIZE(&sblock));
	sblock.fs_dblkno = sblock.fs_iblkno + sblock.fs_ipg / INOPF(&sblock);
	if (Oflag <= 1) {
		sblock.fs_old_spc = sblock.fs_fpg * sblock.fs_old_nspf;
		sblock.fs_old_nsect = sblock.fs_old_spc;
		sblock.fs_old_npsect = sblock.fs_old_spc;
		sblock.fs_old_ncyl = sblock.fs_ncg;
	}

	/*
	 * fill in remaining fields of the super block
	 */
	sblock.fs_csaddr = cgdmin(&sblock, 0);
	sblock.fs_cssize =
	    fragroundup(&sblock, sblock.fs_ncg * sizeof(struct csum));
	fscs = (struct csum *)calloc(1, sblock.fs_cssize);
	if (fscs == NULL)
		exit(39);
	sblock.fs_sbsize = fragroundup(&sblock, sizeof(struct fs));
	if (sblock.fs_sbsize > SBLOCKSIZE)
		sblock.fs_sbsize = SBLOCKSIZE;
	sblock.fs_minfree = minfree;
	sblock.fs_maxcontig = maxcontig;
	sblock.fs_maxbpg = maxbpg;
	sblock.fs_optim = opt;
	sblock.fs_cgrotor = 0;
	sblock.fs_pendingblocks = 0;
	sblock.fs_pendinginodes = 0;
	sblock.fs_cstotal.cs_ndir = 0;
	sblock.fs_cstotal.cs_nbfree = 0;
	sblock.fs_cstotal.cs_nifree = 0;
	sblock.fs_cstotal.cs_nffree = 0;
	sblock.fs_fmod = 0;
	sblock.fs_ronly = 0;
	sblock.fs_state = 0;
	sblock.fs_clean = FS_ISCLEAN;
	sblock.fs_ronly = 0;
	sblock.fs_id[0] = (long)tv.tv_sec;	/* XXXfvdl huh? */
	sblock.fs_id[1] = random();
	sblock.fs_fsmnt[0] = '\0';
	csfrags = howmany(sblock.fs_cssize, sblock.fs_fsize);
	sblock.fs_dsize = sblock.fs_size - sblock.fs_sblkno -
	    sblock.fs_ncg * (sblock.fs_dblkno - sblock.fs_sblkno);
	sblock.fs_cstotal.cs_nbfree =
	    fragstoblks(&sblock, sblock.fs_dsize) -
	    howmany(csfrags, sblock.fs_frag);
	sblock.fs_cstotal.cs_nffree =
	    fragnum(&sblock, sblock.fs_size) +
	    (fragnum(&sblock, csfrags) > 0 ?
	    sblock.fs_frag - fragnum(&sblock, csfrags) : 0);
	sblock.fs_cstotal.cs_nifree = sblock.fs_ncg * sblock.fs_ipg - ROOTINO;
	sblock.fs_cstotal.cs_ndir = 0;
	sblock.fs_dsize -= csfrags;
	sblock.fs_time = tv.tv_sec;
	if (Oflag <= 1) {
		sblock.fs_old_time = tv.tv_sec;
		sblock.fs_old_dsize = sblock.fs_dsize;
		sblock.fs_old_csaddr = sblock.fs_csaddr;
		sblock.fs_old_cstotal.cs_ndir = sblock.fs_cstotal.cs_ndir;
		sblock.fs_old_cstotal.cs_nbfree = sblock.fs_cstotal.cs_nbfree;
		sblock.fs_old_cstotal.cs_nifree = sblock.fs_cstotal.cs_nifree;
		sblock.fs_old_cstotal.cs_nffree = sblock.fs_cstotal.cs_nffree;
	}
	/*
	 * Dump out summary information about file system.
	 */
	if (!mfs) {
#define	B2MBFACTOR (1 / (1024.0 * 1024.0))
		printf("%s: %.1fMB (%lld sectors) block size %d, "
		       "fragment size %d\n",
		    fsys, (float)sblock.fs_size * sblock.fs_fsize * B2MBFACTOR,
		    (long long)fsbtodb(&sblock, sblock.fs_size),
		    sblock.fs_bsize, sblock.fs_fsize);
		printf("\tusing %d cylinder groups of %.2fMB, %d blks, "
		       "%d inodes.\n",
		    sblock.fs_ncg,
		    (float)sblock.fs_fpg * sblock.fs_fsize * B2MBFACTOR,
		    sblock.fs_fpg / sblock.fs_frag, sblock.fs_ipg);
#undef B2MBFACTOR
	}
	/*
	 * Now determine how wide each column will be, and calculate how
	 * many columns will fit in a 76 char line. 76 is the width of the
	 * subwindows in sysinst.
	 */
	printcolwidth = count_digits(
			fsbtodb(&sblock, cgsblock(&sblock, sblock.fs_ncg -1)));
	nprintcols = 76 / (printcolwidth + 2);

	/*
	 * allocate space for superblock, cylinder group map, and
	 * two sets of inode blocks.
	 */
	if (sblock.fs_bsize < SBLOCKSIZE)
		iobufsize = SBLOCKSIZE + 3 * sblock.fs_bsize;
	else
		iobufsize = 4 * sblock.fs_bsize;
	if ((iobuf = malloc(iobufsize)) == 0) {
		printf("Cannot allocate I/O buffer\n");
		exit(38);
	}
	memset(iobuf, 0, iobufsize);
	/*
	 * Make a copy of the superblock into the buffer that we will be
	 * writing out in each cylinder group.
	 */
	memcpy(writebuf, &sblock, sbsize);
	if (needswap)
		ffs_sb_swap(&sblock, (struct fs*)writebuf);
	memcpy(iobuf, writebuf, SBLOCKSIZE);

	if (!mfs)
		printf("super-block backups (for fsck -b #) at:");
	for (cylno = 0; cylno < sblock.fs_ncg; cylno++) {
		initcg(cylno, &tv);
		if (mfs)
			continue;
		if (cylno % nprintcols == 0)
			printf("\n");
		printf(" %*lld,", printcolwidth,
			(long long)fsbtodb(&sblock, cgsblock(&sblock, cylno)));
		fflush(stdout);
	}
	if (!mfs)
		printf("\n");
	if (Nflag && !mfs)
		exit(0);

	/*
	 * Now construct the initial file system,
	 * then write out the super-block.
	 */
	if (fsinit(&tv, mfsmode, mfsuid, mfsgid) == 0 && mfs)
		errx(1, "Error making filesystem");
	sblock.fs_time = tv.tv_sec;
	if (Oflag <= 1) {
		sblock.fs_old_cstotal.cs_ndir = sblock.fs_cstotal.cs_ndir;
		sblock.fs_old_cstotal.cs_nbfree = sblock.fs_cstotal.cs_nbfree;
		sblock.fs_old_cstotal.cs_nifree = sblock.fs_cstotal.cs_nifree;
		sblock.fs_old_cstotal.cs_nffree = sblock.fs_cstotal.cs_nffree;
	}
        memcpy(writebuf, &sblock, sbsize);
	if (needswap)
		ffs_sb_swap(&sblock, (struct fs*)writebuf);
        wtfs(sblock.fs_sblockloc / sectorsize, sbsize, writebuf);

	/*
	 * if we need to swap, create a buffer for the cylinder summaries
	 * to get swapped to.
	 */
	if (needswap) {
		if ((writebuf2 = malloc(sblock.fs_cssize)) == NULL)
			exit(12);
		ffs_csum_swap(fscs, (struct csum*)writebuf2, sblock.fs_cssize);
	} else
		writebuf2 = (char *)fscs;

	for (i = 0; i < sblock.fs_cssize; i += sblock.fs_bsize)
		wtfs(fsbtodb(&sblock, sblock.fs_csaddr + numfrags(&sblock, i)),
			sblock.fs_cssize - i < sblock.fs_bsize ?
			    sblock.fs_cssize - i : sblock.fs_bsize,
			((char *)writebuf2) + i);
	if (writebuf2 != (char *)fscs)
		free(writebuf2);

	/*
	 * Update information about this partion in pack
	 * label, to that it may be updated on disk.
	 */
	if (isappleufs)
		pp->p_fstype = FS_APPLEUFS;
	else
		pp->p_fstype = FS_BSDFFS;
	pp->p_fsize = sblock.fs_fsize;
	pp->p_frag = sblock.fs_frag;
	pp->p_cpg = sblock.fs_fpg;
}

/*
 * Initialize a cylinder group.
 */
void
initcg(int cylno, const struct timeval *tv)
{
	daddr_t cbase, dmax;
	int32_t i, j, d, dlower, dupper, blkno;
	struct csum *cs;
	struct ufs1_dinode *dp1;
	struct ufs2_dinode *dp2;
	int start;

	/*
	 * Determine block bounds for cylinder group.
	 * Allow space for super block summary information in first
	 * cylinder group.
	 */
	cbase = cgbase(&sblock, cylno);
	dmax = cbase + sblock.fs_fpg;
	if (dmax > sblock.fs_size)
		dmax = sblock.fs_size;
	dlower = cgsblock(&sblock, cylno) - cbase;
	dupper = cgdmin(&sblock, cylno) - cbase;
	if (cylno == 0)
		dupper += howmany(sblock.fs_cssize, sblock.fs_fsize);
	cs = fscs + cylno;
	memset(&acg, 0, sblock.fs_cgsize);
	acg.cg_time = tv->tv_sec;
	acg.cg_magic = CG_MAGIC;
	acg.cg_cgx = cylno;
	acg.cg_niblk = sblock.fs_ipg;
	acg.cg_initediblk = sblock.fs_ipg < 2 * INOPB(&sblock) ?
	    sblock.fs_ipg : 2 * INOPB(&sblock);
	acg.cg_ndblk = dmax - cbase;
	if (sblock.fs_contigsumsize > 0)
		acg.cg_nclusterblks = acg.cg_ndblk >> sblock.fs_fragshift;
	start = &acg.cg_space[0] - (u_char *)(&acg.cg_firstfield);
	if (Oflag == 2) {
		acg.cg_iusedoff = start;
	} else {
		acg.cg_old_ncyl = sblock.fs_old_cpg;
		acg.cg_old_time = acg.cg_time;
		acg.cg_time = 0;
		acg.cg_old_niblk = acg.cg_niblk;
		acg.cg_niblk = 0;
		acg.cg_initediblk = 0;
		acg.cg_old_btotoff = start;
		acg.cg_old_boff = acg.cg_old_btotoff +
		    sblock.fs_old_cpg * sizeof(int32_t);
		acg.cg_iusedoff = acg.cg_old_boff +
		    sblock.fs_old_cpg * sizeof(u_int16_t);
	}
	acg.cg_freeoff = acg.cg_iusedoff + howmany(sblock.fs_ipg, CHAR_BIT);
	if (sblock.fs_contigsumsize <= 0) {
		acg.cg_nextfreeoff = acg.cg_freeoff +
		   howmany(sblock.fs_fpg, CHAR_BIT);
	} else {
		acg.cg_clustersumoff = acg.cg_freeoff +
		    howmany(sblock.fs_fpg, CHAR_BIT) - sizeof(int32_t);
		if (isappleufs) {
			/* Apple PR2216969 gives rationale for this change.
			 * I believe they were mistaken, but we need to
			 * duplicate it for compatibility.  -- dbj@NetBSD.org
			 */
			acg.cg_clustersumoff += sizeof(int32_t);
		}
		acg.cg_clustersumoff =
		    roundup(acg.cg_clustersumoff, sizeof(int32_t));
		acg.cg_clusteroff = acg.cg_clustersumoff +
		    (sblock.fs_contigsumsize + 1) * sizeof(int32_t);
		acg.cg_nextfreeoff = acg.cg_clusteroff +
		    howmany(fragstoblks(&sblock, sblock.fs_fpg), CHAR_BIT);
	}
	if (acg.cg_nextfreeoff > sblock.fs_cgsize) {
		printf("Panic: cylinder group too big\n");
		exit(37);
	}
	acg.cg_cs.cs_nifree += sblock.fs_ipg;
	if (cylno == 0)
		for (i = 0; i < ROOTINO; i++) {
			setbit(cg_inosused(&acg, 0), i);
			acg.cg_cs.cs_nifree--;
		}
	if (cylno > 0) {
		/*
		 * In cylno 0, beginning space is reserved
		 * for boot and super blocks.
		 */
		for (d = 0, blkno = 0; d < dlower;) {
			setblock(&sblock, cg_blksfree(&acg, 0), blkno);
			if (sblock.fs_contigsumsize > 0)
				setbit(cg_clustersfree(&acg, 0), blkno);
			acg.cg_cs.cs_nbfree++;
			d += sblock.fs_frag;
			blkno++;
		}
	}
	if ((i = (dupper & (sblock.fs_frag - 1))) != 0) {
		acg.cg_frsum[sblock.fs_frag - i]++;
		for (d = dupper + sblock.fs_frag - i; dupper < d; dupper++) {
			setbit(cg_blksfree(&acg, 0), dupper);
			acg.cg_cs.cs_nffree++;
		}
	}
	for (d = dupper, blkno = dupper >> sblock.fs_fragshift;
	     d + sblock.fs_frag <= acg.cg_ndblk; ) {
		setblock(&sblock, cg_blksfree(&acg, 0), blkno);
		if (sblock.fs_contigsumsize > 0)
			setbit(cg_clustersfree(&acg, 0), blkno);
		acg.cg_cs.cs_nbfree++;
		d += sblock.fs_frag;
		blkno++;
	}
	if (d < acg.cg_ndblk) {
		acg.cg_frsum[acg.cg_ndblk - d]++;
		for (; d < acg.cg_ndblk; d++) {
			setbit(cg_blksfree(&acg, 0), d);
			acg.cg_cs.cs_nffree++;
		}
	}
	if (sblock.fs_contigsumsize > 0) {
		int32_t *sump = cg_clustersum(&acg, 0);
		u_char *mapp = cg_clustersfree(&acg, 0);
		int map = *mapp++;
		int bit = 1;
		int run = 0;

		for (i = 0; i < acg.cg_nclusterblks; i++) {
			if ((map & bit) != 0) {
				run++;
			} else if (run != 0) {
				if (run > sblock.fs_contigsumsize)
					run = sblock.fs_contigsumsize;
				sump[run]++;
				run = 0;
			}
			if ((i & (CHAR_BIT - 1)) != (CHAR_BIT - 1)) {
				bit <<= 1;
			} else {
				map = *mapp++;
				bit = 1;
			}
		}
		if (run != 0) {
			if (run > sblock.fs_contigsumsize)
				run = sblock.fs_contigsumsize;
			sump[run]++;
		}
	}
	*cs = acg.cg_cs;
	/*
	 * Write out the duplicate super block, the cylinder group map
	 * and two blocks worth of inodes in a single write.
	 */
	start = sblock.fs_bsize > SBLOCKSIZE ? sblock.fs_bsize : SBLOCKSIZE;
	memcpy(&iobuf[start], &acg, sblock.fs_cgsize);
	if (needswap)
		ffs_cg_swap(&acg, (struct cg*)&iobuf[start], &sblock);
	start += sblock.fs_bsize;
	dp1 = (struct ufs1_dinode *)(&iobuf[start]);
	dp2 = (struct ufs2_dinode *)(&iobuf[start]);
	for (i = 0; i < acg.cg_initediblk; i++) {
		if (sblock.fs_magic == FS_UFS1_MAGIC) {
			/* No need to swap, it'll stay random */
			dp1->di_gen = random();
			dp1++;
		} else {
			dp2->di_gen = random();
			dp2++;
		}
	}
	wtfs(fsbtodb(&sblock, cgsblock(&sblock, cylno)), iobufsize, iobuf);
	/*
	 * For the old file system, we have to initialize all the inodes.
	 */
	if (Oflag <= 1) {
		for (i = 2 * sblock.fs_frag;
		     i < sblock.fs_ipg / INOPF(&sblock);
		     i += sblock.fs_frag) {
			dp1 = (struct ufs1_dinode *)(&iobuf[start]);
			for (j = 0; j < INOPB(&sblock); j++) {
				dp1->di_gen = random();
				dp1++;
			}
			wtfs(fsbtodb(&sblock, cgimin(&sblock, cylno) + i),
			    sblock.fs_bsize, &iobuf[start]);
		}
	}
}

/*
 * initialize the file system
 */
union dinode node;

#ifdef LOSTDIR
#define	PREDEFDIR 3
#else
#define	PREDEFDIR 2
#endif

struct direct root_dir[] = {
	{ ROOTINO, sizeof(struct direct), DT_DIR, 1, "." },
	{ ROOTINO, sizeof(struct direct), DT_DIR, 2, ".." },
#ifdef LOSTDIR
	{ LOSTFOUNDINO, sizeof(struct direct), DT_DIR, 10, "lost+found" },
#endif
};
struct odirect {
	u_int32_t d_ino;
	u_int16_t d_reclen;
	u_int16_t d_namlen;
	u_char	d_name[MAXNAMLEN + 1];
} oroot_dir[] = {
	{ ROOTINO, sizeof(struct direct), 1, "." },
	{ ROOTINO, sizeof(struct direct), 2, ".." },
#ifdef LOSTDIR
	{ LOSTFOUNDINO, sizeof(struct direct), 10, "lost+found" },
#endif
};
#ifdef LOSTDIR
struct direct lost_found_dir[] = {
	{ LOSTFOUNDINO, sizeof(struct direct), DT_DIR, 1, "." },
	{ ROOTINO, sizeof(struct direct), DT_DIR, 2, ".." },
	{ 0, DIRBLKSIZ, 0, 0, 0 },
};
struct odirect olost_found_dir[] = {
	{ LOSTFOUNDINO, sizeof(struct direct), 1, "." },
	{ ROOTINO, sizeof(struct direct), 2, ".." },
	{ 0, DIRBLKSIZ, 0, 0 },
};
#endif
char buf[MAXBSIZE];
static void copy_dir(struct direct *, struct direct *);

int
fsinit(const struct timeval *tv, mode_t mfsmode, uid_t mfsuid, gid_t mfsgid)
{
#ifdef LOSTDIR
	int i;
	int dirblksiz = DIRBLKSIZ;
	if (isappleufs)
		dirblksiz = APPLEUFS_DIRBLKSIZ;
#endif

	/*
	 * initialize the node
	 */
	memset(&node, 0, sizeof(node));

#ifdef LOSTDIR
	/*
	 * create the lost+found directory
	 */
	if (Oflag == 0) {
		(void)makedir((struct direct *)olost_found_dir, 2);
		for (i = dirblksiz; i < sblock.fs_bsize; i += dirblksiz)
			copy_dir((struct direct*)&olost_found_dir[2],
				(struct direct*)&buf[i]);
	} else {
		(void)makedir(lost_found_dir, 2);
		for (i = dirblksiz; i < sblock.fs_bsize; i += dirblksiz)
			copy_dir(&lost_found_dir[2], (struct direct*)&buf[i]);
	}
	if (sblock.fs_magic == FS_UFS1_MAGIC) {
		node.dp1.di_atime = tv->tv_sec;
		node.dp1.di_atimensec = tv->tv_usec * 1000;
		node.dp1.di_mtime = tv->tv_sec;
		node.dp1.di_mtimensec = tv->tv_usec * 1000;
		node.dp1.di_ctime = tv->tv_sec;
		node.dp1.di_ctimensec = tv->tv_usec * 1000;
		node.dp1.di_mode = IFDIR | UMASK;
		node.dp1.di_nlink = 2;
		node.dp1.di_size = sblock.fs_bsize;
		node.dp1.di_db[0] = alloc(node.dp1.di_size, node.dp1.di_mode);
		if (node.dp1.di_db[0] == 0)
			return (0);
		node.dp1.di_blocks = btodb(fragroundup(&sblock,
		    node.dp1.di_size));
		node.dp1.di_uid = geteuid();
		node.dp1.di_gid = getegid();
		wtfs(fsbtodb(&sblock, node.dp1.di_db[0]), node.dp1.di_size,
		    buf);
	} else {
		node.dp2.di_atime = tv->tv_sec;
		node.dp2.di_atimensec = tv->tv_usec * 1000;
		node.dp2.di_mtime = tv->tv_sec;
		node.dp2.di_mtimensec = tv->tv_usec * 1000;
		node.dp2.di_ctime = tv->tv_sec;
		node.dp2.di_ctimensec = tv->tv_usec * 1000;
		node.dp2.di_birthtime = tv->tv_sec;
		node.dp2.di_birthnsec = tv->tv_usec * 1000;
		node.dp2.di_mode = IFDIR | UMASK;
		node.dp2.di_nlink = 2;
		node.dp2.di_size = sblock.fs_bsize;
		node.dp2.di_db[0] = alloc(node.dp2.di_size, node.dp2.di_mode);
		if (node.dp2.di_db[0] == 0)
			return (0);
		node.dp2.di_blocks = btodb(fragroundup(&sblock,
		    node.dp2.di_size));
		node.dp2.di_uid = geteuid();
		node.dp2.di_gid = getegid();
		wtfs(fsbtodb(&sblock, node.dp2.di_db[0]), node.dp2.di_size,
		    buf);
	}
	iput(&node, LOSTFOUNDINO);
#endif
	/*
	 * create the root directory
	 */
	if (Oflag <= 1) {
		if (mfs) {
			node.dp1.di_mode = IFDIR | mfsmode;
			node.dp1.di_uid = mfsuid;
			node.dp1.di_gid = mfsgid;
		} else {
			node.dp1.di_mode = IFDIR | UMASK;
			node.dp1.di_uid = geteuid();
			node.dp1.di_gid = getegid();
		}
		node.dp1.di_nlink = PREDEFDIR;
		if (Oflag == 0)
			node.dp1.di_size = makedir((struct direct *)oroot_dir,
			    PREDEFDIR);
		else
			node.dp1.di_size = makedir(root_dir, PREDEFDIR);
		node.dp1.di_db[0] = alloc(sblock.fs_fsize, node.dp1.di_mode);
		if (node.dp1.di_db[0] == 0)
			return (0);
		node.dp1.di_blocks = btodb(fragroundup(&sblock,
		    node.dp1.di_size));
		wtfs(fsbtodb(&sblock, node.dp1.di_db[0]), sblock.fs_fsize, buf);
	} else {
		if (mfs) {
			node.dp2.di_mode = IFDIR | mfsmode;
			node.dp2.di_uid = mfsuid;
			node.dp2.di_gid = mfsgid;
		} else {
			node.dp2.di_mode = IFDIR | UMASK;
			node.dp2.di_uid = geteuid();
			node.dp2.di_gid = getegid();
		}
		node.dp2.di_atime = tv->tv_sec;
		node.dp2.di_atimensec = tv->tv_usec * 1000;
		node.dp2.di_mtime = tv->tv_sec;
		node.dp2.di_mtimensec = tv->tv_usec * 1000;
		node.dp2.di_ctime = tv->tv_sec;
		node.dp2.di_ctimensec = tv->tv_usec * 1000;
		node.dp2.di_birthtime = tv->tv_sec;
		node.dp2.di_birthnsec = tv->tv_usec * 1000;
		node.dp2.di_nlink = PREDEFDIR;
		node.dp2.di_size = makedir(root_dir, PREDEFDIR);
		node.dp2.di_db[0] = alloc(sblock.fs_fsize, node.dp2.di_mode);
		if (node.dp2.di_db[0] == 0)
			return (0);
		node.dp2.di_blocks = btodb(fragroundup(&sblock,
		    node.dp2.di_size));
		wtfs(fsbtodb(&sblock, node.dp2.di_db[0]), sblock.fs_fsize, buf);
	}
	iput(&node, ROOTINO);
	return (1);
}

/*
 * construct a set of directory entries in "buf".
 * return size of directory.
 */
int
makedir(struct direct *protodir, int entries)
{
	char *cp;
	int i, spcleft;
	int dirblksiz = DIRBLKSIZ;
	if (isappleufs)
		dirblksiz = APPLEUFS_DIRBLKSIZ;

	memset(buf, 0, DIRBLKSIZ);
	spcleft = dirblksiz;
	for (cp = buf, i = 0; i < entries - 1; i++) {
		protodir[i].d_reclen = DIRSIZ(Oflag == 0, &protodir[i], 0);
		copy_dir(&protodir[i], (struct direct*)cp);
		cp += protodir[i].d_reclen;
		spcleft -= protodir[i].d_reclen;
	}
	protodir[i].d_reclen = spcleft;
	copy_dir(&protodir[i], (struct direct*)cp);
	return (dirblksiz);
}

/*
 * allocate a block or frag
 */
daddr_t
alloc(int size, int mode)
{
	int i, frag;
	daddr_t d, blkno;

	rdfs(fsbtodb(&sblock, cgtod(&sblock, 0)), sblock.fs_cgsize, &acg);
	/* fs -> host byte order */
	if (needswap)
		ffs_cg_swap(&acg, &acg, &sblock);
	if (acg.cg_magic != CG_MAGIC) {
		printf("cg 0: bad magic number\n");
		return (0);
	}
	if (acg.cg_cs.cs_nbfree == 0) {
		printf("first cylinder group ran out of space\n");
		return (0);
	}
	for (d = 0; d < acg.cg_ndblk; d += sblock.fs_frag)
		if (isblock(&sblock, cg_blksfree(&acg, 0),
		    d >> sblock.fs_fragshift))
			goto goth;
	printf("internal error: can't find block in cyl 0\n");
	return (0);
goth:
	blkno = fragstoblks(&sblock, d);
	clrblock(&sblock, cg_blksfree(&acg, 0), blkno);
	if (sblock.fs_contigsumsize > 0)
		clrbit(cg_clustersfree(&acg, 0), blkno);
	acg.cg_cs.cs_nbfree--;
	sblock.fs_cstotal.cs_nbfree--;
	fscs[0].cs_nbfree--;
	if (mode & IFDIR) {
		acg.cg_cs.cs_ndir++;
		sblock.fs_cstotal.cs_ndir++;
		fscs[0].cs_ndir++;
	}
	if (size != sblock.fs_bsize) {
		frag = howmany(size, sblock.fs_fsize);
		fscs[0].cs_nffree += sblock.fs_frag - frag;
		sblock.fs_cstotal.cs_nffree += sblock.fs_frag - frag;
		acg.cg_cs.cs_nffree += sblock.fs_frag - frag;
		acg.cg_frsum[sblock.fs_frag - frag]++;
		for (i = frag; i < sblock.fs_frag; i++)
			setbit(cg_blksfree(&acg, 0), d + i);
	}
	/* host -> fs byte order */
	if (needswap)
		ffs_cg_swap(&acg, &acg, &sblock);
	wtfs(fsbtodb(&sblock, cgtod(&sblock, 0)), sblock.fs_cgsize,
	    (char *)&acg);
	return (d);
}

/*
 * Allocate an inode on the disk
 */
static void
iput(union dinode *ip, ino_t ino)
{
	daddr_t d;
	int c, i;
	struct ufs1_dinode *dp1;
	struct ufs2_dinode *dp2;

	c = ino_to_cg(&sblock, ino);
	rdfs(fsbtodb(&sblock, cgtod(&sblock, 0)), sblock.fs_cgsize, &acg);
	/* fs -> host byte order */
	if (needswap)
		ffs_cg_swap(&acg, &acg, &sblock);
	if (acg.cg_magic != CG_MAGIC) {
		printf("cg 0: bad magic number\n");
		exit(31);
	}
	acg.cg_cs.cs_nifree--;
	setbit(cg_inosused(&acg, 0), ino);
	/* host -> fs byte order */
	if (needswap)
		ffs_cg_swap(&acg, &acg, &sblock);
	wtfs(fsbtodb(&sblock, cgtod(&sblock, 0)), sblock.fs_cgsize,
	    (char *)&acg);
	sblock.fs_cstotal.cs_nifree--;
	fscs[0].cs_nifree--;
	if (ino >= sblock.fs_ipg * sblock.fs_ncg) {
		printf("fsinit: inode value out of range (%d).\n", ino);
		exit(32);
	}
	d = fsbtodb(&sblock, ino_to_fsba(&sblock, ino));
	rdfs(d, sblock.fs_bsize, (char *)iobuf);
	if (sblock.fs_magic == FS_UFS1_MAGIC) {
		dp1 = (struct ufs1_dinode *)iobuf;
		if (needswap) {
			ffs_dinode1_swap(&ip->dp1,
			    &dp1[ino_to_fsbo(&sblock, ino)]);
			/* ffs_dinode1_swap() doesn't swap blocks addrs */
			for (i=0; i<NDADDR + NIADDR; i++)
			    (&dp1[ino_to_fsbo(&sblock, ino)])->di_db[i] = 
				bswap32(ip->dp1.di_db[i]);
		} else
			dp1[ino_to_fsbo(&sblock, ino)] = ip->dp1;
	} else {
		dp2 = (struct ufs2_dinode *)iobuf;
		if (needswap) {
			ffs_dinode2_swap(&ip->dp2,	
			    &dp2[ino_to_fsbo(&sblock, ino)]);
			for (i=0; i<NDADDR + NIADDR; i++)
			    (&dp2[ino_to_fsbo(&sblock, ino)])->di_db[i] = 
				bswap32(ip->dp2.di_db[i]);
		} else
			dp2[ino_to_fsbo(&sblock, ino)] = ip->dp2;
	}
	wtfs(d, sblock.fs_bsize, iobuf);
}

/*
 * read a block from the file system
 */
void
rdfs(daddr_t bno, int size, void *bf)
{
	int n;
	off_t offset;

#ifdef MFS
	if (mfs) {
		memmove(bf, membase + bno * sectorsize, size);
		return;
	}
#endif
	offset = bno;
	offset *= sectorsize;
	if (lseek(fsi, offset, SEEK_SET) < 0) {
		printf("rdfs: seek error for sector %lld: %s\n",
		    (long long)bno, strerror(errno));
		exit(33);
	}
	n = read(fsi, bf, size);
	if (n != size) {
		printf("rdfs: read error for sector %lld: %s\n",
		    (long long)bno, strerror(errno));
		exit(34);
	}
}

/*
 * write a block to the file system
 */
void
wtfs(daddr_t bno, int size, void *bf)
{
	int n;
	off_t offset;

#ifdef MFS
	if (mfs) {
		memmove(membase + bno * sectorsize, bf, size);
		return;
	}
#endif
	if (Nflag)
		return;
	offset = bno;
	offset *= sectorsize;
	if (lseek(fso, offset, SEEK_SET) < 0) {
		printf("wtfs: seek error for sector %lld: %s\n",
		    (long long)bno, strerror(errno));
		exit(35);
	}
	n = write(fso, bf, size);
	if (n != size) {
		printf("wtfs: write error for sector %lld: %s\n",
		    (long long)bno, strerror(errno));
		exit(36);
	}
}

/*
 * check if a block is available
 */
int
isblock(struct fs *fs, unsigned char *cp, int h)
{
	unsigned char mask;

	switch (fs->fs_fragshift) {
	case 3:
		return (cp[h] == 0xff);
	case 2:
		mask = 0x0f << ((h & 0x1) << 2);
		return ((cp[h >> 1] & mask) == mask);
	case 1:
		mask = 0x03 << ((h & 0x3) << 1);
		return ((cp[h >> 2] & mask) == mask);
	case 0:
		mask = 0x01 << (h & 0x7);
		return ((cp[h >> 3] & mask) == mask);
	default:
#ifdef STANDALONE
		printf("isblock bad fs_fragshift %d\n", fs->fs_fragshift);
#else
		fprintf(stderr, "isblock bad fs_fragshift %d\n",
		    fs->fs_fragshift);
#endif
		return (0);
	}
}

/*
 * take a block out of the map
 */
void
clrblock(struct fs *fs, unsigned char *cp, int h)
{
	switch ((fs)->fs_fragshift) {
	case 3:
		cp[h] = 0;
		return;
	case 2:
		cp[h >> 1] &= ~(0x0f << ((h & 0x1) << 2));
		return;
	case 1:
		cp[h >> 2] &= ~(0x03 << ((h & 0x3) << 1));
		return;
	case 0:
		cp[h >> 3] &= ~(0x01 << (h & 0x7));
		return;
	default:
#ifdef STANDALONE
		printf("clrblock bad fs_fragshift %d\n", fs->fs_fragshift);
#else
		fprintf(stderr, "clrblock bad fs_fragshift %d\n",
		    fs->fs_fragshift);
#endif
		return;
	}
}

/*
 * put a block into the map
 */
void
setblock(struct fs *fs, unsigned char *cp, int h)
{
	switch (fs->fs_fragshift) {
	case 3:
		cp[h] = 0xff;
		return;
	case 2:
		cp[h >> 1] |= (0x0f << ((h & 0x1) << 2));
		return;
	case 1:
		cp[h >> 2] |= (0x03 << ((h & 0x3) << 1));
		return;
	case 0:
		cp[h >> 3] |= (0x01 << (h & 0x7));
		return;
	default:
#ifdef STANDALONE
		printf("setblock bad fs_frag %d\n", fs->fs_fragshift);
#else
		fprintf(stderr, "setblock bad fs_fragshift %d\n",
		    fs->fs_fragshift);
#endif
		return;
	}
}

/* copy a direntry to a buffer, in fs byte order */
static void
copy_dir(struct direct *dir, struct direct *dbuf)
{
	memcpy(dbuf, dir, DIRSIZ(Oflag == 0, dir, 0));
	if (needswap) {
		dbuf->d_ino = bswap32(dir->d_ino);
		dbuf->d_reclen = bswap16(dir->d_reclen);
		if (Oflag == 0)
			((struct odirect*)dbuf)->d_namlen =
				bswap16(((struct odirect*)dir)->d_namlen);
	}
}

/* Determine how many digits are needed to print a given integer */
static int
count_digits(int num)
{
	int ndig;

	for(ndig = 1; num > 9; num /=10, ndig++);

	return (ndig);
}

static int
ilog2(int val)
{
	u_int n;

	for (n = 0; n < sizeof(n) * CHAR_BIT; n++)
		if (1 << n == val)
			return (n);
	errx(1, "ilog2: %d is not a power of 2\n", val);
}


#ifdef MFS
/*
 * XXX!
 * Attempt to guess how much more space is available for process data.  The
 * heuristic we use is
 *
 *	max_data_limit - (sbrk(0) - etext) - 128kB
 *
 * etext approximates that start address of the data segment, and the 128kB
 * allows some slop for both segment gap between text and data, and for other
 * (libc) malloc usage.
 */
static void
calc_memfree(void)
{
	extern char etext;
	struct rlimit rlp;
	u_long base;

	base = (u_long)sbrk(0) - (u_long)&etext;
	if (getrlimit(RLIMIT_DATA, &rlp) < 0)
		perror("getrlimit");
	rlp.rlim_cur = rlp.rlim_max;
	if (setrlimit(RLIMIT_DATA, &rlp) < 0)
		perror("setrlimit");
	memleft = rlp.rlim_max - base - (128 * 1024);
}

/*
 * Internal version of malloc that trims the requested size if not enough
 * memory is available.
 */
static void *
mkfs_malloc(size_t size)
{
	u_long pgsz;

	if (size == 0)
		return (NULL);
	if (memleft == 0)
		calc_memfree();

	pgsz = getpagesize() - 1;
	size = (size + pgsz) &~ pgsz;
	if (size > memleft)
		size = memleft;
	memleft -= size;
	return (mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE,
	    -1, 0));
}
#endif	/* MFS */
