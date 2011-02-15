/*	$NetBSD: testutil.h,v 1.1.1.2 2011/02/15 19:32:42 christos Exp $	*/

/* Id: testutil.h,v 1.1.1.1 2003-06-04 00:27:03 marka Exp */
/*
 * Copyright (c) 2002 Japan Network Information Center.
 * All rights reserved.
 *  
 * By using this file, you agree to the terms and conditions set forth bellow.
 * 
 * 			LICENSE TERMS AND CONDITIONS 
 * 
 * The following License Terms and Conditions apply, unless a different
 * license is obtained from Japan Network Information Center ("JPNIC"),
 * a Japanese association, Kokusai-Kougyou-Kanda Bldg 6F, 2-3-4 Uchi-Kanda,
 * Chiyoda-ku, Tokyo 101-0047, Japan.
 * 
 * 1. Use, Modification and Redistribution (including distribution of any
 *    modified or derived work) in source and/or binary forms is permitted
 *    under this License Terms and Conditions.
 * 
 * 2. Redistribution of source code must retain the copyright notices as they
 *    appear in each source code file, this License Terms and Conditions.
 * 
 * 3. Redistribution in binary form must reproduce the Copyright Notice,
 *    this License Terms and Conditions, in the documentation and/or other
 *    materials provided with the distribution.  For the purposes of binary
 *    distribution the "Copyright Notice" refers to the following language:
 *    "Copyright (c) 2000-2002 Japan Network Information Center.  All rights reserved."
 * 
 * 4. The name of JPNIC may not be used to endorse or promote products
 *    derived from this Software without specific prior written approval of
 *    JPNIC.
 * 
 * 5. Disclaimer/Limitation of Liability: THIS SOFTWARE IS PROVIDED BY JPNIC
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL JPNIC BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef IDN_TESTUTIL_H
#define IDN_TESTUTIL_H 1

/*
 * Option flags for create_conf_file().
 *
 *	CONF_NO_EOF_NEWLINE		-- Don't put newline character
 *					   at the end of file.
 */
#define CONF_NO_EOF_NEWLINE		0x0001

/*
 * Create a configuration file.
 *
 * Write strings specified as variable length arguments (`...') to
 * `filename'.  Note that the arguments must be terminated with `NULL'.
 *
 * In the created config file, each string in the variable length
 * arguments becomes a line.  In other words, newline characters are
 * added automatically.
 *
 * This function returns 1 upon success, 0 otherwise.
 */
int
create_conf_file(const char *filename, unsigned int flags, ...);

#endif /* IDN_TESTUTIL_H */
