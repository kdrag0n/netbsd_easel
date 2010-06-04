/*
 * Automated Testing Framework (atf)
 *
 * Copyright (c) 2007, 2008, 2009, 2010 The NetBSD Foundation, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atf-c.h>

#include "atf-c/env.h"
#include "atf-c/error.h"
#include "atf-c/fs.h"
#include "atf-c/text.h"

#include "../atf-c/h_lib.h"

/* ---------------------------------------------------------------------
 * Auxiliary functions.
 * --------------------------------------------------------------------- */

static
void
safe_remove(const char* path)
{
    if (unlink(path) == -1)
        atf_tc_fail("unlink(2) of %s failed", path);
}

static
void
touch(const char *path)
{
    int fd;
    fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1)
        atf_tc_fail("Could not create file %s", path);
    close(fd);
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_cleanup".
 * --------------------------------------------------------------------- */

ATF_TC_WITH_CLEANUP(cleanup_pass);
ATF_TC_HEAD(cleanup_pass, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
               "program");
}
ATF_TC_BODY(cleanup_pass, tc)
{
    touch(atf_tc_get_config_var(tc, "tmpfile"));
}
ATF_TC_CLEANUP(cleanup_pass, tc)
{
    bool cleanup;

    RE(atf_text_to_bool(atf_tc_get_config_var(tc, "cleanup"), &cleanup));

    if (cleanup)
        safe_remove(atf_tc_get_config_var(tc, "tmpfile"));
}

ATF_TC_WITH_CLEANUP(cleanup_fail);
ATF_TC_HEAD(cleanup_fail, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
                      "program");
}
ATF_TC_BODY(cleanup_fail, tc)
{
    touch(atf_tc_get_config_var(tc, "tmpfile"));
    atf_tc_fail("On purpose");
}
ATF_TC_CLEANUP(cleanup_fail, tc)
{
    bool cleanup;

    RE(atf_text_to_bool(atf_tc_get_config_var(tc, "cleanup"), &cleanup));

    if (cleanup)
        safe_remove(atf_tc_get_config_var(tc, "tmpfile"));
}

ATF_TC_WITH_CLEANUP(cleanup_skip);
ATF_TC_HEAD(cleanup_skip, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
                      "program");
}
ATF_TC_BODY(cleanup_skip, tc)
{
    touch(atf_tc_get_config_var(tc, "tmpfile"));
    atf_tc_skip("On purpose");
}
ATF_TC_CLEANUP(cleanup_skip, tc)
{
    bool cleanup;

    RE(atf_text_to_bool(atf_tc_get_config_var(tc, "cleanup"), &cleanup));

    if (cleanup)
        safe_remove(atf_tc_get_config_var(tc, "tmpfile"));
}

ATF_TC_WITH_CLEANUP(cleanup_curdir);
ATF_TC_HEAD(cleanup_curdir, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
                      "program");
}
ATF_TC_BODY(cleanup_curdir, tc)
{
    FILE *f;

    f = fopen("oldvalue", "w");
    if (f == NULL)
        atf_tc_fail("Failed to create oldvalue file");
    fprintf(f, "1234");
    fclose(f);
}
ATF_TC_CLEANUP(cleanup_curdir, tc)
{
    FILE *f;

    f = fopen("oldvalue", "r");
    if (f != NULL) {
        int i;
        if (fscanf(f, "%d", &i) != 1)
            fprintf(stderr, "Failed to read old value\n");
        else
            printf("Old value: %d", i);
        fclose(f);
    }
}

ATF_TC_WITH_CLEANUP(cleanup_sigterm);
ATF_TC_HEAD(cleanup_sigterm, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
                      "program");
}
ATF_TC_BODY(cleanup_sigterm, tc)
{
    char *nofile;

    touch(atf_tc_get_config_var(tc, "tmpfile"));
    kill(getpid(), SIGTERM);

    RE(atf_text_format(&nofile, "%s.no",
                       atf_tc_get_config_var(tc, "tmpfile")));
    touch(nofile);
    free(nofile);
}
ATF_TC_CLEANUP(cleanup_sigterm, tc)
{
    safe_remove(atf_tc_get_config_var(tc, "tmpfile"));
}

ATF_TC_WITH_CLEANUP(cleanup_fork);
ATF_TC_HEAD(cleanup_fork, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_cleanup test "
                      "program");
}
ATF_TC_BODY(cleanup_fork, tc)
{
}
ATF_TC_CLEANUP(cleanup_fork, tc)
{
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    close(3);
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_config".
 * --------------------------------------------------------------------- */

ATF_TC(config_unset);
ATF_TC_HEAD(config_unset, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_config test "
                      "program");
}
ATF_TC_BODY(config_unset, tc)
{
    ATF_REQUIRE(!atf_tc_has_config_var(tc, "test"));
}

ATF_TC(config_empty);
ATF_TC_HEAD(config_empty, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_config test "
                      "program");
}
ATF_TC_BODY(config_empty, tc)
{
    ATF_REQUIRE(atf_tc_has_config_var(tc, "test"));
    ATF_REQUIRE(strlen(atf_tc_get_config_var(tc, "test")) == 0);
}

ATF_TC(config_value);
ATF_TC_HEAD(config_value, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_config test "
                      "program");
}
ATF_TC_BODY(config_value, tc)
{
    ATF_REQUIRE(atf_tc_has_config_var(tc, "test"));
    ATF_REQUIRE(strcmp(atf_tc_get_config_var(tc, "test"), "foo") == 0);
}

ATF_TC(config_multi_value);
ATF_TC_HEAD(config_multi_value, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_config test "
                      "program");
}
ATF_TC_BODY(config_multi_value, tc)
{
    ATF_REQUIRE(atf_tc_has_config_var(tc, "test"));
    ATF_REQUIRE(strcmp(atf_tc_get_config_var(tc, "test"), "foo bar") == 0);
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_fork".
 * --------------------------------------------------------------------- */

ATF_TC(fork_stop);
ATF_TC_HEAD(fork_stop, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_fork test "
                      "program");
}
ATF_TC_BODY(fork_stop, tc)
{
    FILE *f;
    const char *dfstr, *pfstr;

    dfstr = atf_tc_get_config_var(tc, "donefile");
    pfstr = atf_tc_get_config_var(tc, "pidfile");

    f = fopen(pfstr, "w");
    if (f == NULL)
        atf_tc_fail("Failed to create pidfile %s", pfstr);
    fprintf(f, "%d", (int)getpid());
    fclose(f);
    printf("Wrote pid file\n");

    printf("Waiting for done file\n");
    while (access(dfstr, F_OK) != 0)
        usleep(10000);
    printf("Exiting\n");
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_meta_data".
 * --------------------------------------------------------------------- */

ATF_TC(metadata_no_descr);
ATF_TC_HEAD(metadata_no_descr, tc)
{
}
ATF_TC_BODY(metadata_no_descr, tc)
{
}

ATF_TC_WITHOUT_HEAD(metadata_no_head);
ATF_TC_BODY(metadata_no_head, tc)
{
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_srcdir".
 * --------------------------------------------------------------------- */

ATF_TC(srcdir_exists);
ATF_TC_HEAD(srcdir_exists, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_srcdir test "
                      "program");
}
ATF_TC_BODY(srcdir_exists, tc)
{
    atf_fs_path_t p;
    bool b;

    RE(atf_fs_path_init_fmt(&p, "%s/datafile",
                            atf_tc_get_config_var(tc, "srcdir")));
    RE(atf_fs_exists(&p, &b));
    atf_fs_path_fini(&p);
    if (!b)
        atf_tc_fail("Cannot find datafile");
}

/* ---------------------------------------------------------------------
 * Helper tests for "t_status".
 * --------------------------------------------------------------------- */

ATF_TC(status_newlines_fail);
ATF_TC_HEAD(status_newlines_fail, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_status test "
                      "program");
}
ATF_TC_BODY(status_newlines_fail, tc)
{
    atf_tc_fail("First line\nSecond line");
}

ATF_TC(status_newlines_skip);
ATF_TC_HEAD(status_newlines_skip, tc)
{
    atf_tc_set_md_var(tc, "descr", "Helper test case for the t_status test "
                      "program");
}
ATF_TC_BODY(status_newlines_skip, tc)
{
    atf_tc_skip("First line\nSecond line");
}

/* ---------------------------------------------------------------------
 * Main.
 * --------------------------------------------------------------------- */

ATF_TP_ADD_TCS(tp)
{
    /* Add helper tests for t_cleanup. */
    ATF_TP_ADD_TC(tp, cleanup_pass);
    ATF_TP_ADD_TC(tp, cleanup_fail);
    ATF_TP_ADD_TC(tp, cleanup_skip);
    ATF_TP_ADD_TC(tp, cleanup_curdir);
    ATF_TP_ADD_TC(tp, cleanup_sigterm);
    ATF_TP_ADD_TC(tp, cleanup_fork);

    /* Add helper tests for t_config. */
    ATF_TP_ADD_TC(tp, config_unset);
    ATF_TP_ADD_TC(tp, config_empty);
    ATF_TP_ADD_TC(tp, config_value);
    ATF_TP_ADD_TC(tp, config_multi_value);

    /* Add helper tests for t_fork. */
    ATF_TP_ADD_TC(tp, fork_stop);

    /* Add helper tests for t_meta_data. */
    ATF_TP_ADD_TC(tp, metadata_no_descr);
    ATF_TP_ADD_TC(tp, metadata_no_head);

    /* Add helper tests for t_srcdir. */
    ATF_TP_ADD_TC(tp, srcdir_exists);

    /* Add helper tests for t_status. */
    ATF_TP_ADD_TC(tp, status_newlines_fail);
    ATF_TP_ADD_TC(tp, status_newlines_skip);

    return atf_no_error();
}
