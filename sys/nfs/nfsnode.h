/*	 $NetBSD: nfsnode.h,v 1.35 2002/10/21 12:52:36 yamt Exp $	*/

/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	@(#)nfsnode.h	8.9 (Berkeley) 5/14/95
 */


#ifndef _NFS_NFSNODE_H_
#define _NFS_NFSNODE_H_

#ifndef _NFS_NFS_H_
#include <nfs/nfs.h>
#endif
#include <miscfs/genfs/genfs.h>
#include <miscfs/genfs/genfs_node.h>

/*
 * Silly rename structure that hangs off the nfsnode until the name
 * can be removed by nfs_inactive()
 */
struct sillyrename {
	struct	ucred *s_cred;
	struct	vnode *s_dvp;
	long	s_namlen;
	char	s_name[20];
};

/*
 * Definitions for the directory cache. Because directory cookies
 * are an opaque 64 bit entity, we need to provide some sort of
 * mapping between cookies and logical blocknumbers. Also,
 * we should store the cookies from the server somewhere,
 * to be able to satisfy VOP_READDIR requests for cookies.
 * We can't store the cookies in the dirent structure, as some
 * other systems.
 *
 * Each offset is hashed into a per-nfsnode hashtable. An entry
 * found therein contains information about the (faked up)
 * logical blocknumber, and also a pointer to a buffer where
 * the cookies are stored.
 */

extern u_long nfsdirhashmask;

LIST_HEAD(nfsdirhashhead, nfsdircache);
TAILQ_HEAD(nfsdirchainhead, nfsdircache);

struct nfsdircache {
	off_t		dc_cookie;		/* Own offset (key) */
	off_t		dc_blkcookie;		/* Offset of block we're in */
	LIST_ENTRY(nfsdircache) dc_hash;	/* Hash chain */
	TAILQ_ENTRY(nfsdircache) dc_chain;	/* Least recently entered chn */
	u_int32_t	dc_cookie32;		/* Key for 64<->32 xlate case */
	daddr_t		dc_blkno;		/* Number of block we're in */
	int		dc_entry;		/* Entry number within block */
};


/*
 * The nfsnode is the nfs equivalent to ufs's inode. Any similarity
 * is purely coincidental.
 * There is a unique nfsnode allocated for each active file,
 * each current directory, each mounted-on file, text file, and the root.
 * An nfsnode is 'named' by its file handle. (nget/nfs_node.c)
 */
struct nfsnode {
	struct genfs_node	n_gnode;
	u_quad_t		n_size;		/* Current size of file */
	u_quad_t		n_brev;		/* Modify rev when cached */
	u_quad_t		n_lrev;		/* Modify rev for lease */
	union {
		struct timespec	nf_mtim;
		off_t		nd_direof;	/* Dir. EOF offset cache */
	} n_un2;
	union {
		struct timespec	nf_atim;	/* Special file times */
		nfsuint64	nd_cookieverf;	/* Cookie verifier (dir only) */
	} n_un1;
	union {
		struct sillyrename *nf_silly;	/* Ptr to silly rename struct */
		struct nfsdirhashhead *nd_dircache;
	} n_un3;
	LIST_ENTRY(nfsnode)	n_hash;		/* Hash chain */
	CIRCLEQ_ENTRY(nfsnode)	n_timer;	/* Nqnfs timer chain */
	struct nfsdirchainhead	n_dirchain;	/* Chain of dir cookies */
	nfsfh_t			*n_fhp;		/* NFS File Handle */
	struct vattr		*n_vattr;	/* Vnode attribute cache */
	struct vnode		*n_vnode;	/* associated vnode */
	struct lockf		*n_lockf;	/* Locking record of file */
	unsigned		*n_dirgens;	/* 32<->64bit xlate gen. no. */
	time_t			n_attrstamp;	/* Attr. cache timestamp */
	time_t			n_mtime;	/* Prev modify time. */
	time_t			n_ctime;	/* Prev create time. */
	time_t			n_nctime;	/* Last neg cache entry (dir) */
	time_t			n_expiry;	/* Lease expiry time */
	daddr_t			n_dblkno;	/* To keep faked dir blkno */
	unsigned		n_dircachesize;	/* Size of dir cookie cache */
	int			n_error;	/* Save write error value */
	short			n_fhsize;	/* size in bytes, of fh */
	short			n_flag;		/* Flag for locking.. */
	nfsfh_t			n_fh;		/* Small File Handle */
	time_t			n_accstamp;	/* Access cache timestamp */
	uid_t			n_accuid;	/* Last access requester */
	int			n_accmode;	/* Mode last requested */
	int			n_accerror;	/* Error last returned */
	off_t			n_pushedlo;	/* 1st blk in commited range */
	off_t			n_pushedhi;	/* Last block in range */
	off_t			n_pushlo;	/* 1st block in commit range */
	off_t			n_pushhi;	/* Last block in range */
	struct lock		n_commitlock;	/* Serialize commits XXX */
	int			n_commitflags;
	struct ucred		*n_rcred;
	struct ucred		*n_wcred;
};

/*
 * Values for n_commitflags
 */
#define NFS_COMMIT_PUSH_VALID	0x0001		/* push range valid */
#define NFS_COMMIT_PUSHED_VALID	0x0002		/* pushed range valid */

#define n_atim		n_un1.nf_atim
#define n_mtim		n_un2.nf_mtim
#define n_sillyrename	n_un3.nf_silly
#define n_cookieverf	n_un1.nd_cookieverf
#define n_direofoffset	n_un2.nd_direof
#define n_dircache	n_un3.nd_dircache

/*
 * Flags for n_flag
 */
#define	NFLUSHWANT	0x0001	/* Want wakeup from a flush in prog. */
#define	NFLUSHINPROG	0x0002	/* Avoid multiple calls to vinvalbuf() */
#define	NMODIFIED	0x0004	/* Might have a modified buffer in bio */
#define	NWRITEERR	0x0008	/* Flag write errors so close will know */
#define	NQNFSNONCACHE	0x0020	/* Non-cachable lease */
#define	NQNFSWRITE	0x0040	/* Write lease */
#define	NQNFSEVICTED	0x0080	/* Has been evicted */
#define	NACC		0x0100	/* Special file accessed */
#define	NUPD		0x0200	/* Special file updated */
#define	NCHG		0x0400	/* Special file times changed */
#define	NTRUNCDELAYED	0x1000	/* Should be truncated later */

/*
 * Convert between nfsnode pointers and vnode pointers
 */
#define VTONFS(vp)	((struct nfsnode *)(vp)->v_data)
#define NFSTOV(np)	((np)->n_vnode)

/*
 * Queue head for nfsiod's
 */
extern struct proc *nfs_iodwant[NFS_MAXASYNCDAEMON];
extern struct nfsmount *nfs_iodmount[NFS_MAXASYNCDAEMON];

#ifdef _KERNEL
/*
 * Prototypes for NFS vnode operations
 */
int	nfs_lookup	__P((void *));
int	nfs_create	__P((void *));
int	nfs_mknod	__P((void *));
int	nfs_open	__P((void *));
int	nfs_close	__P((void *));
int	nfsspec_close	__P((void *));
int	nfsfifo_close	__P((void *));
int	nfs_access	__P((void *));
int	nfsspec_access	__P((void *));
int	nfs_getattr	__P((void *));
int	nfs_setattr	__P((void *));
int	nfs_read	__P((void *));
int	nfs_write	__P((void *));
#define	nfs_lease_check	genfs_nullop
int	nfsspec_read	__P((void *));
int	nfsspec_write	__P((void *));
int	nfsfifo_read	__P((void *));
int	nfsfifo_write	__P((void *));
#define	nfs_ioctl	genfs_enoioctl
#define	nfs_poll	genfs_poll
#define nfs_revoke	genfs_revoke
#define	nfs_mmap	genfs_mmap
int	nfs_fsync	__P((void *));
#define nfs_seek	genfs_seek
int	nfs_remove	__P((void *));
int	nfs_link	__P((void *));
int	nfs_rename	__P((void *));
int	nfs_mkdir	__P((void *));
int	nfs_rmdir	__P((void *));
int	nfs_symlink	__P((void *));
int	nfs_readdir	__P((void *));
int	nfs_readlink	__P((void *));
#define	nfs_abortop	genfs_abortop
int	nfs_inactive	__P((void *));
int	nfs_reclaim	__P((void *));
#define nfs_lock	genfs_lock
int nfs_unlock	__P((void *));
#define nfs_islocked	genfs_islocked
int	nfs_bmap	__P((void *));
int	nfs_strategy	__P((void *));
int	nfs_print	__P((void *));
int	nfs_pathconf	__P((void *));
int	nfs_advlock	__P((void *));
#define	nfs_blkatoff	genfs_eopnotsupp
int	nfs_bwrite	__P((void *));
#define	nfs_valloc	genfs_eopnotsupp
#define nfs_reallocblks	genfs_eopnotsupp
#define	nfs_vfree	genfs_nullop
int	nfs_truncate	__P((void *));
int	nfs_update	__P((void *));
int	nfs_getpages	__P((void *));
int	nfs_putpages	__P((void *));
int	nfs_gop_write(struct vnode *, struct vm_page **, int, int);

extern int (**nfsv2_vnodeop_p) __P((void *));

#endif /* _KERNEL */

#endif
