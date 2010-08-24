/*	$NetBSD: in6_proto.c,v 1.89 2010/08/24 00:07:00 jakllsch Exp $	*/
/*	$KAME: in6_proto.c,v 1.66 2000/10/10 15:35:47 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1993
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
 * 3. Neither the name of the University nor the names of its contributors
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
 *	@(#)in_proto.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: in6_proto.c,v 1.89 2010/08/24 00:07:00 jakllsch Exp $");

#include "opt_gateway.h"
#include "opt_inet.h"
#include "opt_ipsec.h"
#include "opt_iso.h"

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/protosw.h>
#include <sys/kernel.h>
#include <sys/domain.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/radix.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip_encap.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/in_pcb.h>
#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet/icmp6.h>
#include <netinet6/in6_pcb.h>

#include <netinet/tcp.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_debug.h>

#include <netinet6/udp6.h>
#include <netinet6/udp6_var.h>

#include <netinet6/pim6_var.h>

#include <netinet6/nd6.h>

#ifdef IPSEC
#include <netinet6/ipsec.h>
#include <netinet6/ah.h>
#ifdef IPSEC_ESP
#include <netinet6/esp.h>
#endif
#include <netinet6/ipcomp.h>
#endif /* IPSEC */

#ifdef FAST_IPSEC
#include <netipsec/ipsec.h>
#include <netipsec/ipsec6.h>
#include <netipsec/key.h>
#endif /* FAST_IPSEC */


#include "carp.h"
#if NCARP > 0
#include <netinet/ip_carp.h>
#endif

#include "etherip.h"
#if NETHERIP > 0
#include <netinet6/ip6_etherip.h>
#endif

#include <netinet6/ip6protosw.h>

#include <net/net_osdep.h>

#ifndef offsetof		/* XXX */
#define	offsetof(type, member)	((size_t)(&((type *)0)->member))
#endif

/*
 * TCP/IP protocol family: IP6, ICMP6, UDP, TCP.
 */

DOMAIN_DEFINE(inet6domain);	/* forward declare and add to link set */

/* Wrappers to acquire kernel_lock. */

PR_WRAP_USRREQ(rip6_usrreq)
PR_WRAP_USRREQ(udp6_usrreq)
PR_WRAP_USRREQ(tcp_usrreq)

#define	rip6_usrreq 	rip6_usrreq_wrapper
#define	udp6_usrreq 	udp6_usrreq_wrapper
#define	tcp_usrreq 	tcp_usrreq_wrapper

PR_WRAP_CTLINPUT(rip6_ctlinput)
PR_WRAP_CTLINPUT(encap6_ctlinput)
PR_WRAP_CTLINPUT(udp6_ctlinput)
PR_WRAP_CTLINPUT(tcp6_ctlinput)

#define	rip6_ctlinput	rip6_ctlinput_wrapper
#define	encap6_ctlinput	encap6_ctlinput_wrapper
#define	udp6_ctlinput	udp6_ctlinput_wrapper
#define	tcp6_ctlinput	tcp6_ctlinput_wrapper

PR_WRAP_CTLOUTPUT(rip6_ctloutput)
PR_WRAP_CTLOUTPUT(ip6_ctloutput)
PR_WRAP_CTLOUTPUT(tcp_ctloutput)
PR_WRAP_CTLOUTPUT(icmp6_ctloutput)

#define	rip6_ctloutput	rip6_ctloutput_wrapper
#define	ip6_ctloutput	ip6_ctloutput_wrapper
#define	tcp_ctloutput	tcp_ctloutput_wrapper
#define	icmp6_ctloutput	icmp6_ctloutput_wrapper

#if defined(IPSEC) || defined(FAST_IPSEC)
PR_WRAP_CTLINPUT(ah6_ctlinput)

#define	ah6_ctlinput	ah6_ctlinput_wrapper
#endif

#if (defined(IPSEC) && defined(IPSEC_ESP)) || defined(FAST_IPSEC)
PR_WRAP_CTLINPUT(esp6_ctlinput)

#define	esp6_ctlinput	esp6_ctlinput_wrapper
#endif

const struct ip6protosw inet6sw[] = {
{	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPV6,
	.pr_init = ip6_init,
	.pr_slowtimo = frag6_slowtimo,
	.pr_drain = frag6_drain,
},
{	.pr_type = SOCK_DGRAM,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_UDP,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_PURGEIF,
	.pr_input = udp6_input,
	.pr_ctlinput = udp6_ctlinput,
	.pr_ctloutput = ip6_ctloutput,
	.pr_usrreq = udp6_usrreq,
	.pr_init = udp6_init,
},
{	.pr_type = SOCK_STREAM,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_TCP,
	.pr_flags = PR_CONNREQUIRED|PR_WANTRCVD|PR_LISTEN|PR_ABRTACPTDIS|PR_PURGEIF,
	.pr_input = tcp6_input,
	.pr_ctlinput = tcp6_ctlinput,
	.pr_ctloutput = tcp_ctloutput,
	.pr_usrreq = tcp_usrreq,
#ifndef INET	/* don't call initialization and timeout routines twice */
	.pr_init = tcp_init,
	.pr_slowtimo = tcp_slowtimo,
	.pr_drain = tcp_drain,
#endif
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_RAW,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_PURGEIF,
	.pr_input = rip6_input,
	.pr_output = rip6_output,
	.pr_ctlinput = rip6_ctlinput,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
},
#ifdef GATEWAY
{	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPV6,
	.pr_slowtimo = ip6flow_slowtimo,
	.pr_init = ip6flow_poolinit,
},
#endif /* GATEWAY */
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_ICMPV6,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = icmp6_input,
	.pr_output = rip6_output,
	.pr_ctlinput = rip6_ctlinput,
	.pr_ctloutput = icmp6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	.pr_init = icmp6_init,
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_DSTOPTS,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = dest6_input,
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_ROUTING,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = route6_input,
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_FRAGMENT,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = frag6_input,
},
#ifdef IPSEC
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_AH,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = ah6_input,
	.pr_ctlinput = ah6_ctlinput,
	.pr_init = ah6_init,
},
#ifdef IPSEC_ESP
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_ESP,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = esp6_input,
	.pr_ctlinput = esp6_ctlinput,
	.pr_init = esp6_init,
},
#endif
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPCOMP,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = ipcomp6_input,
	.pr_init = ipcomp6_init,
},
#endif /* IPSEC */
#ifdef FAST_IPSEC
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_AH,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = ipsec6_common_input,
	.pr_ctlinput = ah6_ctlinput,
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_ESP,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = ipsec6_common_input,
	.pr_ctlinput = esp6_ctlinput,
},
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPCOMP,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = ipsec6_common_input,
},
#endif /* FAST_IPSEC */
#ifdef INET
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPV4,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = encap6_input,
	.pr_output = rip6_output,
	.pr_ctlinput = encap6_ctlinput,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	.pr_init = encap_init,
},
#endif
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_IPV6,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = encap6_input,
	.pr_output = rip6_output,
	.pr_ctlinput = encap6_ctlinput,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	.pr_init = encap_init,
},
#if NETHERIP > 0
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_ETHERIP,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = ip6_etherip_input,
	.pr_output = rip6_output,
	.pr_ctlinput = rip6_ctlinput,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
},
#endif
#if NCARP > 0
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_CARP,
	.pr_flags = PR_ATOMIC|PR_ADDR,
	.pr_input = carp6_proto_input,
	.pr_output = rip6_output,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
},
#endif /* NCARP */
#ifdef ISO
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_EON,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = encap6_input,
	.pr_output = rip6_output,
	.pr_ctlinput = encap6_ctlinput,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	/*XXX*/
	.pr_init = encap_init,
},
#endif
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_protocol = IPPROTO_PIM,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = pim6_input,
	.pr_output = rip6_output,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	.pr_init = pim6_init,
},
/* raw wildcard */
{	.pr_type = SOCK_RAW,
	.pr_domain = &inet6domain,
	.pr_flags = PR_ATOMIC|PR_ADDR|PR_LASTHDR,
	.pr_input = rip6_input,
	.pr_output = rip6_output,
	.pr_ctloutput = rip6_ctloutput,
	.pr_usrreq = rip6_usrreq,
	.pr_init = rip6_init,
},
};

static const struct sockaddr_in6 in6_any = {
	  .sin6_len = sizeof(in6_any)
	, .sin6_family = AF_INET6
	, .sin6_port = 0
	, .sin6_flowinfo = 0
	, .sin6_addr = IN6ADDR_ANY_INIT
	, .sin6_scope_id = 0
};

struct domain inet6domain = {
	.dom_family = AF_INET6, .dom_name = "internet6",
	.dom_init = NULL, .dom_externalize = NULL, .dom_dispose = NULL,
	.dom_protosw = (const struct protosw *)inet6sw,
	.dom_protoswNPROTOSW = (const struct protosw *)&inet6sw[sizeof(inet6sw)/sizeof(inet6sw[0])],
	.dom_rtattach = rn_inithead,
	.dom_rtoffset = offsetof(struct sockaddr_in6, sin6_addr) << 3,
	.dom_maxrtkey = sizeof(struct ip_pack6),
	.dom_ifattach = in6_domifattach, .dom_ifdetach = in6_domifdetach,
	.dom_ifqueues = { &ip6intrq, NULL },
	.dom_link = { NULL },
	.dom_mowner = MOWNER_INIT("",""),
	.dom_sa_cmpofs = offsetof(struct sockaddr_in6, sin6_addr),
	.dom_sa_cmplen = sizeof(struct in6_addr),
	.dom_sa_any = (const struct sockaddr *)&in6_any,
	.dom_sockaddr_externalize = sockaddr_in6_externalize,
	.dom_rtcache = LIST_HEAD_INITIALIZER(inet6domain.dom_rtcache)
};

#if 0
int
sockaddr_in6_cmp(const struct sockaddr *lsa, const struct sockaddr *rsa)
{
	uint_fast8_t len;
	const uint_fast8_t addrofs = offsetof(struct sockaddr_in6, sin6_addr),
			   addrend = addrofs + sizeof(struct in6_addr);
	int rc;
	const struct sockaddr_in6 *lsin6, *rsin6;

	lsin6 = satocsin6(lsa);
	rsin6 = satocsin6(rsa);

	len = MIN(addrend, MIN(lsin6->sin6_len, rsin6->sin6_len));

	if (len > addrofs &&
	    (rc = memcmp(&lsin6->sin6_addr, &rsin6->sin6_addr,
	                  len - addrofs)) != 0)
		return rc;

	return lsin6->sin6_len - rsin6->sin6_len;
}
#endif

/*
 * Internet configuration info
 */
#ifndef	IPV6FORWARDING
#ifdef GATEWAY6
#define	IPV6FORWARDING	1	/* forward IP6 packets not for us */
#else
#define	IPV6FORWARDING	0	/* don't forward IP6 packets not for us */
#endif /* GATEWAY6 */
#endif /* !IPV6FORWARDING */

int	ip6_forwarding = IPV6FORWARDING;	/* act as router? */
int	ip6_sendredirects = 1;
int	ip6_defhlim = IPV6_DEFHLIM;
int	ip6_defmcasthlim = IPV6_DEFAULT_MULTICAST_HOPS;
int	ip6_accept_rtadv = 0;	/* "IPV6FORWARDING ? 0 : 1" is dangerous */
int	ip6_maxfragpackets = 200;
int	ip6_maxfrags = 200;
int	ip6_log_interval = 5;
int	ip6_hdrnestlimit = 50;	/* appropriate? */
int	ip6_dad_count = 1;	/* DupAddrDetectionTransmits */
int	ip6_auto_flowlabel = 1;
int	ip6_use_deprecated = 1;	/* allow deprecated addr (RFC2462 5.5.4) */
int	ip6_rr_prune = 5;	/* router renumbering prefix
				 * walk list every 5 sec. */
int	ip6_mcast_pmtu = 0;	/* enable pMTU discovery for multicast? */
int	ip6_v6only = 1;

int	ip6_keepfaith = 0;
time_t	ip6_log_time = (time_t)0L;

/* icmp6 */
/*
 * BSDI4 defines these variables in in_proto.c...
 * XXX: what if we don't define INET? Should we define pmtu6_expire
 * or so? (jinmei@kame.net 19990310)
 */
int pmtu_expire = 60*10;

/* raw IP6 parameters */
/*
 * Nominal space allocated to a raw ip socket.
 */
#define	RIPV6SNDQ	8192
#define	RIPV6RCVQ	8192

u_long	rip6_sendspace = RIPV6SNDQ;
u_long	rip6_recvspace = RIPV6RCVQ;

/* ICMPV6 parameters */
int	icmp6_rediraccept = 1;		/* accept and process redirects */
int	icmp6_redirtimeout = 10 * 60;	/* 10 minutes */
int	icmp6errppslim = 100;		/* 100pps */
int	icmp6_nodeinfo = 1;		/* enable/disable NI response */

/* UDP on IP6 parameters */
int	udp6_sendspace = 9216;		/* really max datagram size */
int	udp6_recvspace = 40 * (1024 + sizeof(struct sockaddr_in6));
					/* 40 1K datagrams */
