/*	$NetBSD: nsap_22.h,v 1.1.1.2 2011/02/15 19:38:00 christos Exp $	*/

/*
 * Copyright (C) 2004, 2005, 2007  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef IN_1_NSAP_22_H
#define IN_1_NSAP_22_H 1

/* Id: nsap_22.h,v 1.18 2007-06-19 23:47:17 tbox Exp */

/*! 
 *  \brief Per RFC1706 */

typedef struct dns_rdata_in_nsap {
	dns_rdatacommon_t	common;
	isc_mem_t		*mctx;
	unsigned char		*nsap;
	isc_uint16_t		nsap_len;
} dns_rdata_in_nsap_t;

#endif /* IN_1_NSAP_22_H */
