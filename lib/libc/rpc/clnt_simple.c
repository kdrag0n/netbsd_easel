/*	$NetBSD: clnt_simple.c,v 1.22 2001/01/04 14:42:19 lukem Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
/*
 * Copyright (c) 1986-1991 by Sun Microsystems Inc. 
 */

/* #ident	"@(#)clnt_simple.c	1.17	94/04/24 SMI" */

#if 0
#if !defined(lint) && defined(SCCSIDS)
static char sccsid[] = "@(#)clnt_simple.c 1.49 89/01/31 Copyr 1984 Sun Micro";
#endif
#endif

/*
 * clnt_simple.c
 * Simplified front end to client rpc.
 *
 */

#include "namespace.h"
#include "reentrant.h"
#include <sys/param.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <rpc/rpc.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __weak_alias
__weak_alias(rpc_call,_rpc_call)
#endif

#ifndef MAXHOSTNAMELEN
#define	MAXHOSTNAMELEN 64
#endif

#ifndef NETIDLEN
#define	NETIDLEN 32
#endif

struct rpc_call_private {
	int	valid;			/* Is this entry valid ? */
	CLIENT	*client;		/* Client handle */
	pid_t	pid;			/* process-id at moment of creation */
	rpcprog_t prognum;		/* Program */
	rpcvers_t versnum;		/* Version */
	char	host[MAXHOSTNAMELEN];	/* Servers host */
	char	nettype[NETIDLEN];	/* Network type */
};
static struct rpc_call_private *rpc_call_private_main;

#ifdef __REENT
static void rpc_call_destroy __P((void *));

static void
rpc_call_destroy(void *vp)
{
	struct rpc_call_private *rcp = (struct rpc_call_private *)vp;

	if (rcp) {
		if (rcp->client)
			CLNT_DESTROY(rcp->client);
		free(rcp);
	}
}
#endif

/*
 * This is the simplified interface to the client rpc layer.
 * The client handle is not destroyed here and is reused for
 * the future calls to same prog, vers, host and nettype combination.
 *
 * The total time available is 25 seconds.
 */
enum clnt_stat
rpc_call(host, prognum, versnum, procnum, inproc, in, outproc, out, nettype)
	const char *host;			/* host name */
	rpcprog_t prognum;			/* program number */
	rpcvers_t versnum;			/* version number */
	rpcproc_t procnum;			/* procedure number */
	xdrproc_t inproc, outproc;	/* in/out XDR procedures */
	const char *in;
	char  *out;			/* recv/send data */
	const char *nettype;			/* nettype */
{
	struct rpc_call_private *rcp = (struct rpc_call_private *) 0;
	enum clnt_stat clnt_stat;
	struct timeval timeout, tottimeout;
#ifdef __REENT
	static thread_key_t rpc_call_key;
	extern mutex_t tsd_lock;
#endif
	int main_thread = 1;

	_DIAGASSERT(host != NULL);
	/* XXX: in may be NULL ??? */
	/* XXX: out may be NULL ??? */
	/* XXX: nettype may be NULL ??? */

#ifdef __REENT
	if ((main_thread = _thr_main())) {
		rcp = rpc_call_private_main;
	} else {
		if (rpc_call_key == 0) {
			mutex_lock(&tsd_lock);
			if (rpc_call_key == 0)
				thr_keycreate(&rpc_call_key, rpc_call_destroy);
			mutex_unlock(&tsd_lock);
		}
		thr_getspecific(rpc_call_key, (void **) &rcp);
	}
#else
	rcp = rpc_call_private_main;
#endif
	if (rcp == NULL) {
		rcp = malloc(sizeof (*rcp));
		if (rcp == NULL) {
			rpc_createerr.cf_stat = RPC_SYSTEMERROR;
			rpc_createerr.cf_error.re_errno = errno;
			return (rpc_createerr.cf_stat);
		}
		if (main_thread)
			rpc_call_private_main = rcp;
		else
			thr_setspecific(rpc_call_key, (void *) rcp);
		rcp->valid = 0;
		rcp->client = NULL;
	}
	if ((nettype == NULL) || (nettype[0] == NULL))
		nettype = "netpath";
	if (!(rcp->valid && rcp->pid == getpid() &&
		(rcp->prognum == prognum) &&
		(rcp->versnum == versnum) &&
		(!strcmp(rcp->host, host)) &&
		(!strcmp(rcp->nettype, nettype)))) {
		int fd;

		rcp->valid = 0;
		if (rcp->client)
			CLNT_DESTROY(rcp->client);
		/*
		 * Using the first successful transport for that type
		 */
		rcp->client = clnt_create(host, prognum, versnum, nettype);
		rcp->pid = getpid();
		if (rcp->client == NULL) {
			return (rpc_createerr.cf_stat);
		}
		/*
		 * Set time outs for connectionless case.  Do it
		 * unconditionally.  Faster than doing a t_getinfo()
		 * and then doing the right thing.
		 */
		timeout.tv_usec = 0;
		timeout.tv_sec = 5;
		(void) CLNT_CONTROL(rcp->client,
				CLSET_RETRY_TIMEOUT, (char *)(void *)&timeout);
		if (CLNT_CONTROL(rcp->client, CLGET_FD, (char *)(void *)&fd))
			fcntl(fd, F_SETFD, 1);	/* make it "close on exec" */
		rcp->prognum = prognum;
		rcp->versnum = versnum;
		if ((strlen(host) < (size_t)MAXHOSTNAMELEN) &&
		    (strlen(nettype) < (size_t)NETIDLEN)) {
			(void) strcpy(rcp->host, host);
			(void) strcpy(rcp->nettype, nettype);
			rcp->valid = 1;
		} else {
			rcp->valid = 0;
		}
	} /* else reuse old client */
	tottimeout.tv_sec = 25;
	tottimeout.tv_usec = 0;
	/*LINTED const castaway*/
	clnt_stat = CLNT_CALL(rcp->client, procnum, inproc, (char *) in,
	    outproc, out, tottimeout);
	/*
	 * if call failed, empty cache
	 */
	if (clnt_stat != RPC_SUCCESS)
		rcp->valid = 0;
	return (clnt_stat);
}
