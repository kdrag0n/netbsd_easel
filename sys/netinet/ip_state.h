/*	$NetBSD: ip_state.h,v 1.3 1997/03/29 00:55:05 thorpej Exp $	*/

/*
 * (C)opyright 1995 by Darren Reed.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and due credit is given
 * to the original author and the contributors.
 *
 * @(#)ip_state.h	1.3 1/12/96 (C) 1995 Darren Reed
 * $Id: ip_state.h,v 1.3 1997/03/29 00:55:05 thorpej Exp $
 */
#ifndef	__IP_STATE_H__
#define	__IP_STATE_H__

#define	IPSTATE_SIZE	257
#define	IPSTATE_MAX	2048	/* Maximum number of states held */

typedef struct udpstate {
	u_short	us_sport;
	u_short	us_dport;
} udpstate_t;

typedef struct icmpstate {
	u_short	ics_id;
	u_short	ics_seq;
	u_char	ics_type;
} icmpstate_t;

typedef	struct tcpstate {
	u_short	ts_sport;
	u_short	ts_dport;
	u_long	ts_seq;
	u_long	ts_ack;
	u_short	ts_swin;
	u_short	ts_dwin;
	u_char	ts_state[2];
} tcpstate_t;

typedef struct ipstate {
	struct	ipstate	*is_next;
	u_long	is_age;
	u_int	is_pass;
	u_int	is_pkts;
	u_int	is_bytes;
	struct	in_addr	is_src;
	struct	in_addr	is_dst;
	u_char	is_p;
	u_char	is_flags;
	union {
		icmpstate_t	is_ics;
		tcpstate_t	is_ts;
		udpstate_t	is_us;
	} is_ps;
} ipstate_t;

#define	is_icmp	is_ps.is_ics
#define	is_tcp	is_ps.is_ts
#define	is_udp	is_ps.is_us
#define	is_seq	is_tcp.ts_seq
#define	is_ack	is_tcp.ts_ack
#define	is_dwin	is_tcp.ts_dwin
#define	is_swin	is_tcp.ts_swin
#define	is_sport	is_tcp.ts_sport
#define	is_dport	is_tcp.ts_dport
#define	is_state	is_tcp.ts_state

#define	TH_OPENING	(TH_SYN|TH_ACK)

typedef	struct	ips_stat {
	u_long	iss_hits;
	u_long	iss_miss;
	u_long	iss_max;
	u_long	iss_tcp;
	u_long	iss_udp;
	u_long	iss_icmp;
	u_long	iss_nomem;
	u_long	iss_expire;
	u_long	iss_fin;
	u_long	iss_active;
	ipstate_t **iss_table;
} ips_stat_t;

extern int fr_tcpstate __P((ipstate_t *, fr_info_t *, ip_t *,
			    tcphdr_t *, u_short));
extern ips_stat_t *fr_statetstats __P((void));
extern int fr_addstate __P((ip_t *, fr_info_t *, u_int));
extern int fr_checkstate __P((ip_t *, fr_info_t *));
extern void fr_timeoutstate __P((void));
extern void fr_tcp_age __P((u_long *, u_char *, ip_t *, fr_info_t *, int));
extern void fr_stateunload __P((void));
#endif /* __IP_STATE_H__ */
