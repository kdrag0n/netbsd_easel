/*	$NetBSD: version.h,v 1.23 2002/05/13 02:58:19 itojun Exp $	*/
/* $OpenBSD: version.h,v 1.30 2002/04/23 12:54:10 markus Exp $ */

#define __OPENSSH_VERSION	"OpenSSH_3.2.1"
#define __NETBSDSSH_VERSION	"NetBSD_Secure_Shell-20020513"

/*
 * it is important to retain OpenSSH version identification part, it is
 * used for bug compatibility operation.  present NetBSD SSH version as comment
 */
#define SSH_VERSION	(__OPENSSH_VERSION " " __NETBSDSSH_VERSION)
