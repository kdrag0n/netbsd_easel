/*	$NetBSD: meta.c,v 1.1 2000/04/22 13:29:02 blymn Exp $	*/

/*-
 * Copyright (c) 1998-2000 Brett Lymn
 *                         (blymn@baea.com.au, brett_lymn@yahoo.com.au)
 * All rights reserved.
 *
 * This code has been donated to The NetBSD Foundation by the Author.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
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
 *
 *
 */

#include "curses.h"
#include "curses_private.h"

/*
 * meta --
 *    Turn on or off the terminal meta mode.
 */
int
meta(/*ARGSUSED*/ WINDOW *win, bool bf)
{
	if (bf == TRUE) {
		if (MM != NULL) {
#ifdef DEBUG
			__CTRACE("meta: TRUE\n");
#endif
			tputs(MM, 0, __cputchar);
		}
	} else {
		if (MO != NULL) {
#ifdef DEBUG
			__CTRACE("meta: FALSE\n");
#endif
			tputs(MO, 0, __cputchar);
		}
	}

	return OK;
}

	
