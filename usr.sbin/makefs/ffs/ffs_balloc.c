/*	$NetBSD: ffs_balloc.c,v 1.3 2001/11/22 02:47:26 lukem Exp $	*/
/* From NetBSD: ffs_balloc.c,v 1.25 2001/08/08 08:36:36 lukem Exp */

/*
 * Copyright (c) 1982, 1986, 1989, 1993
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
 *
 *	@(#)ffs_balloc.c	8.8 (Berkeley) 6/16/95
 */

#include <sys/cdefs.h>
#ifndef __lint
__RCSID("$NetBSD: ffs_balloc.c,v 1.3 2001/11/22 02:47:26 lukem Exp $");
#endif	/* !__lint */

#include <sys/param.h>
#include <sys/time.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "ufs/ufs/ufs_bswap.h"
#include "ufs/ufs/inode.h"
#include "ufs/ffs/fs.h"

#include "ffs/buf.h"
#include "ffs/ffs_extern.h"

/*
 * Balloc defines the structure of file system storage
 * by allocating the physical blocks on a device given
 * the inode and the logical block number in a file.
 *
 * Assume: flags == B_SYNC | B_CLRBUF
 */
int
ffs_balloc(struct inode *ip, off_t offset, int bufsize, struct buf **bpp)
{
	ufs_daddr_t lbn;
	int size;
	ufs_daddr_t nb;
	struct buf *bp, *nbp;
	struct fs *fs = ip->i_fs;
	struct indir indirs[NIADDR + 2];
	ufs_daddr_t newb, *bap, pref;
	int osize, nsize, num, i, error;
	ufs_daddr_t *allocib, *allocblk, allociblk[NIADDR + 1];
	const int needswap = UFS_FSNEEDSWAP(fs);

	lbn = lblkno(fs, offset);
	size = blkoff(fs, offset) + bufsize;
	if (bpp != NULL) {
		*bpp = NULL;
	}

	assert(size <= fs->fs_bsize);
	if (lbn < 0)
		return (EFBIG);

	/*
	 * If the next write will extend the file into a new block,
	 * and the file is currently composed of a fragment
	 * this fragment has to be extended to be a full block.
	 */

	nb = lblkno(fs, ip->i_ffs_size);
	if (nb < NDADDR && nb < lbn) {
		osize = blksize(fs, ip, nb);
		if (osize < fs->fs_bsize && osize > 0) {
			warnx("need to ffs_realloccg; not supported!");
			abort();
		}
	}

	/*
	 * The first NDADDR blocks are direct blocks
	 */

	if (lbn < NDADDR) {
		nb = ufs_rw32(ip->i_ffs_db[lbn], needswap);
		if (nb != 0 && ip->i_ffs_size >= lblktosize(fs, lbn + 1)) {

			/*
			 * The block is an already-allocated direct block
			 * and the file already extends past this block,
			 * thus this must be a whole block.
			 * Just read the block (if requested).
			 */

			if (bpp != NULL) {
				error = bread(ip->i_fd, ip->i_fs, lbn,
				    fs->fs_bsize, bpp);
				if (error) {
					brelse(*bpp);
					return (error);
				}
			}
			return (0);
		}
		if (nb != 0) {

			/*
			 * Consider need to reallocate a fragment.
			 */

			osize = fragroundup(fs, blkoff(fs, ip->i_ffs_size));
			nsize = fragroundup(fs, size);
			if (nsize <= osize) {

				/*
				 * The existing block is already
				 * at least as big as we want.
				 * Just read the block (if requested).
				 */

				if (bpp != NULL) {
					error = bread(ip->i_fd, ip->i_fs, lbn,
					    osize, bpp);
					if (error) {
						brelse(*bpp);
						return (error);
					}
				}
				return 0;
			} else {
				warnx("need to ffs_realloccg; not supported!");
				abort();
			}
		} else {

			/*
			 * the block was not previously allocated,
			 * allocate a new block or fragment.
			 */

			if (ip->i_ffs_size < lblktosize(fs, lbn + 1))
				nsize = fragroundup(fs, size);
			else
				nsize = fs->fs_bsize;
			error = ffs_alloc(ip, lbn,
			    ffs_blkpref(ip, lbn, (int)lbn, &ip->i_ffs_db[0]),
				nsize, &newb);
			if (error)
				return (error);
			if (bpp != NULL) {
				bp = getblk(ip->i_fd, ip->i_fs, lbn, nsize);
				bp->b_blkno = fsbtodb(fs, newb);
				clrbuf(bp);
				*bpp = bp;
			}
		}
		ip->i_ffs_db[lbn] = ufs_rw32(newb, needswap);
		return (0);
	}
	/*
	 * Determine the number of levels of indirection.
	 */
	pref = 0;
	if ((error = ufs_getlbns(ip, lbn, indirs, &num)) != 0)
		return(error);

	if (num < 1) {
		warnx("ffs_balloc: ufs_getlbns returned indirect block");
		abort();
	}
	/*
	 * Fetch the first indirect block allocating if necessary.
	 */
	--num;
	nb = ufs_rw32(ip->i_ffs_ib[indirs[0].in_off], needswap);
	allocib = NULL;
	allocblk = allociblk;
	if (nb == 0) {
		pref = ffs_blkpref(ip, lbn, 0, (ufs_daddr_t *)0);
		error = ffs_alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb);
		if (error)
			return (error);
		nb = newb;
		*allocblk++ = nb;
		bp = getblk(ip->i_fd, ip->i_fs, indirs[1].in_lbn, fs->fs_bsize);
		bp->b_blkno = fsbtodb(fs, nb);
		clrbuf(bp);
		/*
		 * Write synchronously so that indirect blocks
		 * never point at garbage.
		 */
		if ((error = bwrite(bp)) != 0)
			goto fail;
		allocib = &ip->i_ffs_ib[indirs[0].in_off];
		*allocib = ufs_rw32(nb, needswap);
	}
	/*
	 * Fetch through the indirect blocks, allocating as necessary.
	 */
	for (i = 1;;) {
		error = bread(ip->i_fd, ip->i_fs, indirs[i].in_lbn,
		    (int)fs->fs_bsize, &bp);
		if (error) {
			brelse(bp);
			goto fail;
		}
		bap = (ufs_daddr_t *)bp->b_data;
		nb = ufs_rw32(bap[indirs[i].in_off], needswap);
		if (i == num)
			break;
		i++;
		if (nb != 0) {
			brelse(bp);
			continue;
		}
		if (pref == 0)
			pref = ffs_blkpref(ip, lbn, 0, (ufs_daddr_t *)0);
		error = ffs_alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb);
		if (error) {
			brelse(bp);
			goto fail;
		}
		nb = newb;
		*allocblk++ = nb;
		nbp = getblk(ip->i_fd, ip->i_fs, indirs[i].in_lbn,
		    fs->fs_bsize);
		nbp->b_blkno = fsbtodb(fs, nb);
		clrbuf(nbp);
		/*
		 * Write synchronously so that indirect blocks
		 * never point at garbage.
		 */
		if ((error = bwrite(nbp)) != 0) {
			brelse(bp);
			goto fail;
		}
		bap[indirs[i - 1].in_off] = ufs_rw32(nb, needswap);
		/*
		 * If required, write synchronously, otherwise use
		 * delayed write.
		 */
		bwrite(bp);
	}
	/*
	 * Get the data block, allocating if necessary.
	 */
	if (nb == 0) {
		pref = ffs_blkpref(ip, lbn, indirs[num].in_off, &bap[0]);
		error = ffs_alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb);
		if (error) {
			brelse(bp);
			goto fail;
		}
		nb = newb;
		*allocblk++ = nb;
		if (bpp != NULL) {
			nbp = getblk(ip->i_fd, ip->i_fs, lbn, fs->fs_bsize);
			nbp->b_blkno = fsbtodb(fs, nb);
			clrbuf(nbp);
			*bpp = nbp;
		}
		bap[indirs[num].in_off] = ufs_rw32(nb, needswap);
		/*
		 * If required, write synchronously, otherwise use
		 * delayed write.
		 */
		bwrite(bp);
		return (0);
	}
	brelse(bp);
	if (bpp != NULL) {
			error = bread(ip->i_fd, ip->i_fs, lbn,
			    (int)fs->fs_bsize, &nbp);
			if (error) {
				brelse(nbp);
				goto fail;
			}
		*bpp = nbp;
	}
	return (0);
fail:
	return (error);
}
