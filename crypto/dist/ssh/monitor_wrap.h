/*	$NetBSD: monitor_wrap.h,v 1.2 2002/09/09 06:45:19 itojun Exp $	*/
/*	$OpenBSD: monitor_wrap.h,v 1.5 2002/05/12 23:53:45 djm Exp $	*/

/*
 * Copyright 2002 Niels Provos <provos@citi.umich.edu>
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

#ifndef _MM_WRAP_H_
#define _MM_WRAP_H_
#include "key.h"
#include "buffer.h"

extern int use_privsep;
#define PRIVSEP(x)	(use_privsep ? mm_##x : x)

enum mm_keytype {MM_NOKEY, MM_HOSTKEY, MM_USERKEY, MM_RSAHOSTKEY, MM_RSAUSERKEY};

struct monitor;
struct mm_master;
struct passwd;
struct Authctxt;

DH *mm_choose_dh(int, int, int);
int mm_key_sign(Key *, u_char **, u_int *, u_char *, u_int);
void mm_inform_authserv(char *, char *);
struct passwd *mm_getpwnamallow(const char *);
char* mm_auth2_read_banner(void);
int mm_auth_password(struct Authctxt *, char *);
int mm_key_allowed(enum mm_keytype, char *, char *, Key *);
int mm_user_key_allowed(struct passwd *, Key *);
int mm_hostbased_key_allowed(struct passwd *, char *, char *, Key *);
int mm_auth_rhosts_rsa_key_allowed(struct passwd *, char *, char *, Key *);
int mm_key_verify(Key *, u_char *, u_int, u_char *, u_int);
int mm_auth_rsa_key_allowed(struct passwd *, BIGNUM *, Key **);
int mm_auth_rsa_verify_response(Key *, BIGNUM *, u_char *);
BIGNUM *mm_auth_rsa_generate_challenge(Key *);

void mm_terminate(void);
int mm_pty_allocate(int *, int *, char *, int);
void mm_session_pty_cleanup2(void *);

/* SSHv1 interfaces */
void mm_ssh1_session_id(u_char *);
int mm_ssh1_session_key(BIGNUM *);

/* Key export functions */
struct Newkeys *mm_newkeys_from_blob(u_char *, int);
int mm_newkeys_to_blob(int, u_char **, u_int *);

void monitor_apply_keystate(struct monitor *);
void mm_get_keystate(struct monitor *);
void mm_send_keystate(struct monitor*);

/* bsdauth */
int mm_bsdauth_query(void *, char **, char **, u_int *, char ***, u_int **);
int mm_bsdauth_respond(void *, u_int, char **);

/* skey */
int mm_skey_query(void *, char **, char **, u_int *, char ***, u_int **);
int mm_skey_respond(void *, u_int, char **);

/* auth_krb5 */
#ifdef KRB5
/* auth and reply are really krb5_data objects, but we don't want to
 * include all of the krb5 headers here */
int mm_auth_krb5(void *authctxt, void *auth, char **client, void *reply);
#endif

/* zlib allocation hooks */

void *mm_zalloc(struct mm_master *, u_int, u_int);
void mm_zfree(struct mm_master *, void *);
void mm_init_compression(struct mm_master *);

#endif /* _MM_H_ */
