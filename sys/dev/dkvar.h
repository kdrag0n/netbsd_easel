/* $NetBSD: dkvar.h,v 1.3 2003/06/29 22:29:59 fvdl Exp $ */

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Roland C. Dowdeswell.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

struct dk_geom {
	u_int32_t	pdg_secsize;
	u_int32_t	pdg_nsectors;
	u_int32_t	pdg_ntracks;
	u_int32_t	pdg_ncylinders;
};

/* literally this is not a softc, but is intended to be included in
 * the pseudo-disk's softc and passed to calls in dksubr.c.  It
 * should include the common elements of the pseudo-disk's softc.
 * All elements that are included here should describe the external
 * representation of the disk to the higher layers, and flags that
 * are common to each of the pseudo-disk drivers.
 */
struct dk_softc {
	void			*sc_osc;	/* the softc of the underlying
						 * driver */
	u_int32_t		 sc_flags;	/* flags */
	size_t			 sc_size;	/* size of disk */
	struct dk_geom	 sc_geom;		/* geometry info */
#define DK_XNAME_SIZE 8
	char			 sc_xname[DK_XNAME_SIZE]; /* external name */
	struct disk		 sc_dkdev;	/* generic disk info */
	struct lock		 sc_lock;	/* the lock */
};

/* sc_flags:
 *   We separate the flags into two varieties, those that dksubr.c
 *   understands and manipulates and those that it does not.
 */

#define DKF_INITED	0x00010000 /* unit has been initialised */
#define DKF_WLABEL	0x00020000 /* label area is writable */
#define DKF_LABELLING	0x00040000 /* unit is currently being labeled */
#define DKF_WARNLABEL	0x00080000 /* warn if disklabel not present */
#define DKF_LABELSANITY	0x00100000 /* warn if disklabel not sane */
#define DKF_TAKEDUMP	0x00200000 /* allow dumping */

/* Mask of flags that dksubr.c understands, other flags are fair game */
#define DK_FLAGMASK	0xffff0000

/*
 * This defines the interface to the routines in dksubr.c.  This
 * should be a single static structure per pseudo-disk driver.
 * We only define the functions that we currently need.
 */
struct dk_intf {
	int	  di_dtype;			/* disk type */
	char	 *di_dkname;			/* disk type name */
	int	(*di_open)(dev_t, int, int, struct proc *);
	int	(*di_close)(dev_t, int, int, struct proc *);
	void	(*di_strategy)(struct buf *);
	void	(*di_diskstart)(struct dk_softc *, struct buf *);
};

#define DK_BUSY(_dksc, _pmask)				\
	((_dksc)->sc_dkdev.dk_openmask & ~(_pmask)) ||	\
	((_dksc)->sc_dkdev.dk_bopenmask & (_pmask)  &&	\
	((_dksc)->sc_dkdev.dk_copenmask & (_pmask)))

/*
 * Functions that are exported to the pseudo disk implementations:
 */

void	dk_sc_init(struct dk_softc *, void *, char *);

int	dk_open(struct dk_intf *, struct dk_softc *, dev_t,
		int, int, struct proc *);
int	dk_close(struct dk_intf *, struct dk_softc *, dev_t,
		 int, int, struct proc *);
void	dk_strategy(struct dk_intf *, struct dk_softc *, struct buf *);
int	dk_size(struct dk_intf *, struct dk_softc *, dev_t);
int	dk_ioctl(struct dk_intf *, struct dk_softc *, dev_t,
		 u_long, caddr_t, int, struct proc *);
int	dk_dump(struct dk_intf *, struct dk_softc *, dev_t,
		daddr_t, caddr_t, size_t);
void	dk_getdisklabel(struct dk_intf *, struct dk_softc *, dev_t);
void	dk_getdefaultlabel(struct dk_intf *, struct dk_softc *,
			   struct disklabel *);

int	dk_lookup(char *, struct proc *, struct vnode **);
