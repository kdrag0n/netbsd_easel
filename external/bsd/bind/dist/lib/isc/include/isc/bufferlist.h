/*	$NetBSD: bufferlist.h,v 1.1.1.2 2011/02/15 19:38:18 christos Exp $	*/

/*
 * Copyright (C) 2004-2007  Internet Systems Consortium, Inc. ("ISC")
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

/* Id: bufferlist.h,v 1.17 2007-06-19 23:47:18 tbox Exp */

#ifndef ISC_BUFFERLIST_H
#define ISC_BUFFERLIST_H 1

/*****
 ***** Module Info
 *****/

/*! \file isc/bufferlist.h
 *
 *
 *\brief	Buffer lists have no synchronization.  Clients must ensure exclusive
 *	access.
 *
 * \li Reliability:
 *	No anticipated impact.

 * \li Security:
 *	No anticipated impact.
 *
 * \li Standards:
 *	None.
 */

/***
 *** Imports
 ***/

#include <isc/lang.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

/***
 *** Functions
 ***/

unsigned int
isc_bufferlist_usedcount(isc_bufferlist_t *bl);
/*!<
 * \brief Return the length of the sum of all used regions of all buffers in
 * the buffer list 'bl'
 *
 * Requires:
 *
 *\li	'bl' is not NULL.
 *
 * Returns:
 *\li	sum of all used regions' lengths.
 */

unsigned int
isc_bufferlist_availablecount(isc_bufferlist_t *bl);
/*!<
 * \brief Return the length of the sum of all available regions of all buffers in
 * the buffer list 'bl'
 *
 * Requires:
 *
 *\li	'bl' is not NULL.
 *
 * Returns:
 *\li	sum of all available regions' lengths.
 */

ISC_LANG_ENDDECLS

#endif /* ISC_BUFFERLIST_H */
