/* $NetBSD: linux_syscalls.c,v 1.12 2003/01/18 08:07:21 thorpej Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD  
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.12 2003/01/18 08:07:21 thorpej Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_netbsd.h"
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/common/linux_socketcall.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"waitpid",			/* 7 = waitpid */
	"creat",			/* 8 = creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"execve",			/* 11 = execve */
	"chdir",			/* 12 = chdir */
	"time",			/* 13 = time */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"lchown",			/* 16 = lchown */
	"#17 (unimplemented)",		/* 17 = unimplemented */
	"#18 (obsolete ostat)",		/* 18 = obsolete ostat */
	"lseek",			/* 19 = lseek */
	"getpid",			/* 20 = getpid */
	"#21 (unimplemented mount)",		/* 21 = unimplemented mount */
	"#22 (obsolete umount)",		/* 22 = obsolete umount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"stime",			/* 25 = stime */
	"ptrace",			/* 26 = ptrace */
	"alarm",			/* 27 = alarm */
	"#28 (obsolete ofstat)",		/* 28 = obsolete ofstat */
	"pause",			/* 29 = pause */
	"utime",			/* 30 = utime */
	"#31 (unimplemented)",		/* 31 = unimplemented */
	"#32 (unimplemented)",		/* 32 = unimplemented */
	"access",			/* 33 = access */
	"nice",			/* 34 = nice */
	"#35 (unimplemented)",		/* 35 = unimplemented */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"rename",			/* 38 = rename */
	"mkdir",			/* 39 = mkdir */
	"rmdir",			/* 40 = rmdir */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"times",			/* 43 = times */
	"#44 (unimplemented)",		/* 44 = unimplemented */
	"brk",			/* 45 = brk */
	"setgid",			/* 46 = setgid */
	"getgid",			/* 47 = getgid */
	"signal",			/* 48 = signal */
	"geteuid",			/* 49 = geteuid */
	"getegid",			/* 50 = getegid */
	"acct",			/* 51 = acct */
	"#52 (unimplemented umount)",		/* 52 = unimplemented umount */
	"#53 (unimplemented)",		/* 53 = unimplemented */
	"ioctl",			/* 54 = ioctl */
	"fcntl",			/* 55 = fcntl */
	"#56 (obsolete mpx)",		/* 56 = obsolete mpx */
	"setpgid",			/* 57 = setpgid */
	"#58 (unimplemented)",		/* 58 = unimplemented */
	"olduname",			/* 59 = olduname */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"#62 (unimplemented ustat)",		/* 62 = unimplemented ustat */
	"dup2",			/* 63 = dup2 */
	"getppid",			/* 64 = getppid */
	"getpgrp",			/* 65 = getpgrp */
	"setsid",			/* 66 = setsid */
	"sigaction",			/* 67 = sigaction */
	"siggetmask",			/* 68 = siggetmask */
	"sigsetmask",			/* 69 = sigsetmask */
	"setreuid",			/* 70 = setreuid */
	"setregid",			/* 71 = setregid */
	"sigsuspend",			/* 72 = sigsuspend */
	"sigpending",			/* 73 = sigpending */
	"sethostname",			/* 74 = sethostname */
	"setrlimit",			/* 75 = setrlimit */
	"getrlimit",			/* 76 = getrlimit */
	"getrusage",			/* 77 = getrusage */
	"gettimeofday",			/* 78 = gettimeofday */
	"settimeofday",			/* 79 = settimeofday */
	"getgroups",			/* 80 = getgroups */
	"setgroups",			/* 81 = setgroups */
	"#82 (unimplemented old_select)",		/* 82 = unimplemented old_select */
	"symlink",			/* 83 = symlink */
	"oolstat",			/* 84 = oolstat */
	"readlink",			/* 85 = readlink */
	"#86 (unimplemented uselib)",		/* 86 = unimplemented uselib */
	"swapon",			/* 87 = swapon */
	"reboot",			/* 88 = reboot */
	"readdir",			/* 89 = readdir */
	"mmap",			/* 90 = mmap */
	"munmap",			/* 91 = munmap */
	"truncate",			/* 92 = truncate */
	"ftruncate",			/* 93 = ftruncate */
	"fchmod",			/* 94 = fchmod */
	"__posix_fchown",			/* 95 = __posix_fchown */
	"getpriority",			/* 96 = getpriority */
	"setpriority",			/* 97 = setpriority */
	"#98 (unimplemented)",		/* 98 = unimplemented */
	"statfs",			/* 99 = statfs */
	"fstatfs",			/* 100 = fstatfs */
	"ioperm",			/* 101 = ioperm */
	"socketcall",			/* 102 = socketcall */
	"#103 (unimplemented syslog)",		/* 103 = unimplemented syslog */
	"setitimer",			/* 104 = setitimer */
	"getitimer",			/* 105 = getitimer */
	"stat",			/* 106 = stat */
	"lstat",			/* 107 = lstat */
	"fstat",			/* 108 = fstat */
	"uname",			/* 109 = uname */
	"#110 (unimplemented iopl)",		/* 110 = unimplemented iopl */
	"#111 (unimplemented vhangup)",		/* 111 = unimplemented vhangup */
	"#112 (unimplemented idle)",		/* 112 = unimplemented idle */
	"#113 (unimplemented vm86old)",		/* 113 = unimplemented vm86old */
	"wait4",			/* 114 = wait4 */
	"swapoff",			/* 115 = swapoff */
	"sysinfo",			/* 116 = sysinfo */
	"ipc",			/* 117 = ipc */
	"fsync",			/* 118 = fsync */
	"sigreturn",			/* 119 = sigreturn */
	"clone",			/* 120 = clone */
	"setdomainname",			/* 121 = setdomainname */
	"new_uname",			/* 122 = new_uname */
	"#123 (unimplemented modify_ldt)",		/* 123 = unimplemented modify_ldt */
	"#124 (unimplemented adjtimex)",		/* 124 = unimplemented adjtimex */
	"mprotect",			/* 125 = mprotect */
	"sigprocmask",			/* 126 = sigprocmask */
	"#127 (unimplemented create_module)",		/* 127 = unimplemented create_module */
	"#128 (unimplemented init_module)",		/* 128 = unimplemented init_module */
	"#129 (unimplemented delete_module)",		/* 129 = unimplemented delete_module */
	"#130 (unimplemented get_kernel_syms)",		/* 130 = unimplemented get_kernel_syms */
	"#131 (unimplemented quotactl)",		/* 131 = unimplemented quotactl */
	"getpgid",			/* 132 = getpgid */
	"fchdir",			/* 133 = fchdir */
	"#134 (unimplemented bdflush)",		/* 134 = unimplemented bdflush */
	"#135 (unimplemented sysfs)",		/* 135 = unimplemented sysfs */
	"personality",			/* 136 = personality */
	"#137 (unimplemented afs_syscall)",		/* 137 = unimplemented afs_syscall */
	"setfsuid",			/* 138 = setfsuid */
	"getfsuid",			/* 139 = getfsuid */
	"llseek",			/* 140 = llseek */
	"getdents",			/* 141 = getdents */
	"select",			/* 142 = select */
	"flock",			/* 143 = flock */
	"msync",			/* 144 = msync */
	"readv",			/* 145 = readv */
	"writev",			/* 146 = writev */
	"cacheflush",			/* 147 = cacheflush */
	"#148 (unimplemented cachectl)",		/* 148 = unimplemented cachectl */
	"sysmips",			/* 149 = sysmips */
	"#150 (unimplemented)",		/* 150 = unimplemented */
	"getsid",			/* 151 = getsid */
	"fdatasync",			/* 152 = fdatasync */
	"__sysctl",			/* 153 = __sysctl */
	"mlock",			/* 154 = mlock */
	"munlock",			/* 155 = munlock */
	"mlockall",			/* 156 = mlockall */
	"munlockall",			/* 157 = munlockall */
	"sched_setparam",			/* 158 = sched_setparam */
	"sched_getparam",			/* 159 = sched_getparam */
	"sched_setscheduler",			/* 160 = sched_setscheduler */
	"sched_getscheduler",			/* 161 = sched_getscheduler */
	"sched_yield",			/* 162 = sched_yield */
	"sched_get_priority_max",			/* 163 = sched_get_priority_max */
	"sched_get_priority_min",			/* 164 = sched_get_priority_min */
	"#165 (unimplemented sched_rr_get_interval)",		/* 165 = unimplemented sched_rr_get_interval */
	"nanosleep",			/* 166 = nanosleep */
	"mremap",			/* 167 = mremap */
	"accept",			/* 168 = accept */
	"bind",			/* 169 = bind */
	"connect",			/* 170 = connect */
	"getpeername",			/* 171 = getpeername */
	"getsockname",			/* 172 = getsockname */
	"getsockopt",			/* 173 = getsockopt */
	"listen",			/* 174 = listen */
	"recv",			/* 175 = recv */
	"recvfrom",			/* 176 = recvfrom */
	"recvmsg",			/* 177 = recvmsg */
	"send",			/* 178 = send */
	"sendmsg",			/* 179 = sendmsg */
	"sendto",			/* 180 = sendto */
	"setsockopt",			/* 181 = setsockopt */
	"#182 (unimplemented shutdown)",		/* 182 = unimplemented shutdown */
	"socket",			/* 183 = socket */
	"socketpair",			/* 184 = socketpair */
	"setresuid",			/* 185 = setresuid */
	"getresuid",			/* 186 = getresuid */
	"#187 (unimplemented query_module)",		/* 187 = unimplemented query_module */
	"poll",			/* 188 = poll */
	"#189 (unimplemented nfsservctl)",		/* 189 = unimplemented nfsservctl */
	"setresgid",			/* 190 = setresgid */
	"getresgid",			/* 191 = getresgid */
	"#192 (unimplemented prctl)",		/* 192 = unimplemented prctl */
	"rt_sigreturn",			/* 193 = rt_sigreturn */
	"rt_sigaction",			/* 194 = rt_sigaction */
	"rt_sigprocmask",			/* 195 = rt_sigprocmask */
	"rt_sigpending",			/* 196 = rt_sigpending */
	"#197 (unimplemented rt_sigtimedwait)",		/* 197 = unimplemented rt_sigtimedwait */
	"rt_queueinfo",			/* 198 = rt_queueinfo */
	"rt_sigsuspend",			/* 199 = rt_sigsuspend */
	"pread",			/* 200 = pread */
	"pwrite",			/* 201 = pwrite */
	"chown",			/* 202 = chown */
	"__getcwd",			/* 203 = __getcwd */
	"#204 (unimplemented capget)",		/* 204 = unimplemented capget */
	"#205 (unimplemented capset)",		/* 205 = unimplemented capset */
	"sigaltstack",			/* 206 = sigaltstack */
	"#207 (unimplemented sendfile)",		/* 207 = unimplemented sendfile */
	"#208 (unimplemented)",		/* 208 = unimplemented */
	"#209 (unimplemented)",		/* 209 = unimplemented */
	"#210 (unimplemented mmap2)",		/* 210 = unimplemented mmap2 */
	"truncate64",			/* 211 = truncate64 */
	"#212 (unimplemented ftruncate64)",		/* 212 = unimplemented ftruncate64 */
	"stat64",			/* 213 = stat64 */
	"lstat64",			/* 214 = lstat64 */
	"fstat64",			/* 215 = fstat64 */
	"#216 (unimplemented pivot_root)",		/* 216 = unimplemented pivot_root */
	"#217 (unimplemented mincore)",		/* 217 = unimplemented mincore */
	"#218 (unimplemented modvise)",		/* 218 = unimplemented modvise */
	"getdents64",			/* 219 = getdents64 */
	"fcntl64",			/* 220 = fcntl64 */
};
