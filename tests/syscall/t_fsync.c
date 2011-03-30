/* $NetBSD: t_fsync.c,v 1.1 2011/03/30 09:43:21 jruoho Exp $ */

/*-
 * Copyright (c) 2011 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jukka Ruohonen.
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
#include <sys/cdefs.h>
__RCSID("$NetBSD: t_fsync.c,v 1.1 2011/03/30 09:43:21 jruoho Exp $");

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <atf-c.h>

ATF_TC(fsync_1);
ATF_TC_HEAD(fsync_1, tc)
{
	atf_tc_set_md_var(tc, "descr", "Test error conditions of fsync(2)");
}

ATF_TC_BODY(fsync_1, tc)
{
	int i, fd[2];

	/*
	 * The fsync(2) call should fail with EBADF
	 * when the 'fd' is not a valid descriptor.
	 */
	for (i = 1; i < 1024; i = i + 128) {

		errno = 0;

		ATF_REQUIRE(fsync(-i) == -1);
		ATF_REQUIRE(errno == EBADF);
	}

	/*
	 * On the other hand, EINVAL should follow
	 * if the operation is not possible with
	 * the file descriptor (cf. PR kern/30).
	 */
	ATF_REQUIRE(pipe(fd) == 0);

	errno = 0;

	ATF_REQUIRE(fsync(fd[0]) == -1);
	ATF_REQUIRE(errno == EINVAL);

	errno = 0;

	ATF_REQUIRE(fsync(fd[1]) == -1);
	ATF_REQUIRE(errno == EINVAL);

	ATF_REQUIRE(close(fd[0]) == 0);
	ATF_REQUIRE(close(fd[1]) == 0);
}

ATF_TC(fsync_2);
ATF_TC_HEAD(fsync_2, tc)
{
	atf_tc_set_md_var(tc, "descr", "A basic test of fsync(2)");
}

ATF_TC_BODY(fsync_2, tc)
{
	char buf[128];
	int fd, i;

	for (i = 0; i < 10; i++) {

		(void)snprintf(buf, sizeof(buf), "t_fsync-%d", i);

		fd = mkstemp(buf);

		ATF_REQUIRE(fd != -1);
		ATF_REQUIRE(write(fd, "0", 1) == 1);
		ATF_REQUIRE(fsync(fd) == 0);

		ATF_REQUIRE(unlink(buf) == 0);
		ATF_REQUIRE(close(fd) == 0);
	}
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, fsync_1);
	ATF_TP_ADD_TC(tp, fsync_2);

	return atf_no_error();
}
