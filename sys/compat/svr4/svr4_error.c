/*	$NetBSD: svr4_error.c,v 1.1 1994/11/14 06:13:12 christos Exp $	 */

/*
 * Copyright (c) 1994 Christos Zoulas
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

/*
 * Translate error codes.
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/file.h>
#include <sys/filedesc.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/device.h>
#include <sys/errno.h>


#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_util.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/svr4/svr4_errno.h>


int svr4_error[] = {
	0,
	SVR4_EPERM,
	SVR4_ENOENT,
	SVR4_ESRCH,
	SVR4_EINTR,
	SVR4_EIO,
	SVR4_ENXIO,
	SVR4_E2BIG,
	SVR4_ENOEXEC,
	SVR4_EBADF,
	SVR4_ECHILD,
	SVR4_EDEADLK,
	SVR4_ENOMEM,
	SVR4_EACCES,
	SVR4_EFAULT,
	SVR4_ENOTBLK,
	SVR4_EBUSY,
	SVR4_EEXIST,
	SVR4_EXDEV,
	SVR4_ENODEV,
	SVR4_ENOTDIR,
	SVR4_EISDIR,
	SVR4_EINVAL,
	SVR4_ENFILE,
	SVR4_EMFILE,
	SVR4_ENOTTY,
	SVR4_ETXTBSY,
	SVR4_EFBIG,
	SVR4_ENOSPC,
	SVR4_ESPIPE,
	SVR4_EROFS,
	SVR4_EMLINK,
	SVR4_EPIPE,
	SVR4_EDOM,
	SVR4_ERANGE,
	SVR4_EAGAIN,
	SVR4_EINPROGRESS,
	SVR4_EALREADY,
	SVR4_ENOTSOCK,
	SVR4_EDESTADDRREQ,
	SVR4_EMSGSIZE,
	SVR4_EPROTOTYPE,
	SVR4_ENOPROTOOPT,
	SVR4_EPROTONOSUPPORT,
	SVR4_ESOCKTNOSUPPORT,
	SVR4_EOPNOTSUPP,
	SVR4_EPFNOSUPPORT,
	SVR4_EAFNOSUPPORT,
	SVR4_EADDRINUSE,
	SVR4_EADDRNOTAVAIL,
	SVR4_ENETDOWN,
	SVR4_ENETUNREACH,
	SVR4_ENETRESET,
	SVR4_ECONNABORTED,
	SVR4_ECONNRESET,
	SVR4_ENOBUFS,
	SVR4_EISCONN,
	SVR4_ENOTCONN,
	SVR4_ESHUTDOWN,
	SVR4_ETOOMANYREFS,
	SVR4_ETIMEDOUT,
	SVR4_ECONNREFUSED,
	SVR4_ELOOP,
	SVR4_ENAMETOOLONG,
	SVR4_EHOSTDOWN,
	SVR4_EHOSTUNREACH,
	SVR4_ENOTEMPTY,
	SVR4_EPROCLIM,
	SVR4_EUSERS,
	SVR4_EDQUOT,
	SVR4_ESTALE,
	SVR4_EREMOTE,
	SVR4_EBADRPC,
	SVR4_ERPCMISMATCH,
	SVR4_EPROGUNAVAIL,
	SVR4_EPROGMISMATCH,
	SVR4_EPROCUNAVAIL,
	SVR4_ENOLCK,
	SVR4_ENOSYS,
	SVR4_EFTYPE,
	SVR4_EAUTH,
	SVR4_ENEEDAUTH,
};
