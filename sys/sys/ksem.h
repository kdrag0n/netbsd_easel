/*	$NetBSD: ksem.h,v 1.8 2008/10/22 11:49:23 ad Exp $	*/

/*
 * Copyright (c) 2002 Alfred Perlstein <alfred@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYS_KSEM_H_
#define _SYS_KSEM_H_

#include <sys/cdefs.h>

#ifdef _KERNEL
int do_ksem_init(struct lwp *, unsigned int, semid_t *, copyout_t);
int do_ksem_open(struct lwp *, const char *, int, mode_t, unsigned int,
    semid_t *, copyout_t);

extern int	posix_semaphores;
#endif

#ifdef _LIBC
__BEGIN_DECLS
int _ksem_close(semid_t);
int _ksem_destroy(semid_t);
int _ksem_getvalue(semid_t, int *);
int _ksem_init(unsigned int, semid_t *);
int _ksem_open(const char *, int, mode_t, unsigned int, semid_t *);
int _ksem_post(semid_t);
int _ksem_trywait(semid_t);
int _ksem_unlink(const char *);
int _ksem_wait(semid_t);
__END_DECLS
#endif /* _LIBC */

#endif /* _SYS_KSEM_H_ */
