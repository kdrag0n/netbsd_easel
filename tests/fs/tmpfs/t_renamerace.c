/*	$NetBSD: t_renamerace.c,v 1.2 2009/04/08 08:57:24 pooka Exp $	*/

/*
 * Modified for rump and atf from a program supplied
 * by Nicolas Joly in kern/40948
 */

#include <sys/types.h>
#include <sys/mount.h>

#include <atf-c.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <rump/rump.h>
#include <rump/rump_syscalls.h>
#include <rump/ukfs.h>

#include <fs/tmpfs/tmpfs_args.h>

ATF_TC(renamerace);
ATF_TC_HEAD(renamerace, tc)
{
	atf_tc_set_md_var(tc, "descr", "rename(2) race against files "
	    "unlinked mid-operation, kern/41128");
}

void *
w1(void *arg)
{
  int fd;

  for (;;) {
    fd = rump_sys_open("/rename.test1", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    rump_sys_unlink("/rename.test1");
    rump_sys_close(fd);
  }
  return NULL;
}

void *
w2(void *arg)
{

  for (;;) {
    rump_sys_rename("/rename.test1", "/rename.test2");
  }
  return NULL;
}

ATF_TC_BODY(renamerace, tc)
{
  struct tmpfs_args args;
  struct ukfs *fs;
  pthread_t pt1, pt2;
  int fail = 0, succ = 0, i;

  memset(&args, 0, sizeof(args));
  args.ta_version = TMPFS_ARGS_VERSION;
  args.ta_root_mode = 0777;

  ukfs_init();
  fs = ukfs_mount(MOUNT_TMPFS, "tmpfs", UKFS_DEFAULTMP, 0, &args, sizeof(args));
  if (fs == NULL)
    err(1, "ukfs_mount");

  pthread_create(&pt1, NULL, w1, fs);
  pthread_create(&pt2, NULL, w2, fs);

  sleep(10);
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, renamerace);
}
