/*	$NetBSD: ntp_proto.h,v 1.2 1999/02/15 04:03:03 hubertf Exp $
 */

#ifndef __ntp_proto_h
#define __ntp_proto_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_PROTOTYPES
#define P(x) x
#else
#define P(x) ()
#endif

#endif /* __ntp_proto_h */
