/*	$NetBSD: aes_armv8.h,v 1.1 2020/06/29 23:31:41 riastradh Exp $	*/

/*-
 * Copyright (c) 2020 The NetBSD Foundation, Inc.
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

#ifndef	_CRYPTO_AES_AES_ARCH_ARM_AES_ARMV8_H
#define	_CRYPTO_AES_AES_ARCH_ARM_AES_ARMV8_H

#include <sys/types.h>

#include <crypto/aes/aes.h>

/* Assembly routines */

void	aesarmv8_setenckey128(struct aesenc *, const uint8_t[static 16]);
void	aesarmv8_setenckey192(struct aesenc *, const uint8_t[static 24]);
void	aesarmv8_setenckey256(struct aesenc *, const uint8_t[static 32]);

void	aesarmv8_enctodec(const struct aesenc *, struct aesdec *, uint32_t);

void	aesarmv8_enc(const struct aesenc *, const uint8_t[static 16],
	    uint8_t[static 16], uint32_t);
void	aesarmv8_dec(const struct aesdec *, const uint8_t[static 16],
	    uint8_t[static 16], uint32_t);

void	aesarmv8_cbc_enc(const struct aesenc *, const uint8_t[static 16],
	    uint8_t[static 16], size_t, uint8_t[static 16], uint32_t);
void	aesarmv8_cbc_dec1(const struct aesdec *, const uint8_t[static 16],
	    uint8_t[static 16], size_t, const uint8_t[static 16], uint32_t);
void	aesarmv8_cbc_dec8(const struct aesdec *, const uint8_t[static 128],
	    uint8_t[static 128], size_t, const uint8_t[static 16], uint32_t);

void	aesarmv8_xts_enc1(const struct aesenc *, const uint8_t[static 16],
	    uint8_t[static 16], size_t, uint8_t[static 16], uint32_t);
void	aesarmv8_xts_enc8(const struct aesenc *, const uint8_t[static 128],
	    uint8_t[static 128], size_t, const uint8_t[static 16], uint32_t);
void	aesarmv8_xts_dec1(const struct aesdec *, const uint8_t[static 16],
	    uint8_t[static 16], size_t, uint8_t[static 16], uint32_t);
void	aesarmv8_xts_dec8(const struct aesdec *, const uint8_t[static 128],
	    uint8_t[static 128], size_t, const uint8_t[static 16], uint32_t);
void	aesarmv8_xts_update(const uint8_t[static 16], uint8_t[static 16]);

extern struct aes_impl aes_armv8_impl;

#endif	/* _CRYPTO_AES_AES_ARCH_ARM_AES_ARMV8_H */
