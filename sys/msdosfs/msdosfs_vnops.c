/*	$NetBSD: msdosfs_vnops.c,v 1.26 1994/12/13 20:16:50 mycroft Exp $	*/

/*-
 * Copyright (C) 1994 Wolfgang Solfrank.
 * Copyright (C) 1994 TooLs GmbH.
 * All rights reserved.
 * Original code by Paul Popelka (paulp@uts.amdahl.com) (see below).
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
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Written by Paul Popelka (paulp@uts.amdahl.com)
 * 
 * You can do anything you want with this software, just don't say you wrote
 * it, and don't remove this notice.
 * 
 * This software is provided "as is".
 * 
 * The author supplies this software to be publicly redistributed on the
 * understanding that the author is not responsible for the correct
 * functioning of this software in any circumstances and is not liable for
 * any damages caused by this software.
 * 
 * October 1992
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/resourcevar.h>	/* defines plimit structure in proc struct */
#include <sys/kernel.h>
#include <sys/file.h>		/* define FWRITE ... */
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <miscfs/specfs/specdev.h> /* XXX */	/* defines v_rdev */
#include <sys/malloc.h>
#include <sys/dir.h>		/* defines dirent structure */

#include <msdosfs/bpb.h>
#include <msdosfs/direntry.h>
#include <msdosfs/denode.h>
#include <msdosfs/msdosfsmount.h>
#include <msdosfs/fat.h>

/*
 * Some general notes:
 * 
 * In the ufs filesystem the inodes, superblocks, and indirect blocks are
 * read/written using the vnode for the filesystem. Blocks that represent
 * the contents of a file are read/written using the vnode for the file
 * (including directories when they are read/written as files). This
 * presents problems for the dos filesystem because data that should be in
 * an inode (if dos had them) resides in the directory itself.  Since we
 * must update directory entries without the benefit of having the vnode
 * for the directory we must use the vnode for the filesystem.  This means
 * that when a directory is actually read/written (via read, write, or
 * readdir, or seek) we must use the vnode for the filesystem instead of
 * the vnode for the directory as would happen in ufs. This is to insure we
 * retreive the correct block from the buffer cache since the hash value is
 * based upon the vnode address and the desired block number.
 */

/*
 * Create a regular file. On entry the directory to contain the file being
 * created is locked.  We must release before we return. We must also free
 * the pathname buffer pointed at by cnp->cn_pnbuf, always on error, or
 * only if the SAVESTART bit in cn_flags is clear on success.
 */
int
msdosfs_create(ap)
	struct vop_create_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap;
{
	struct componentname *cnp = ap->a_cnp;
	struct denode ndirent;
	struct denode *dep;
	struct denode *pdep = VTODE(ap->a_dvp);
	struct timespec ts;
	int error;

#ifdef MSDOSFS_DEBUG
	printf("msdosfs_create(cnp %08x, vap %08x\n", cnp, ap->a_vap);
#endif

	/*
	 * If this is the root directory and there is no space left we
	 * can't do anything.  This is because the root directory can not
	 * change size.
	 */
	if (pdep->de_StartCluster == MSDOSFSROOT && pdep->de_fndclust == (u_long)-1) {
		error = ENOSPC;
		goto bad;
	}

	bzero(&ndirent, sizeof(ndirent));
	TIMEVAL_TO_TIMESPEC(&time, &ts);
	unix2dostime(&ts, &ndirent.de_Date, &ndirent.de_Time);

	/*
	 * Create a directory entry for the file, then call createde() to
	 * have it installed. NOTE: DOS files are always executable.  We
	 * use the absence of the owner write bit to make the file
	 * readonly.
	 */
#ifdef DIAGNOSTIC
	if ((cnp->cn_flags & HASBUF) == 0)
		panic("msdosfs_create: no name");
#endif
	unix2dosfn((u_char *)cnp->cn_nameptr, ndirent.de_Name, cnp->cn_namelen);
	ndirent.de_Attributes = (ap->a_vap->va_mode & VWRITE) ? 0 : ATTR_READONLY;
	ndirent.de_StartCluster = 0;
	ndirent.de_FileSize = 0;
	ndirent.de_dev = pdep->de_dev;
	ndirent.de_devvp = pdep->de_devvp;
	if (error = createde(&ndirent, pdep, &dep))
		goto bad;
	if ((cnp->cn_flags & SAVESTART) == 0)
		FREE(cnp->cn_pnbuf, M_NAMEI);
	vput(ap->a_dvp);
	*ap->a_vpp = DETOV(dep);
	return (0);

bad:
	FREE(cnp->cn_pnbuf, M_NAMEI);
	vput(ap->a_dvp);
	return (error);
}

int
msdosfs_mknod(ap)
	struct vop_mknod_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap;
{
	int error;
	
	switch (ap->a_vap->va_type) {
	case VDIR:
		return (msdosfs_mkdir((struct vop_mkdir_args *)ap));
		break;

	case VREG:
		return (msdosfs_create((struct vop_create_args *)ap));
		break;

	default:
		FREE(ap->a_cnp->cn_pnbuf, M_NAMEI);
		vput(ap->a_dvp);
		return (EINVAL);
	}
	/* NOTREACHED */
}

int
msdosfs_open(ap)
	struct vop_open_args /* {
		struct vnode *a_vp;
		int a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{

	return (0);
}

int
msdosfs_close(ap)
	struct vop_close_args /* {
		struct vnode *a_vp;
		int a_fflag;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct denode *dep = VTODE(vp);
	struct timespec ts;

	if (vp->v_usecount > 1 && !(dep->de_flag & DE_LOCKED)) {
		TIMEVAL_TO_TIMESPEC(&time, &ts);
		DE_TIMES(dep, &ts);
	}
	return (0);
}

int
msdosfs_access(ap)
	struct vop_access_args /* {
		struct vnode *a_vp;
		int a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	struct denode *dep = VTODE(ap->a_vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct ucred *cred = ap->a_cred;
	mode_t dosmode, mask, mode = ap->a_mode;
	register gid_t *gp;
	int i;
	
	dosmode = (S_IXUSR|S_IXGRP|S_IXOTH) | (S_IRUSR|S_IRGRP|S_IROTH) |
	    ((dep->de_Attributes & ATTR_READONLY) ? 0 : (S_IWUSR|S_IWGRP|S_IWOTH));
	dosmode &= pmp->pm_mask;
	
	/* User id 0 always gets access. */
	if (cred->cr_uid == 0)
		return (0);
	
	mask = 0;
	
	/* Otherwise, check the owner. */
	if (cred->cr_uid == pmp->pm_uid) {
		if (mode & VEXEC)
			mask |= S_IXUSR;
		if (mode & VREAD)
			mask |= S_IRUSR;
		if (mode & VWRITE)
			mask |= S_IWUSR;
		return ((dosmode & mask) == mask ? 0 : EACCES);
	}
	
	/* Otherwise, check the groups. */
	for (i = 0, gp = cred->cr_groups; i < cred->cr_ngroups; i++, gp++)
		if (pmp->pm_gid == *gp) {
			if (mode & VEXEC)
				mask |= S_IXGRP;
			if (mode & VREAD)
				mask |= S_IRGRP;
			if (mode & VWRITE)
				mask |= S_IWGRP;
			return ((dosmode & mask) == mask ? 0 : EACCES);
		}
	
	/* Otherwise, check everyone else. */
	if (mode & VEXEC)
		mask |= S_IXOTH;
	if (mode & VREAD)
		mask |= S_IROTH;
	if (mode & VWRITE)
		mask |= S_IWOTH;
	return ((dosmode & mask) == mask ? 0 : EACCES);
}

int
msdosfs_getattr(ap)
	struct vop_getattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	u_int cn;
	struct denode *dep = VTODE(ap->a_vp);
	struct vattr *vap = ap->a_vap;
	struct timespec ts;

	TIMEVAL_TO_TIMESPEC(&time, &ts);
	DE_TIMES(dep, &ts);
	vap->va_fsid = dep->de_dev;
	/*
	 * The following computation of the fileid must be the same as that
	 * used in msdosfs_readdir() to compute d_fileno. If not, pwd
	 * doesn't work.
	 */
	if (dep->de_Attributes & ATTR_DIRECTORY) {
		if ((cn = dep->de_StartCluster) == MSDOSFSROOT)
			cn = 1;
	} else {
		if ((cn = dep->de_dirclust) == MSDOSFSROOT)
			cn = 1;
		cn = (cn << 16) | (dep->de_diroffset & 0xffff);
	}
	vap->va_fileid = cn;
	vap->va_mode = (S_IXUSR|S_IXGRP|S_IXOTH) | (S_IRUSR|S_IRGRP|S_IROTH) |
		((dep->de_Attributes & ATTR_READONLY) ? 0 : (S_IWUSR|S_IWGRP|S_IWOTH));
	vap->va_mode &= dep->de_pmp->pm_mask;
	if (dep->de_Attributes & ATTR_DIRECTORY)
		vap->va_mode |= S_IFDIR;
	vap->va_nlink = 1;
	vap->va_gid = dep->de_pmp->pm_gid;
	vap->va_uid = dep->de_pmp->pm_uid;
	vap->va_rdev = 0;
	vap->va_size = dep->de_FileSize;
	dos2unixtime(dep->de_Date, dep->de_Time, &vap->va_atime);
	vap->va_mtime = vap->va_atime;
#ifndef MSDOSFS_NODIRMOD
	if (vap->va_mode & S_IFDIR)
		TIMEVAL_TO_TIMESPEC(&time, &vap->va_mtime);
#endif
	vap->va_ctime = vap->va_atime;
	vap->va_flags = dep->de_flag;
	vap->va_gen = 0;
	vap->va_blocksize = dep->de_pmp->pm_bpcluster;
	vap->va_bytes = (dep->de_FileSize + dep->de_pmp->pm_crbomask) &
	    			~(dep->de_pmp->pm_crbomask);
	vap->va_type = ap->a_vp->v_type;
	return (0);
}

int
msdosfs_setattr(ap)
	struct vop_setattr_args /* {
		struct vnode *a_vp;
		struct vattr *a_vap;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	int error = 0;
	struct denode *dep = VTODE(ap->a_vp);
	struct vattr *vap = ap->a_vap;
	struct ucred *cred = ap->a_cred;
	
#ifdef MSDOSFS_DEBUG
	printf("msdosfs_setattr(): vp %08x, vap %08x, cred %08x, p %08x\n",
	       ap->a_vp, vap, cred, ap->a_p);
#endif
	if ((vap->va_type != VNON) || (vap->va_nlink != VNOVAL) ||
	    (vap->va_fsid != VNOVAL) || (vap->va_fileid != VNOVAL) ||
	    (vap->va_blocksize != VNOVAL) || (vap->va_rdev != VNOVAL) ||
	    (vap->va_bytes != VNOVAL) || (vap->va_gen != VNOVAL) ||
	    (vap->va_uid != VNOVAL) || (vap->va_gid != VNOVAL) ||
	    (vap->va_flags != VNOVAL)) {
#ifdef MSDOSFS_DEBUG
		printf("msdosfs_setattr(): returning EINVAL\n");
		printf("    va_type %d, va_nlink %x, va_fsid %x, va_fileid %x\n",
		       vap->va_type, vap->va_nlink, vap->va_fsid, vap->va_fileid);
		printf("    va_blocksize %x, va_rdev %x, va_bytes %x, va_gen %x\n",
		       vap->va_blocksize, vap->va_rdev, vap->va_bytes, vap->va_gen);
		printf("    va_uid %x, va_gid %x\n",
		       vap->va_uid, vap->va_gid);
#endif
		return (EINVAL);
	}
	if (vap->va_size != VNOVAL) {
		if (ap->a_vp->v_type == VDIR)
			return (EISDIR);
		if (error = detrunc(dep, (u_long)vap->va_size, 0, cred, ap->a_p))
			return (error);
	}
	if (vap->va_mtime.ts_sec != VNOVAL) {
		dep->de_flag |= DE_UPDATE;
		if (error = deupdat(dep, &vap->va_mtime, 1))
			return (error);
	}
	/*
	 * DOS files only have the ability to have thier writability
	 * attribute set, so we use the owner write bit to set the readonly
	 * attribute.
	 */
	if (vap->va_mode != (mode_t)VNOVAL) {
		/* We ignore the read and execute bits */
		if (vap->va_mode & VWRITE)
			dep->de_Attributes &= ~ATTR_READONLY;
		else
			dep->de_Attributes |= ATTR_READONLY;
		dep->de_flag |= DE_UPDATE;
	}
	return (0);
}

int
msdosfs_read(ap)
	struct vop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int a_ioflag;
		struct ucred *a_cred;
	} */ *ap;
{
	int error = 0;
	int diff;
	int isadir;
	long n;
	long on;
	daddr_t lbn;
	daddr_t rablock;
	struct buf *bp;
	struct vnode *vp = ap->a_vp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct uio *uio = ap->a_uio;

	/*
	 * If they didn't ask for any data, then we are done.
	 */
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0)
		return (EINVAL);

	isadir = dep->de_Attributes & ATTR_DIRECTORY;
	do {
		lbn = uio->uio_offset >> pmp->pm_cnshift;
		on = uio->uio_offset & pmp->pm_crbomask;
		n = min((u_long) (pmp->pm_bpcluster - on), uio->uio_resid);
		diff = dep->de_FileSize - uio->uio_offset;
		if (diff <= 0)
			return (0);
		/* convert cluster # to block # if a directory */
		if (isadir) {
			error = pcbmap(dep, lbn, &lbn, 0);
			if (error)
				return (error);
		}
		if (diff < n)
			n = diff;
		/*
		 * If we are operating on a directory file then be sure to
		 * do i/o with the vnode for the filesystem instead of the
		 * vnode for the directory.
		 */
		if (isadir) {
			error = bread(pmp->pm_devvp, lbn, pmp->pm_bpcluster,
			    NOCRED, &bp);
		} else {
			rablock = lbn + 1;
			if (vp->v_lastr + 1 == lbn &&
			    rablock * pmp->pm_bpcluster < dep->de_FileSize) {
				error = breada(vp, lbn, pmp->pm_bpcluster,
					       rablock, pmp->pm_bpcluster, NOCRED, &bp);
			} else {
				error = bread(vp, lbn, pmp->pm_bpcluster, NOCRED,
					      &bp);
			}
			vp->v_lastr = lbn;
		}
		n = min(n, pmp->pm_bpcluster - bp->b_resid);
		if (error) {
			brelse(bp);
			return (error);
		}
		error = uiomove(bp->b_data + on, (int) n, uio);
		/*
		 * If we have read everything from this block or have read
		 * to end of file then we are done with this block.  Mark
		 * it to say the buffer can be reused if need be.
		 */
#if 0
		if (n + on == pmp->pm_bpcluster ||
		    uio->uio_offset == dep->de_FileSize)
			bp->b_flags |= B_AGE;
#endif
		brelse(bp);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	return (error);
}

/*
 * Write data to a file or directory.
 */
int
msdosfs_write(ap)
	struct vop_write_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int a_ioflag;
		struct ucred *a_cred;
	} */ *ap;
{
	int n;
	int isadir;
	int croffset;
	int resid;
	u_long osize;
	int error;
	u_long count;
	daddr_t bn, lastcn;
	struct buf *bp;
	int ioflag = ap->a_ioflag;
	struct uio *uio = ap->a_uio;
	struct proc *p = uio->uio_procp;
	struct vnode *vp = ap->a_vp;
	struct vnode *thisvp;
	struct denode *dep = VTODE(vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct ucred *cred = ap->a_cred;
	struct timespec ts;
	
#ifdef MSDOSFS_DEBUG
	printf("msdosfs_write(vp %08x, uio %08x, ioflag %08x, cred %08x\n",
	       vp, uio, ioflag, cred);
	printf("msdosfs_write(): diroff %d, dirclust %d, startcluster %d\n",
	       dep->de_diroffset, dep->de_dirclust, dep->de_StartCluster);
#endif

	switch (vp->v_type) {
	case VREG:
		if (ioflag & IO_APPEND)
			uio->uio_offset = dep->de_FileSize;
		isadir = 0;
		thisvp = vp;
		break;

	case VDIR:
		if ((ioflag & IO_SYNC) == 0)
			panic("msdosfs_write(): non-sync directory update");
		isadir = 1;
		thisvp = pmp->pm_devvp;
		break;

	default:
		panic("msdosfs_write(): bad file type");
		break;
	}

	if (uio->uio_offset < 0)
		return (EINVAL);

	if (uio->uio_resid == 0)
		return (0);

	/*
	 * If they've exceeded their filesize limit, tell them about it.
	 */
	if (vp->v_type == VREG && p &&
	    ((uio->uio_offset + uio->uio_resid) >
		p->p_rlimit[RLIMIT_FSIZE].rlim_cur)) {
		psignal(p, SIGXFSZ);
		return (EFBIG);
	}

	/*
	 * If attempting to write beyond the end of the root directory we
	 * stop that here because the root directory can not grow.
	 */
	if ((dep->de_Attributes & ATTR_DIRECTORY) &&
	    dep->de_StartCluster == MSDOSFSROOT &&
	    (uio->uio_offset + uio->uio_resid) > dep->de_FileSize)
		return (ENOSPC);

	/*
	 * If the offset we are starting the write at is beyond the end of
	 * the file, then they've done a seek.  Unix filesystems allow
	 * files with holes in them, DOS doesn't so we must fill the hole
	 * with zeroed blocks.
	 */
	if (uio->uio_offset > dep->de_FileSize) {
		if (error = deextend(dep, uio->uio_offset, cred))
			return (error);
	}

	/*
	 * Remember some values in case the write fails.
	 */
	resid = uio->uio_resid;
	osize = dep->de_FileSize;

	/*
	 * If we write beyond the end of the file, extend it to its ultimate
	 * size ahead of the time to hopefully get a contiguous area.
	 */
	if (uio->uio_offset + resid > osize) {
		count = de_clcount(pmp, uio->uio_offset + resid) -
			de_clcount(pmp, osize);
		if ((error = extendfile(dep, count, NULL, NULL, 0)) &&
		    (error != ENOSPC || (ioflag & IO_UNIT)))
			goto errexit;
		lastcn = dep->de_fc[FC_LASTFC].fc_frcn;
	} else
		lastcn = de_clcount(pmp, osize) - 1;
	
	do {
		bn = de_blk(pmp, uio->uio_offset);
		if (isadir) {
			if (error = pcbmap(dep, bn, &bn, 0))
				break;
		} else if (bn > lastcn) {
			error = ENOSPC;
			break;
		}
		
		if ((uio->uio_offset & pmp->pm_crbomask) == 0
		    && (de_blk(pmp, uio->uio_offset + uio->uio_resid) > de_blk(pmp, uio->uio_offset)
			|| uio->uio_offset + uio->uio_resid >= dep->de_FileSize)) {
			/*
			 * If either the whole cluster gets written,
			 * or we write the cluster from its start beyond EOF,
			 * then no need to read data from disk.
			 */
			bp = getblk(thisvp, bn, pmp->pm_bpcluster, 0, 0);
			clrbuf(bp);
			/*
			 * Do the bmap now, since pcbmap needs buffers
			 * for the fat table. (see msdosfs_strategy)
			 */
			if (!isadir) {
				if (bp->b_blkno == bp->b_lblkno) {
					if (error = pcbmap(dep, bp->b_lblkno,
							   &bp->b_blkno, 0)) 
						bp->b_blkno = -1;
				}
				if (bp->b_blkno == -1) {
					brelse(bp);
					if (!error)
						error = EIO;		/* XXX */
					break;
				}
			}
		} else {
			/*
			 * The block we need to write into exists, so read it in.
			 */
			if (error = bread(thisvp, bn, pmp->pm_bpcluster, cred, &bp))
				break;
		}

		croffset = uio->uio_offset & pmp->pm_crbomask;
		n = min(uio->uio_resid, pmp->pm_bpcluster - croffset);
		if (uio->uio_offset + n > dep->de_FileSize) {
			dep->de_FileSize = uio->uio_offset + n;
			vnode_pager_setsize(vp, dep->de_FileSize);	/* why? */
		}
		(void) vnode_pager_uncache(vp);	/* why not? */
		/*
		 * Should these vnode_pager_* functions be done on dir
		 * files?
		 */

		/*
		 * Copy the data from user space into the buf header.
		 */
		error = uiomove(bp->b_data + croffset, n, uio);

		/*
		 * If they want this synchronous then write it and wait for
		 * it.  Otherwise, if on a cluster boundary write it
		 * asynchronously so we can move on to the next block
		 * without delay.  Otherwise do a delayed write because we
		 * may want to write somemore into the block later.
		 */
		if (ioflag & IO_SYNC)
			(void) bwrite(bp);
		else if (n + croffset == pmp->pm_bpcluster) {
			bp->b_flags |= B_AGE;
			bawrite(bp);
		} else
			bdwrite(bp);
		dep->de_flag |= DE_UPDATE;
	} while (error == 0 && uio->uio_resid > 0);

	/*
	 * If the write failed and they want us to, truncate the file back
	 * to the size it was before the write was attempted.
	 */
errexit:
	if (error) {
		if (ioflag & IO_UNIT) {
			detrunc(dep, osize, ioflag & IO_SYNC, NOCRED, NULL);
			uio->uio_offset -= resid - uio->uio_resid;
			uio->uio_resid = resid;
		} else {
			detrunc(dep, dep->de_FileSize, ioflag & IO_SYNC, NOCRED, NULL);
			if (uio->uio_resid != resid)
				error = 0;
		}
	} else {
		TIMEVAL_TO_TIMESPEC(&time, &ts);
		error = deupdat(dep, &ts, 1);
	}
	return (error);
}

int
msdosfs_ioctl(ap)
	struct vop_ioctl_args /* {
		struct vnode *a_vp;
		u_long a_command;
		caddr_t a_data;
		int a_fflag;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{

	return (ENOTTY);
}

int
msdosfs_select(ap)
	struct vop_select_args /* {
		struct vnode *a_vp;
		int a_which;
		int a_fflags;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{

	return (1);		/* DOS filesystems never block? */
}

int
msdosfs_mmap(ap)
	struct vop_mmap_args /* {
		struct vnode *a_vp;
		int a_fflags;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{

	return (EINVAL);
}

/*
 * Flush the blocks of a file to disk.
 * 
 * This function is worthless for vnodes that represent directories. Maybe we
 * could just do a sync if they try an fsync on a directory file.
 */
int
msdosfs_fsync(ap)
	struct vop_fsync_args /* {
		struct vnode *a_vp;
		struct ucred *a_cred;
		int a_waitfor;
		struct proc *a_p;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct timespec ts;

	vflushbuf(vp, ap->a_waitfor == MNT_WAIT);
	TIMEVAL_TO_TIMESPEC(&time, &ts);
	return (deupdat(VTODE(vp), &ts, ap->a_waitfor == MNT_WAIT));
}

/*
 * Now the whole work of extending a file is done in the write function.
 * So nothing to do here.
 */
int
msdosfs_seek(ap)
	struct vop_seek_args /* {
		struct vnode *a_vp;
		off_t a_oldoff;
		off_t a_newoff;
		struct ucred *a_cred;
	} */ *ap;
{

	return (0);
}

int
msdosfs_remove(ap)
	struct vop_remove_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap;
{
	struct denode *dep = VTODE(ap->a_vp);
	struct denode *ddep = VTODE(ap->a_dvp);
	int error;

	error = removede(ddep, dep);
#ifdef MSDOSFS_DEBUG
	printf("msdosfs_remove(), dep %08x, v_usecount %d\n", dep, ap->a_vp->v_usecount);
#endif
	if (ddep == dep)
		vrele(ap->a_vp);
	else
		vput(ap->a_vp);	/* causes msdosfs_inactive() to be called
				 * via vrele() */
	vput(ap->a_dvp);
	return (error);
}

/*
 * DOS filesystems don't know what links are. But since we already called
 * msdosfs_lookup() with create and lockparent, the parent is locked so we
 * have to free it before we return the error.
 */
int
msdosfs_link(ap)
	struct vop_link_args /* {
		struct vnode *a_vp;
		struct vnode *a_tdvp;
		struct componentname *a_cnp;
	} */ *ap;
{

	return (VOP_ABORTOP(ap->a_vp, ap->a_cnp));
}

/*
 * Renames on files require moving the denode to a new hash queue since the
 * denode's location is used to compute which hash queue to put the file
 * in. Unless it is a rename in place.  For example "mv a b".
 * 
 * What follows is the basic algorithm:
 * 
 * if (file move) {
 *	if (dest file exists) {
 *		remove dest file
 *	}
 *	if (dest and src in same directory) {
 *		rewrite name in existing directory slot
 *	} else {
 *		write new entry in dest directory
 *		update offset and dirclust in denode
 *		move denode to new hash chain
 *		clear old directory entry
 *	}
 * } else {
 *	directory move
 *	if (dest directory exists) {
 *		if (dest is not empty) {
 *			return ENOTEMPTY
 *		}
 *		remove dest directory
 *	}
 *	if (dest and src in same directory) {
 *		rewrite name in existing entry
 *	} else {
 *		be sure dest is not a child of src directory
 *		write entry in dest directory
 *		update "." and ".." in moved directory
 *		update offset and dirclust in denode
 *		move denode to new hash chain
 *		clear old directory entry for moved directory
 *	}
 * }
 * 
 * On entry:
 *	source's parent directory is unlocked
 *	source file or directory is unlocked
 *	destination's parent directory is locked
 *	destination file or directory is locked if it exists
 * 
 * On exit:
 *	all denodes should be released
 *
 * Notes:
 * I'm not sure how the memory containing the pathnames pointed at by the
 * componentname structures is freed, there may be some memory bleeding
 * for each rename done.
 */
int
msdosfs_rename(ap)
	struct vop_rename_args /* {
		struct vnode *a_fdvp;
		struct vnode *a_fvp;
		struct componentname *a_fcnp;
		struct vnode *a_tdvp;
		struct vnode *a_tvp;
		struct componentname *a_tcnp;
	} */ *ap;
{
	struct vnode *tvp = ap->a_tvp;
	register struct vnode *tdvp = ap->a_tdvp;
	struct vnode *fvp = ap->a_fvp;
	register struct vnode *fdvp = ap->a_fdvp;
	register struct componentname *tcnp = ap->a_tcnp;
	register struct componentname *fcnp = ap->a_fcnp;
	register struct denode *ip, *xp, *dp;
	u_char toname[11];
	int doingdirectory = 0, newparent = 0;
	int error;
	u_long cn;
	daddr_t bn;
	struct msdosfsmount *pmp;
	struct direntry *dotdotp;
	struct direntry *ep;
	struct buf *bp;

	pmp = VFSTOMSDOSFS(fdvp->v_mount);

#ifdef DIAGNOSTIC
	if ((tcnp->cn_flags & HASBUF) == 0 ||
	    (fcnp->cn_flags & HASBUF) == 0)
		panic("msdosfs_rename: no name");
#endif
	/*
	 * Check for cross-device rename.
	 */
	if ((fvp->v_mount != tdvp->v_mount) ||
	    (tvp && (fvp->v_mount != tvp->v_mount))) {
		error = EXDEV;
abortit:
		VOP_ABORTOP(tdvp, tcnp);
		if (tdvp == tvp)
			vrele(tdvp);
		else
			vput(tdvp);
		if (tvp)
			vput(tvp);
		VOP_ABORTOP(fdvp, fcnp);
		vrele(fdvp);
		vrele(fvp);
		return (error);
	}

	/*
	 * Convert the filename in tcnp into a dos filename. We copy this
	 * into the denode and directory entry for the destination
	 * file/directory.
	 */
	unix2dosfn((u_char *)tcnp->cn_nameptr, toname, tcnp->cn_namelen);

	/* */
	if (error = VOP_LOCK(fvp))
		goto abortit;
	dp = VTODE(fdvp);
	ip = VTODE(fvp);

	/*
	 * Be sure we are not renaming ".", "..", or an alias of ".". This
	 * leads to a crippled directory tree.  It's pretty tough to do a
	 * "ls" or "pwd" with the "." directory entry missing, and "cd .."
	 * doesn't work if the ".." entry is missing.
	 */
	if (ip->de_Attributes & ATTR_DIRECTORY) {
		/*
		 * Avoid ".", "..", and aliases of "." for obvious reasons.
		 */
		if ((fcnp->cn_namelen == 1 && fcnp->cn_nameptr[0] == '.') ||
		    dp == ip || (fcnp->cn_flags & ISDOTDOT)) {
			VOP_UNLOCK(fvp);
			error = EINVAL;
			goto abortit;
		}
		doingdirectory++;
	}

	/*
	 * When the target exists, both the directory
	 * and target vnodes are returned locked.
	 */
	dp = VTODE(tdvp);
	xp = tvp ? VTODE(tvp) : NULL;

	/*
	 * If ".." must be changed (ie the directory gets a new
	 * parent) then the source directory must not be in the
	 * directory heirarchy above the target, as this would
	 * orphan everything below the source directory. Also
	 * the user must have write permission in the source so
	 * as to be able to change "..". We must repeat the call 
	 * to namei, as the parent directory is unlocked by the
	 * call to doscheckpath().
	 */
	error = VOP_ACCESS(fvp, VWRITE, tcnp->cn_cred, tcnp->cn_proc);
	VOP_UNLOCK(fvp);
	if (VTODE(fdvp)->de_StartCluster != VTODE(tdvp)->de_StartCluster)
		newparent = 1;
	if (doingdirectory && newparent) {
		if (error)	/* write access check above */
			goto bad;
		if (xp != NULL)
			vput(tvp);
		/* doscheckpath() vput()'s dp */
		if (error = doscheckpath(ip, dp))
			goto out;
		if ((tcnp->cn_flags & SAVESTART) == 0)
			panic("msdosfs_rename: lost to startdir");
		if (error = relookup(tdvp, &tvp, tcnp))
			goto out;
		dp = VTODE(tdvp);
		xp = tvp ? VTODE(tvp) : NULL;
	}

	if (xp != NULL) {
		u_long to_dirclust, to_diroffset;

		/*
		 * Target must be empty if a directory and have no links
		 * to it. Also, ensure source and target are compatible
		 * (both directories, or both not directories).
		 */
		if (xp->de_Attributes & ATTR_DIRECTORY) {
			if (!dosdirempty(xp)) {
				error = ENOTEMPTY;
				goto bad;
			}
			if (!doingdirectory) {
				error = ENOTDIR;
				goto bad;
			}
			cache_purge(tdvp);
		} else if (doingdirectory) {
			error = EISDIR;
			goto bad;
		}
		to_dirclust = xp->de_dirclust;
		to_diroffset = xp->de_diroffset;
		if (error = removede(dp, xp))
			goto bad;
		vput(tvp);
		xp = NULL;
		/*
		 * Remember where the slot was for createde().
		 */
		dp->de_fndclust = to_dirclust;
		dp->de_fndoffset = to_diroffset;
	}

	/*
	 * If the source and destination are in the same directory then
	 * just read in the directory entry, change the name in the
	 * directory entry and write it back to disk.
	 */
	VOP_LOCK(fvp);
	if (newparent == 0) {
		if (error = readep(dp->de_pmp,
				   ip->de_dirclust,
				   ip->de_diroffset,
				   &bp, &ep)) {
			VOP_UNLOCK(fvp);
			goto bad;
		}
		bcopy(toname, ep->deName, 11);
		if (error = bwrite(bp)) {
			VOP_UNLOCK(fvp);
			goto bad;
		}
		bcopy(toname, ip->de_Name, 11);	/* update denode */
	} else {
		struct denode *zp;

		/*
		 * If the source and destination are in different
		 * directories, then mark the entry in the source directory
		 * as deleted and write a new entry in the destination
		 * directory.  Then move the denode to the correct hash
		 * chain for its new location in the filesystem.  And, if
		 * we moved a directory, then update its .. entry to point
		 * to the new parent directory.
		 */
		bcopy(toname, ip->de_Name, 11);	/* update denode */
		if (error = createde(ip, dp, (struct denode **)0)) {
			/* XXX should put back filename */
			VOP_UNLOCK(fvp);
			goto bad;
		}
		VOP_LOCK(fdvp);
		zp = VTODE(fdvp);
		if (error = readep(zp->de_pmp, zp->de_fndclust, zp->de_fndoffset,
				   &bp, &ep)) {
			VOP_UNLOCK(fvp);
			VOP_UNLOCK(fdvp);
			goto bad;
		}
		ep->deName[0] = SLOT_DELETED;
		if (error = bwrite(bp)) {
			VOP_UNLOCK(fvp);
			VOP_UNLOCK(fdvp);
			goto bad;
		}
		ip->de_dirclust = dp->de_fndclust;
		ip->de_diroffset = dp->de_fndoffset;
		reinsert(ip);
		VOP_UNLOCK(fdvp);
	}

	/*
	 * If we moved a directory to a new parent directory, then we must
	 * fixup the ".." entry in the moved directory.
	 */
	if (doingdirectory && newparent) {
		cn = ip->de_StartCluster;
		if (cn == MSDOSFSROOT) {
			/* this should never happen */
			panic("msdosfs_rename: updating .. in root directory?\n");
		} else
			bn = cntobn(pmp, cn);
		if (error = bread(pmp->pm_devvp, bn, pmp->pm_bpcluster, NOCRED,
				  &bp)) {
			/* XXX should really panic here, fs is corrupt */
			VOP_UNLOCK(fvp);
			goto bad;
		}
		dotdotp = (struct direntry *)bp->b_data + 1;
		putushort(dotdotp->deStartCluster, dp->de_StartCluster);
		if (error = bwrite(bp)) {
			/* XXX should really panic here, fs is corrupt */
			VOP_UNLOCK(fvp);
			goto bad;
		}
	}
	VOP_UNLOCK(fvp);
bad:
	if (xp)
		vput(tvp);
	vput(tdvp);
out:
	vrele(fdvp);
	vrele(fvp);
	return (error);

}

struct {
	struct direntry dot;
	struct direntry dotdot;
} dosdirtemplate = {
	".       ", "   ",		/* the . entry */
	ATTR_DIRECTORY,			/* file attribute */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* resevered */
	210, 4, 210, 4,			/* time and date */
	0, 0,				/* startcluster */
	0, 0, 0, 0,			/* filesize */
	"..      ", "   ",		/* the .. entry */
	ATTR_DIRECTORY,			/* file attribute */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* resevered */
	210, 4, 210, 4,			/* time and date */
	0, 0,				/* startcluster */
	0, 0, 0, 0,			/* filesize */
};

int
msdosfs_mkdir(ap)
	struct vop_mkdir_args /* {
		struct vnode *a_dvp;
		struvt vnode **a_vpp;
		struvt componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap;
{
	struct componentname *cnp = ap->a_cnp;
	struct denode ndirent;
	struct denode *dep;
	struct denode *pdep = VTODE(ap->a_dvp);
	struct timespec ts;
	int error;
	int bn;
	u_long newcluster;
	struct direntry *denp;
	struct msdosfsmount *pmp = pdep->de_pmp;
	struct buf *bp;

	/*
	 * If this is the root directory and there is no space left we
	 * can't do anything.  This is because the root directory can not
	 * change size.
	 */
	if (pdep->de_StartCluster == MSDOSFSROOT && pdep->de_fndclust == (u_long)-1) {
		error = ENOSPC;
		goto bad2;
	}

	/*
	 * Allocate a cluster to hold the about to be created directory.
	 */
	if (error = clusteralloc(pmp, 0, 1, CLUST_EOFE, &newcluster, NULL))
		goto bad2;

	bzero(&ndirent, sizeof(ndirent));
	TIMEVAL_TO_TIMESPEC(&time, &ts);
	unix2dostime(&ts, &ndirent.de_Date, &ndirent.de_Time);

	/*
	 * Now fill the cluster with the "." and ".." entries. And write
	 * the cluster to disk.  This way it is there for the parent
	 * directory to be pointing at if there were a crash.
	 */
	bn = cntobn(pmp, newcluster);
	/* always succeeds */
	bp = getblk(pmp->pm_devvp, bn, pmp->pm_bpcluster, 0, 0);
	bzero(bp->b_data, pmp->pm_bpcluster);
	bcopy(&dosdirtemplate, bp->b_data, sizeof dosdirtemplate);
	denp = (struct direntry *)bp->b_data;
	putushort(denp[0].deStartCluster, newcluster);
	putushort(denp[0].deDate, ndirent.de_Date);
	putushort(denp[0].deTime, ndirent.de_Time);
	putushort(denp[1].deStartCluster, pdep->de_StartCluster);
	putushort(denp[1].deDate, ndirent.de_Date);
	putushort(denp[1].deTime, ndirent.de_Time);
	if (error = bwrite(bp))
		goto bad;

	/*
	 * Now build up a directory entry pointing to the newly allocated
	 * cluster.  This will be written to an empty slot in the parent
	 * directory.
	 */
#ifdef DIAGNOSTIC
	if ((cnp->cn_flags & HASBUF) == 0)
		panic("msdosfs_mkdir: no name");
#endif
	unix2dosfn((u_char *)cnp->cn_nameptr, ndirent.de_Name, cnp->cn_namelen);
	ndirent.de_Attributes = ATTR_DIRECTORY;
	ndirent.de_StartCluster = newcluster;
	ndirent.de_FileSize = 0;
	ndirent.de_dev = pdep->de_dev;
	ndirent.de_devvp = pdep->de_devvp;
	if (error = createde(&ndirent, pdep, &dep))
		goto bad;
	if ((cnp->cn_flags & SAVESTART) == 0)
		FREE(cnp->cn_pnbuf, M_NAMEI);
	vput(ap->a_dvp);
	*ap->a_vpp = DETOV(dep);
	return (0);

bad:
	clusterfree(pmp, newcluster, NULL);
bad2:
	FREE(cnp->cn_pnbuf, M_NAMEI);
	vput(ap->a_dvp);
	return (error);
}

int
msdosfs_rmdir(ap)
	struct vop_rmdir_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
	register struct vnode *dvp = ap->a_dvp;
	register struct componentname *cnp = ap->a_cnp;
	register struct denode *ip, *dp;
	int error;

	ip = VTODE(vp);
	dp = VTODE(dvp);
	/*
	 * No rmdir "." please.
	 */
	if (dp == ip) {
		vrele(dvp);
		vput(vp);
		return (EINVAL);
	}
	/*
	 * Verify the directory is empty (and valid).
	 * (Rmdir ".." won't be valid since
	 *  ".." will contain a reference to
	 *  the current directory and thus be
	 *  non-empty.)
	 */
	error = 0;
	if (!dosdirempty(ip)) {
		error = ENOTEMPTY;
		goto out;
	}
	/*
	 * Delete the entry from the directory.  For dos filesystems this
	 * gets rid of the directory entry on disk, the in memory copy
	 * still exists but the de_refcnt is <= 0.  This prevents it from
	 * being found by deget().  When the vput() on dep is done we give
	 * up access and eventually msdosfs_reclaim() will be called which
	 * will remove it from the denode cache.
	 */
	if (error = removede(dp, ip))
		goto out;
	/*
	 * This is where we decrement the link count in the parent
	 * directory.  Since dos filesystems don't do this we just purge
	 * the name cache and let go of the parent directory denode.
	 */
	cache_purge(dvp);
	vput(dvp);
	dvp = NULL;
	/*
	 * Truncate the directory that is being deleted.
	 */
	error = detrunc(ip, (u_long)0, IO_SYNC, cnp->cn_cred, cnp->cn_proc);
	cache_purge(vp);
out:
	if (dvp)
		vput(dvp);
	vput(vp);
	return (error);
}

/*
 * DOS filesystems don't know what symlinks are.
 */
int
msdosfs_symlink(ap)
	struct vop_symlink_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
		char *a_target;
	} */ *ap;
{
	register struct vnode *dvp = ap->a_dvp;
	register struct componentname *cnp = ap->a_cnp;

	FREE(cnp->cn_pnbuf, M_NAMEI);
	vput(dvp);
	return (EINVAL);
}

int
msdosfs_readdir(ap)
	struct vop_readdir_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
		int *a_eofflag;
		u_long *a_cookies;
		int a_ncookies;
	} */ *ap;
{
	int error = 0;
	int diff;
	long n;
	long on;
	long lost;
	long count;
	u_long cn;
	u_long fileno;
	long bias = 0;
	daddr_t bn, lbn;
	struct buf *bp;
	struct denode *dep = VTODE(ap->a_vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct direntry *dentp;
	struct dirent dirbuf;
	int i = 0;
	struct uio *uio = ap->a_uio;
	u_long *cookies;
	int ncookies;
	off_t offset;

#ifdef MSDOSFS_DEBUG
	printf("msdosfs_readdir(): vp %08x, uio %08x, cred %08x, eofflagp %08x\n",
	       ap->a_vp, uio, ap->a_cred, ap->a_eofflag);
#endif

	/*
	 * msdosfs_readdir() won't operate properly on regular files since
	 * it does i/o only with the the filesystem vnode, and hence can
	 * retrieve the wrong block from the buffer cache for a plain file.
	 * So, fail attempts to readdir() on a plain file.
	 */
	if ((dep->de_Attributes & ATTR_DIRECTORY) == 0)
		return (ENOTDIR);

	/*
	 * If the user buffer is smaller than the size of one dos directory
	 * entry or the file offset is not a multiple of the size of a
	 * directory entry, then we fail the read.
	 */
	count = uio->uio_resid & ~(sizeof(struct direntry) - 1);
	offset = uio->uio_offset;
	if (count < sizeof(struct direntry) ||
	    (offset & (sizeof(struct direntry) - 1)))
		return (EINVAL);
	lost = uio->uio_resid - count;
	uio->uio_resid = count;

	cookies = ap->a_cookies;
	ncookies = ap->a_ncookies;
	
	/*
	 * If they are reading from the root directory then, we simulate
	 * the . and .. entries since these don't exist in the root
	 * directory.  We also set the offset bias to make up for having to
	 * simulate these entries. By this I mean that at file offset 64 we
	 * read the first entry in the root directory that lives on disk.
	 */
	if (dep->de_StartCluster == MSDOSFSROOT) {
		/*
		 * printf("msdosfs_readdir(): going after . or .. in root dir, offset %d\n",
		 *	  offset);
		 */
		bias = 2 * sizeof(struct direntry);
		if (offset < bias) {
			for (n = offset / sizeof(struct direntry);
			     n < 2; n++) {
				dirbuf.d_fileno = 1;
				dirbuf.d_type = DT_DIR;
				switch (n) {
				case 0:
					dirbuf.d_namlen = 1;
					strcpy(dirbuf.d_name, ".");
					break;
				case 1:
					dirbuf.d_namlen = 2;
					strcpy(dirbuf.d_name, "..");
					break;
				}
				dirbuf.d_reclen = DIRSIZ(&dirbuf);
				if (uio->uio_resid < dirbuf.d_reclen)
					goto out;
				if (error = uiomove(&dirbuf, dirbuf.d_reclen, uio))
					goto out;
				offset += sizeof(struct direntry);
				if (cookies) {
					*cookies++ = offset;
					if (!--ncookies)
						goto out;
				}
			}
		}
	}

	while (uio->uio_resid > 0) {
		lbn = (offset - bias) >> pmp->pm_cnshift;
		on = (offset - bias) & pmp->pm_crbomask;
		n = min(pmp->pm_bpcluster - on, uio->uio_resid);
		diff = dep->de_FileSize - (offset - bias);
		if (diff <= 0)
			break;
		n = min(n, diff);
		if (error = pcbmap(dep, lbn, &bn, &cn))
			break;
		if (error = bread(pmp->pm_devvp, bn, pmp->pm_bpcluster, NOCRED, &bp)) {
			brelse(bp);
			return (error);
		}
		n = min(n, pmp->pm_bpcluster - bp->b_resid);

		/*
		 * Convert from dos directory entries to fs-independent
		 * directory entries.
		 */
		for (dentp = (struct direntry *)(bp->b_data + on);
		     (char *)dentp < bp->b_data + on + n;
		     dentp++) {
			/*
			 * printf("rd: dentp %08x prev %08x crnt %08x deName %02x attr %02x\n",
			 *	  dentp, prev, crnt, dentp->deName[0], dentp->deAttributes);
			 */
			/*
			 * If this is an unused entry, we can stop.
			 */
			if (dentp->deName[0] == SLOT_EMPTY) {
				brelse(bp);
				goto out;
			}
			/*
			 * Skip deleted entries and volume labels.
			 */
			if (dentp->deName[0] == SLOT_DELETED ||
			    (dentp->deAttributes & ATTR_VOLUME)) {
				offset += sizeof(struct direntry);
				continue;
			}
			/*
			 * This computation of d_fileno must match
			 * the computation of va_fileid in
			 * msdosfs_getattr.
			 */
			if (dentp->deAttributes & ATTR_DIRECTORY) {
				/* if this is the root directory */
				fileno = getushort(dentp->deStartCluster);
				if (fileno == MSDOSFSROOT)
					fileno = 1;
			} else {
				/*
				 * If the file's dirent lives in
				 * root dir.
				 */
				if ((fileno = cn) == MSDOSFSROOT)
					fileno = 1;
				fileno = (fileno << 16) |
				    ((dentp - (struct direntry *)bp->b_data) & 0xffff);
			}
			dirbuf.d_fileno = fileno;
			dirbuf.d_type =
			    (dentp->deAttributes & ATTR_DIRECTORY) ? DT_DIR : DT_REG;
			dirbuf.d_namlen = dos2unixfn(dentp->deName,
						     (u_char *)dirbuf.d_name);
			dirbuf.d_reclen = DIRSIZ(&dirbuf);
			if (uio->uio_resid < dirbuf.d_reclen) {
				brelse(bp);
				goto out;
			}
			if (error = uiomove(&dirbuf, dirbuf.d_reclen, uio)) {
				brelse(bp);
				goto out;
			}
			offset += sizeof(struct direntry);
			if (cookies) {
				*cookies++ = offset;
				if (--ncookies) {
					brelse(bp);
					goto out;
				}
			}
		}

#if 0
		/*
		 * If we have read everything from this block or have read
		 * to end of file then we are done with this block.  Mark
		 * it to say the buffer can be reused if need be.
		 */
		if (n + on == pmp->pm_bpcluster ||
		    dep->de_FileSize - (offset - bias) == 0)
			bp->b_flags |= B_AGE;
#endif /* if 0 */
		brelse(bp);
	}

out:
	uio->uio_offset = offset;
	uio->uio_resid += lost;
	if (dep->de_FileSize - (offset - bias) <= 0)
		*ap->a_eofflag = 1;
	else
		*ap->a_eofflag = 0;
	return (error);
}

/*
 * DOS filesystems don't know what symlinks are.
 */
int
msdosfs_readlink(ap)
	struct vop_readlink_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
	} */ *ap;
{

	return (EINVAL);
}

int
msdosfs_abortop(ap)
	struct vop_abortop_args /* {
		struct vnode *a_dvp;
		struct componentname *a_cnp;
	} */ *ap;
{

	if ((ap->a_cnp->cn_flags & (HASBUF | SAVESTART)) == HASBUF)
		FREE(ap->a_cnp->cn_pnbuf, M_NAMEI);
	return (0);
}

int
msdosfs_lock(ap)
	struct vop_lock_args /* {
		struct vnode *a_vp;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
	register struct denode *dep;
	struct proc *p = curproc;	/* XXX */

start:
	while (vp->v_flag & VXLOCK) {
		vp->v_flag |= VXWANT;
		sleep((caddr_t)vp, PINOD);
	}
	if (vp->v_tag == VT_NON)
		return (ENOENT);
	dep = VTODE(vp);
	if (dep->de_flag & DE_LOCKED) {
		dep->de_flag |= DE_WANTED;
#ifdef DIAGNOSTIC
		if (p) {
			if (p->p_pid == dep->de_lockholder)
				panic("locking against myself");
			dep->de_lockwaiter = p->p_pid;
		} else
			dep->de_lockwaiter = -1;
#endif
		(void) sleep((caddr_t)dep, PINOD);
		goto start;
	}
#ifdef DIAGNOSTIC
	dep->de_lockwaiter = 0;
	if (dep->de_lockholder != 0)
		panic("lockholder (%d) != 0", dep->de_lockholder);
	if (p && p->p_pid == 0)
		printf("locking by process 0\n");
	if (p)
		dep->de_lockholder = p->p_pid;
	else
		dep->de_lockholder = -1;
#endif
	dep->de_flag |= DE_LOCKED;
	return (0);
}

int
msdosfs_unlock(ap)
	struct vop_unlock_args /* {
		struct vnode *vp;
	} */ *ap;
{
	register struct denode *dep = VTODE(ap->a_vp);
	struct proc *p = curproc;	/* XXX */

#ifdef DIAGNOSTIC
	if ((dep->de_flag & DE_LOCKED) == 0) {
		vprint("msdosfs_unlock: unlocked denode", ap->a_vp);
		panic("msdosfs_unlock NOT LOCKED");
	}
	if (p && p->p_pid != dep->de_lockholder && p->p_pid > -1 &&
	    dep->de_lockholder > -1/* && lockcount++ < 100*/)
		panic("unlocker (%d) != lock holder (%d)",
		    p->p_pid, dep->de_lockholder);
	dep->de_lockholder = 0;
#endif
	dep->de_flag &= ~DE_LOCKED;
	if (dep->de_flag & DE_WANTED) {
		dep->de_flag &= ~DE_WANTED;
		wakeup((caddr_t)dep);
	}
	return (0);
}

int
msdosfs_islocked(ap)
	struct vop_islocked_args /* {
		struct vnode *a_vp;
	} */ *ap;
{

	if (VTODE(ap->a_vp)->de_flag & DE_LOCKED)
		return (1);
	return (0);
}

/*
 * vp  - address of vnode file the file
 * bn  - which cluster we are interested in mapping to a filesystem block number.
 * vpp - returns the vnode for the block special file holding the filesystem
 *	 containing the file of interest
 * bnp - address of where to return the filesystem relative block number
 */
int
msdosfs_bmap(ap)
	struct vop_bmap_args /* {
		struct vnode *a_vp;
		daddr_t a_bn;
		struct vnode **a_vpp;
		daddr_t *a_bnp;
		int *a_runp;
	} */ *ap;
{
	struct denode *dep = VTODE(ap->a_vp);
	struct msdosfsmount *pmp = dep->de_pmp;

	if (ap->a_vpp != NULL)
		*ap->a_vpp = dep->de_devvp;
	if (ap->a_bnp == NULL)
		return (0);
	if (ap->a_runp) {
		/*
		 * Sequential clusters should be counted here.
		 */
		*ap->a_runp = 0;
	}
	return (pcbmap(dep, ap->a_bn << (pmp->pm_cnshift - pmp->pm_bnshift),
		       ap->a_bnp, 0));
}

int
msdosfs_reallocblks(ap)
	struct vop_reallocblks_args /* {
		struct vnode *a_vp;
		struct cluster_save *a_buflist;
	} */ *ap;
{

	/* Currently no support for clustering */		/* XXX */
	return (ENOSPC);
}

int
msdosfs_strategy(ap)
	struct vop_strategy_args /* {
		struct buf *a_bp;
	} */ *ap;
{
	struct buf *bp = ap->a_bp;
	struct denode *dep = VTODE(bp->b_vp);
	struct msdosfsmount *pmp = dep->de_pmp;
	struct vnode *vp;
	int error = 0;

	if (bp->b_vp->v_type == VBLK || bp->b_vp->v_type == VCHR)
		panic("msdosfs_strategy: spec");
	/*
	 * If we don't already know the filesystem relative block number
	 * then get it using pcbmap().  If pcbmap() returns the block
	 * number as -1 then we've got a hole in the file.  DOS filesystems
	 * don't allow files with holes, so we shouldn't ever see this.
	 */
	if (bp->b_blkno == bp->b_lblkno) {
		if (error = pcbmap(dep, bp->b_lblkno, &bp->b_blkno, 0))
			bp->b_blkno = -1;
		if (bp->b_blkno == -1)
			clrbuf(bp);
	}
	if (bp->b_blkno == -1) {
		biodone(bp);
		return (error);
	}
#ifdef DIAGNOSTIC
#endif
	/*
	 * Read/write the block from/to the disk that contains the desired
	 * file block.
	 */
	vp = dep->de_devvp;
	bp->b_dev = vp->v_rdev;
	VOCALL(vp->v_op, VOFFSET(vop_strategy), ap);
	return (0);
}

int
msdosfs_print(ap)
	struct vop_print_args /* {
		struct vnode *vp;
	} */ *ap;
{
	struct denode *dep = VTODE(ap->a_vp);

	printf("tag VT_MSDOSFS, startcluster %d, dircluster %d, diroffset %d ",
	       dep->de_StartCluster, dep->de_dirclust, dep->de_diroffset);
	printf(" dev %d, %d, %s\n",
	       major(dep->de_dev), minor(dep->de_dev),
	       dep->de_flag & DE_LOCKED ? "(LOCKED)" : "");
	if (dep->de_lockholder) {
		printf("    owner pid %d", dep->de_lockholder);
		if (dep->de_lockwaiter)
			printf(" waiting pid %d", dep->de_lockwaiter);
		printf("\n");
	}
}

int
msdosfs_advlock(ap)
	struct vop_advlock_args /* {
		struct vnode *a_vp;
		caddr_t a_id;
		int a_op;
		struct flock *a_fl;
		int a_flags;
	} */ *ap;
{

	return (EINVAL);		/* we don't do locking yet */
}

int
msdosfs_pathconf(ap)
	struct vop_pathconf_args /* {
		struct vnode *a_vp;
		int a_name;
		register_t *a_retval;
	} */ *ap;
{

	switch (ap->a_name) {
	case _PC_LINK_MAX:
		*ap->a_retval = 1;
		return (0);
	case _PC_NAME_MAX:
		*ap->a_retval = 12;
		return (0);
	case _PC_PATH_MAX:
		*ap->a_retval = PATH_MAX;
		return (0);
	case _PC_CHOWN_RESTRICTED:
		*ap->a_retval = 1;
		return (0);
	case _PC_NO_TRUNC:
		*ap->a_retval = 0;
		return (0);
	default:
		return (EINVAL);
	}
	/* NOTREACHED */
}

/* Global vfs data structures for msdosfs */
int (**msdosfs_vnodeop_p)();
struct vnodeopv_entry_desc msdosfs_vnodeop_entries[] = {
	{ &vop_default_desc, vn_default_error },
	{ &vop_lookup_desc, msdosfs_lookup },		/* lookup */
	{ &vop_create_desc, msdosfs_create },		/* create */
	{ &vop_mknod_desc, msdosfs_mknod },		/* mknod */
	{ &vop_open_desc, msdosfs_open },		/* open */
	{ &vop_close_desc, msdosfs_close },		/* close */
	{ &vop_access_desc, msdosfs_access },		/* access */
	{ &vop_getattr_desc, msdosfs_getattr },		/* getattr */
	{ &vop_setattr_desc, msdosfs_setattr },		/* setattr */
	{ &vop_read_desc, msdosfs_read },		/* read */
	{ &vop_write_desc, msdosfs_write },		/* write */
	{ &vop_lease_desc, msdosfs_lease_check },	/* lease */
	{ &vop_ioctl_desc, msdosfs_ioctl },		/* ioctl */
	{ &vop_select_desc, msdosfs_select },		/* select */
	{ &vop_mmap_desc, msdosfs_mmap },		/* mmap */
	{ &vop_fsync_desc, msdosfs_fsync },		/* fsync */
	{ &vop_seek_desc, msdosfs_seek },		/* seek */
	{ &vop_remove_desc, msdosfs_remove },		/* remove */
	{ &vop_link_desc, msdosfs_link },		/* link */
	{ &vop_rename_desc, msdosfs_rename },		/* rename */
	{ &vop_mkdir_desc, msdosfs_mkdir },		/* mkdir */
	{ &vop_rmdir_desc, msdosfs_rmdir },		/* rmdir */
	{ &vop_symlink_desc, msdosfs_symlink },		/* symlink */
	{ &vop_readdir_desc, msdosfs_readdir },		/* readdir */
	{ &vop_readlink_desc, msdosfs_readlink },	/* readlink */
	{ &vop_abortop_desc, msdosfs_abortop },		/* abortop */
	{ &vop_inactive_desc, msdosfs_inactive },	/* inactive */
	{ &vop_reclaim_desc, msdosfs_reclaim },		/* reclaim */
	{ &vop_lock_desc, msdosfs_lock },		/* lock */
	{ &vop_unlock_desc, msdosfs_unlock },		/* unlock */
	{ &vop_bmap_desc, msdosfs_bmap },		/* bmap */
	{ &vop_strategy_desc, msdosfs_strategy },	/* strategy */
	{ &vop_print_desc, msdosfs_print },		/* print */
	{ &vop_islocked_desc, msdosfs_islocked },	/* islocked */
	{ &vop_pathconf_desc, msdosfs_pathconf },	/* pathconf */
	{ &vop_advlock_desc, msdosfs_advlock },		/* advlock */
	{ &vop_reallocblks_desc, msdosfs_reallocblks },	/* reallocblks */
	{ &vop_bwrite_desc, vn_bwrite },
	{ (struct vnodeop_desc *)NULL, (int (*)())NULL }
};
struct vnodeopv_desc msdosfs_vnodeop_opv_desc =
	{ &msdosfs_vnodeop_p, msdosfs_vnodeop_entries };
