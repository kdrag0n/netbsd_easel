/*	$NetBSD: netif_sun.c,v 1.10 1998/02/05 04:57:12 gwr Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Gordon W. Ross.
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

/*
 * The Sun PROM has a fairly general set of network drivers,
 * so it is easiest to just replace the netif module with
 * this adaptation to the PROM network interface.
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>

#include <net/if.h>
#include <net/if_ether.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>

#include <machine/mon.h>

#include <stand.h>
#include <net.h>

#include "libsa.h"
#include "dvma.h"
#include "saio.h"
#include "netif.h"

#define	PKT_BUF_SIZE 2048

int errno;

struct iodesc sockets[SOPEN_MAX];

struct netif prom_netif;
struct devdata {
	struct saioreq dd_si;
	int rbuf_len;
	char *rbuf;
	int tbuf_len;
	char *tbuf;
	u_short dd_opens;
	u_char dd_myea[6];
} netif_devdata;

void netif_getether(struct saioreq *, u_char *);


/*
 * Open the PROM device.
 * Return netif ptr on success.
 */
struct devdata *
netif_init(aux)
	void *aux;
{
	struct devdata *dd = &netif_devdata;
	struct saioreq *si;
	struct bootparam *bp;
	int error;

	/*
	 * Setup our part of the saioreq.
	 * (determines what gets opened)
	 */
	si = &dd->dd_si;
	bzero((caddr_t)si, sizeof(*si));
	bp = *romVectorPtr->bootParam;
	si->si_boottab = bp->bootDevice;
	si->si_ctlr = bp->ctlrNum;
	si->si_unit = bp->unitNum;
	si->si_boff = bp->partNum;

#ifdef NETIF_DEBUG
	if (debug)
		printf("netif_init: calling prom_iopen\n");
#endif

	/*
	 * Note: Sun PROMs will do RARP on open, but does not tell
	 * you the IP address it gets, so it is just noise to us...
	 */
	if ((error = prom_iopen(si)) != 0) {
		printf("netif_init: prom_iopen, error=%d\n", error);
		return (NULL);
	}

	if (si->si_sif == NULL) {
		printf("netif_init: not a network device\n");
		prom_iclose(si);
		return (NULL);
	}

	/* Allocate the transmit/receive buffers. */
	if (dd->rbuf == NULL) {
		dd->rbuf_len = PKT_BUF_SIZE;
		dd->rbuf = dvma_alloc(dd->rbuf_len);
	}
	if (dd->tbuf == NULL) {
		dd->tbuf_len = PKT_BUF_SIZE;
		dd->tbuf = dvma_alloc(dd->tbuf_len);
	}
	if ((dd->rbuf == NULL) ||
	    (dd->tbuf == NULL))
		panic("netif_init: malloc failed\n");

#ifdef NETIF_DEBUG
	if (debug)
		printf("netif_init: rbuf=0x%x, tbuf=0x%x\n",
			   dd->rbuf, dd->tbuf);
#endif

	/* Record our ethernet address. */
	netif_getether(si, dd->dd_myea);

	dd->dd_opens = 0;

	return(dd);
}

void
netif_fini(dd)
	struct devdata *dd;
{
	struct saioreq *si;

	si = &dd->dd_si;

#ifdef NETIF_DEBUG
	if (debug)
		printf("netif_fini: calling prom_iclose\n");
#endif

	prom_iclose(si);
	/* Dellocate the transmit/receive buffers. */
	if (dd->rbuf) {
		dvma_free(dd->rbuf, dd->rbuf_len);
		dd->rbuf = NULL;
	}
	if (dd->tbuf) {
		dvma_free(dd->tbuf, dd->tbuf_len);
		dd->tbuf = NULL;
	}
}

int
netif_attach(nif, s, aux)
	struct netif *nif;
	struct iodesc *s;
	void *aux;
{
	struct devdata *dd;

	dd = nif->nif_devdata;
	if (dd == NULL) {
		dd = netif_init(aux);
		if (dd == NULL)
			return (ENXIO);
		nif->nif_devdata = dd;
	}
	dd->dd_opens++;
	MACPY(dd->dd_myea, s->myea);
	s->io_netif = nif;
	return(0);
}

void
netif_detach(nif)
	struct netif *nif;
{
	struct devdata *dd;

	dd = nif->nif_devdata;
	if (dd == NULL)
		return;
	dd->dd_opens--;
	if (dd->dd_opens > 0)
		return;
	netif_fini(dd);
	nif->nif_devdata = NULL;
}

int
netif_open(aux)
	void *aux;
{
	struct netif *nif;
	struct iodesc *s;
	int fd, error;

	/* find a free socket */
	for (fd = 0, s = sockets; fd < SOPEN_MAX; fd++, s++)
		if (s->io_netif == NULL)
			goto found;
	errno = EMFILE;
	return (-1);

found:
	bzero(s, sizeof(*s));
	nif = &prom_netif;
	error = netif_attach(nif, s);
	if (error != 0) {
		errno = error;
		return (-1);
	}
	return (fd);
}

int
netif_close(fd)
	int fd;
{
	struct iodesc *s;
	struct netif *nif;

	if (fd < 0 || fd >= SOPEN_MAX) {
		errno = EBADF;
		return(-1);
	}
	s = &sockets[fd];
	nif = s->io_netif;
	/* Already closed? */
	if (nif == NULL)
		return(0);
	netif_detach(nif);
	s->io_netif = NULL;
	return(0);
}


struct iodesc *
socktodesc(fd)
	int fd;
{
	if (fd < 0 || fd >= SOPEN_MAX) {
		errno = EBADF;
		return (NULL);
	}
	return (&sockets[fd]);
}


/*
 * Send a packet.  The ether header is already there.
 * Return the length sent (or -1 on error).
 */
int
netif_put(desc, pkt, len)
	struct iodesc *desc;
	void *pkt;
	size_t len;
{
	struct netif *nif;
	struct devdata *dd;
	struct saioreq *si;
	struct saif *sif;
	char *dmabuf;
	int rv, slen;

#ifdef NETIF_DEBUG
	if (debug > 1) {
		struct ether_header *eh;

		printf("netif_put: desc=0x%x pkt=0x%x len=%d\n",
			   desc, pkt, len);
		eh = pkt;
		printf("dst: %s ", ether_sprintf(eh->ether_dhost));
		printf("src: %s ", ether_sprintf(eh->ether_shost));
		printf("type: 0x%x\n", eh->ether_type & 0xFFFF);
	}
#endif

	nif = desc->io_netif;
	dd = nif->nif_devdata;
	si = &dd->dd_si;
	sif = si->si_sif;
	slen = len;

#ifdef PARANOID
	if (sif == NULL)
		panic("netif_put: no saif ptr\n");
#endif

	/*
	 * Copy into our transmit buffer because the PROM
	 * network driver might continue using the packet
	 * after the sif_xmit call returns.  We never send
	 * very much data anyway, so the copy is fine.
	 */
	if (slen > dd->tbuf_len)
		panic("netif_put: slen=%d\n", slen);
	bcopy(pkt, dd->tbuf, slen);

	if (slen < 60) {
		slen = 60;
	}

	rv = (*sif->sif_xmit)(si->si_devdata, dd->tbuf, slen);

#ifdef NETIF_DEBUG
	if (debug > 1)
		printf("netif_put: xmit returned %d\n", rv);
#endif
	/*
	 * Just ignore the return value.  If the PROM transmit
	 * function fails, it will make some noise, such as:
	 *      le: No Carrier
	 */

	return len;
}

/*
 * Receive a packet, including the ether header.
 * Return the total length received (or -1 on error).
 */
int
netif_get(desc, pkt, maxlen, timo)
	struct iodesc *desc;
	void *pkt;
	size_t maxlen;
	time_t timo;	/* seconds */
{
	struct netif *nif;
	struct devdata *dd;
	struct saioreq *si;
	struct saif *sif;
	int tick0, tmo_ticks;
	int rlen = 0;

#ifdef NETIF_DEBUG
	if (debug > 1)
		printf("netif_get: pkt=0x%x, maxlen=%d, tmo=%d\n",
			   pkt, maxlen, timo);
#endif

	nif = desc->io_netif;
	dd = nif->nif_devdata;
	si = &dd->dd_si;
	sif = si->si_sif;

	tmo_ticks = timo * hz;

	/* Have to receive into our own buffer and copy. */
	do {
		tick0 = getticks();
		do {
			rlen = (*sif->sif_poll)(si->si_devdata, dd->rbuf);
			if (rlen != 0)
				goto break2;
		} while (getticks() == tick0);
	} while (--tmo_ticks > 0);

	/* No packet arrived.  Better reset the interface. */
	printf("netif_get: timeout; resetting\n");
	(*sif->sif_reset)(si->si_devdata, si);

break2:

#ifdef NETIF_DEBUG
	if (debug > 1)
		printf("netif_get: received rlen=%d\n", rlen);
#endif

	/* Need at least a valid Ethernet header. */
	if (rlen < 12)
		return -1;

	/* If we went beyond our buffer, were dead! */
	if (rlen > dd->rbuf_len)
		panic("netif_get: rlen=%d\n", rlen);

	/* The caller's buffer may be smaller... */
	if (rlen > maxlen)
		rlen = maxlen;

	bcopy(dd->rbuf, pkt, rlen);

#ifdef NETIF_DEBUG
	if (debug > 1) {
		struct ether_header *eh = pkt;

		printf("dst: %s ", ether_sprintf(eh->ether_dhost));
		printf("src: %s ", ether_sprintf(eh->ether_shost));
		printf("type: 0x%x\n", eh->ether_type & 0xFFFF);
	}
#endif

	return rlen;
}

/*
 * Copy our Ethernet address into the passed array.
 * Later PROM versions offer a function to do this.
 */
void
netif_getether(si, ea)
	struct saioreq *si;
	u_char *ea;
{
	struct saif *sif;

	sif = si->si_sif;
#ifdef NETIF_DEBUG
	if (debug) {
		printf("netif_getether: sif=0x%x\n", sif);
		breakpoint();
	}
#endif

	/*
	 * On Sun3X machines we want to use the PROM function
	 * to gets our Ethernet address.  On old Sun3 machines
	 * the PROM usually does not provide that function, so
	 * those do it the old way (by reading control space).
	 */
	if (_is3x) {
		/*
		 * All Sun3X machines have PROM version 3.0 or later.
		 * Well... I hope so anyway.  If not, loose here.
		 */
		if (romVectorPtr->monId[0] < '3')
			panic("netboot on Sun3X needs PROM 3.0 or later");
		(*sif->sif_macaddr)(ea);
		return;
	}

	/*
	 * Old Sun3 PROMs may not have sif_macaddr,
	 * so go read the Sun3 IDPROM directly.
	 */
	sun3_etheraddr(ea);
}
