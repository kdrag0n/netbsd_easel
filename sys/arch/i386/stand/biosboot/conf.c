/*	$NetBSD: conf.c,v 1.7 2003/04/11 11:36:28 dsl Exp $	 */

/*
 * Copyright (c) 1997
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
 */


#include <sys/types.h>

#include <lib/libsa/stand.h>
#include <lib/libsa/ufs.h>
#include <lib/libsa/lfs.h>
#ifdef SUPPORT_USTARFS
#include <lib/libsa/ustarfs.h>
#endif
#ifdef SUPPORT_DOSFS
#include <lib/libsa/dosfs.h>
#endif

#include <biosdisk.h>

struct devsw devsw[] = {
	{"disk", biosdiskstrategy, biosdiskopen, biosdiskclose, biosdiskioctl},
};
int ndevs = sizeof(devsw) / sizeof(struct devsw);

struct fs_ops file_system[] = {
#ifdef SUPPORT_USTARFS
	FS_OPS(ustarfs),
#endif
	FS_OPS(ffsv2),
	FS_OPS(ffsv1),
	FS_OPS(lfsv1),
	FS_OPS(lfsv2),
#ifdef SUPPORT_DOSFS
	FS_OPS(dosfs),
#endif
};
int nfsys = sizeof(file_system) / sizeof(struct fs_ops);
