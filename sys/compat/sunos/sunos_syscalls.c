/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.19 1994/10/26 01:03:43 deraadt Exp 
 */

char *sunos_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"sunos_open",			/* 5 = sunos_open */
	"close",			/* 6 = close */
	"sunos_wait4",			/* 7 = sunos_wait4 */
	"sunos_creat",			/* 8 = sunos_creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"sunos_execv",			/* 11 = sunos_execv */
	"chdir",			/* 12 = chdir */
	"#13 (obsolete time)",		/* 13 = obsolete time */
	"sunos_mknod",			/* 14 = sunos_mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"break",			/* 17 = break */
	"#18 (obsolete stat)",		/* 18 = obsolete stat */
	"compat_43_lseek",			/* 19 = compat_43_lseek */
	"getpid",			/* 20 = getpid */
	"#21 (obsolete sunos_old_mount)",		/* 21 = obsolete sunos_old_mount */
	"#22 (unimplemented System V umount)",		/* 22 = unimplemented System V umount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"#25 (unimplemented sunos_stime)",		/* 25 = unimplemented sunos_stime */
	"#26 (unimplemented sunos_ptrace)",		/* 26 = unimplemented sunos_ptrace */
	"#27 (unimplemented sunos_alarm)",		/* 27 = unimplemented sunos_alarm */
	"#28 (unimplemented sunos_fstat)",		/* 28 = unimplemented sunos_fstat */
	"#29 (unimplemented sunos_pause)",		/* 29 = unimplemented sunos_pause */
	"#30 (unimplemented sunos_utime)",		/* 30 = unimplemented sunos_utime */
	"#31 (unimplemented sunos_stty)",		/* 31 = unimplemented sunos_stty */
	"#32 (unimplemented sunos_gtty)",		/* 32 = unimplemented sunos_gtty */
	"access",			/* 33 = access */
	"#34 (unimplemented sunos_nice)",		/* 34 = unimplemented sunos_nice */
	"#35 (unimplemented sunos_ftime)",		/* 35 = unimplemented sunos_ftime */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"compat_43_stat",			/* 38 = compat_43_stat */
	"#39 (unimplemented sunos_setpgrp)",		/* 39 = unimplemented sunos_setpgrp */
	"compat_43_lstat",			/* 40 = compat_43_lstat */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"#43 (unimplemented sunos_times)",		/* 43 = unimplemented sunos_times */
	"profil",			/* 44 = profil */
	"#45 (unimplemented)",		/* 45 = unimplemented */
	"#46 (unimplemented sunos_setgid)",		/* 46 = unimplemented sunos_setgid */
	"getgid",			/* 47 = getgid */
	"#48 (unimplemented sunos_ssig)",		/* 48 = unimplemented sunos_ssig */
	"#49 (unimplemented reserved for USG)",		/* 49 = unimplemented reserved for USG */
	"#50 (unimplemented reserved for USG)",		/* 50 = unimplemented reserved for USG */
	"acct",			/* 51 = acct */
	"#52 (unimplemented)",		/* 52 = unimplemented */
	"sunos_mctl",			/* 53 = sunos_mctl */
	"sunos_ioctl",			/* 54 = sunos_ioctl */
	"reboot",			/* 55 = reboot */
	"#56 (obsolete sunos_owait3)",		/* 56 = obsolete sunos_owait3 */
	"symlink",			/* 57 = symlink */
	"readlink",			/* 58 = readlink */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"compat_43_fstat",			/* 62 = compat_43_fstat */
	"#63 (unimplemented)",		/* 63 = unimplemented */
	"compat_43_getpagesize",			/* 64 = compat_43_getpagesize */
	"sunos_omsync",			/* 65 = sunos_omsync */
	"vfork",			/* 66 = vfork */
	"#67 (obsolete vread)",		/* 67 = obsolete vread */
	"#68 (obsolete vwrite)",		/* 68 = obsolete vwrite */
	"sbrk",			/* 69 = sbrk */
	"sstk",			/* 70 = sstk */
	"sunos_mmap",			/* 71 = sunos_mmap */
	"vadvise",			/* 72 = vadvise */
	"munmap",			/* 73 = munmap */
	"mprotect",			/* 74 = mprotect */
	"madvise",			/* 75 = madvise */
	"sunos_vhangup",			/* 76 = sunos_vhangup */
	"#77 (unimplemented vlimit)",		/* 77 = unimplemented vlimit */
	"mincore",			/* 78 = mincore */
	"getgroups",			/* 79 = getgroups */
	"setgroups",			/* 80 = setgroups */
	"getpgrp",			/* 81 = getpgrp */
	"sunos_setpgid",			/* 82 = sunos_setpgid */
	"setitimer",			/* 83 = setitimer */
	"#84 (unimplemented { int wait ( void ) ; })",		/* 84 = unimplemented { int wait ( void ) ; } */
	"swapon",			/* 85 = swapon */
	"getitimer",			/* 86 = getitimer */
	"compat_43_gethostname",			/* 87 = compat_43_gethostname */
	"compat_43_sethostname",			/* 88 = compat_43_sethostname */
	"compat_43_getdtablesize",			/* 89 = compat_43_getdtablesize */
	"dup2",			/* 90 = dup2 */
	"#91 (unimplemented getdopt)",		/* 91 = unimplemented getdopt */
	"fcntl",			/* 92 = fcntl */
	"select",			/* 93 = select */
	"#94 (unimplemented setdopt)",		/* 94 = unimplemented setdopt */
	"fsync",			/* 95 = fsync */
	"setpriority",			/* 96 = setpriority */
	"socket",			/* 97 = socket */
	"connect",			/* 98 = connect */
	"compat_43_accept",			/* 99 = compat_43_accept */
	"getpriority",			/* 100 = getpriority */
	"compat_43_send",			/* 101 = compat_43_send */
	"compat_43_recv",			/* 102 = compat_43_recv */
	"#103 (unimplemented old socketaddr)",		/* 103 = unimplemented old socketaddr */
	"bind",			/* 104 = bind */
	"sunos_setsockopt",			/* 105 = sunos_setsockopt */
	"listen",			/* 106 = listen */
	"#107 (unimplemented vtimes)",		/* 107 = unimplemented vtimes */
	"compat_43_sigvec",			/* 108 = compat_43_sigvec */
	"compat_43_sigblock",			/* 109 = compat_43_sigblock */
	"compat_43_sigsetmask",			/* 110 = compat_43_sigsetmask */
	"sigsuspend",			/* 111 = sigsuspend */
	"compat_43_sigstack",			/* 112 = compat_43_sigstack */
	"compat_43_recvmsg",			/* 113 = compat_43_recvmsg */
	"compat_43_sendmsg",			/* 114 = compat_43_sendmsg */
	"#115 (obsolete vtrace)",		/* 115 = obsolete vtrace */
	"gettimeofday",			/* 116 = gettimeofday */
	"getrusage",			/* 117 = getrusage */
	"getsockopt",			/* 118 = getsockopt */
	"#119 (unimplemented)",		/* 119 = unimplemented */
	"readv",			/* 120 = readv */
	"writev",			/* 121 = writev */
	"settimeofday",			/* 122 = settimeofday */
	"fchown",			/* 123 = fchown */
	"fchmod",			/* 124 = fchmod */
	"compat_43_recvfrom",			/* 125 = compat_43_recvfrom */
	"compat_43_setreuid",			/* 126 = compat_43_setreuid */
	"compat_43_setregid",			/* 127 = compat_43_setregid */
	"rename",			/* 128 = rename */
	"compat_43_truncate",			/* 129 = compat_43_truncate */
	"compat_43_ftruncate",			/* 130 = compat_43_ftruncate */
	"flock",			/* 131 = flock */
	"#132 (unimplemented)",		/* 132 = unimplemented */
	"sendto",			/* 133 = sendto */
	"shutdown",			/* 134 = shutdown */
	"socketpair",			/* 135 = socketpair */
	"mkdir",			/* 136 = mkdir */
	"rmdir",			/* 137 = rmdir */
	"utimes",			/* 138 = utimes */
	"sigreturn",			/* 139 = sigreturn */
	"adjtime",			/* 140 = adjtime */
	"compat_43_getpeername",			/* 141 = compat_43_getpeername */
	"compat_43_gethostid",			/* 142 = compat_43_gethostid */
	"#143 (unimplemented old sethostid)",		/* 143 = unimplemented old sethostid */
	"sunos_getrlimit",			/* 144 = sunos_getrlimit */
	"sunos_setrlimit",			/* 145 = sunos_setrlimit */
	"compat_43_killpg",			/* 146 = compat_43_killpg */
	"#147 (unimplemented)",		/* 147 = unimplemented */
	"#148 (unimplemented)",		/* 148 = unimplemented */
	"#149 (unimplemented)",		/* 149 = unimplemented */
	"compat_43_getsockname",			/* 150 = compat_43_getsockname */
	"#151 (unimplemented getmsg)",		/* 151 = unimplemented getmsg */
	"#152 (unimplemented putmsg)",		/* 152 = unimplemented putmsg */
	"#153 (unimplemented poll)",		/* 153 = unimplemented poll */
	"#154 (unimplemented)",		/* 154 = unimplemented */
#ifdef NFSSERVER
	"sunos_nfssvc",			/* 155 = sunos_nfssvc */
#else
	"#155 (unimplemented)",		/* 155 = unimplemented */
#endif
	"getdirentries",			/* 156 = getdirentries */
	"sunos_statfs",			/* 157 = sunos_statfs */
	"sunos_fstatfs",			/* 158 = sunos_fstatfs */
	"sunos_unmount",			/* 159 = sunos_unmount */
#ifdef NFSCLIENT
	"async_daemon",			/* 160 = async_daemon */
	"getfh",			/* 161 = getfh */
#else
	"#160 (unimplemented)",		/* 160 = unimplemented */
	"#161 (unimplemented)",		/* 161 = unimplemented */
#endif
	"compat_09_getdomainname",			/* 162 = compat_09_getdomainname */
	"compat_09_setdomainname",			/* 163 = compat_09_setdomainname */
	"#164 (unimplemented rtschedule)",		/* 164 = unimplemented rtschedule */
	"sunos_quotactl",			/* 165 = sunos_quotactl */
	"sunos_exportfs",			/* 166 = sunos_exportfs */
	"sunos_mount",			/* 167 = sunos_mount */
	"sunos_ustat",			/* 168 = sunos_ustat */
#ifdef SYSVSEM
	"semsys",			/* 169 = semsys */
#else
	"#169 (unimplemented 1.0 semsys)",		/* 169 = unimplemented 1.0 semsys */
#endif
#ifdef SYSVMSG
	"msgsys",			/* 170 = msgsys */
#else
	"#170 (unimplemented 1.0 msgsys)",		/* 170 = unimplemented 1.0 msgsys */
#endif
#ifdef SYSVSHM
	"shmsys",			/* 171 = shmsys */
#else
	"#171 (unimplemented 1.0 shmsys)",		/* 171 = unimplemented 1.0 shmsys */
#endif
	"sunos_auditsys",			/* 172 = sunos_auditsys */
	"#173 (unimplemented rfssys)",		/* 173 = unimplemented rfssys */
	"sunos_getdents",			/* 174 = sunos_getdents */
	"setsid",			/* 175 = setsid */
	"fchdir",			/* 176 = fchdir */
	"sunos_fchroot",			/* 177 = sunos_fchroot */
	"#178 (unimplemented vpixsys)",		/* 178 = unimplemented vpixsys */
	"#179 (unimplemented aioread)",		/* 179 = unimplemented aioread */
	"#180 (unimplemented aiowrite)",		/* 180 = unimplemented aiowrite */
	"#181 (unimplemented aiowait)",		/* 181 = unimplemented aiowait */
	"#182 (unimplemented aiocancel)",		/* 182 = unimplemented aiocancel */
	"sunos_sigpending",			/* 183 = sunos_sigpending */
	"#184 (unimplemented)",		/* 184 = unimplemented */
	"setpgid",			/* 185 = setpgid */
	"#186 (unimplemented { long pathconf ( char * path , int name ) ; })",		/* 186 = unimplemented { long pathconf ( char * path , int name ) ; } */
	"#187 (unimplemented { long fpathconf ( int fd , int name ) ; })",		/* 187 = unimplemented { long fpathconf ( int fd , int name ) ; } */
	"sunos_sysconf",			/* 188 = sunos_sysconf */
	"sunos_uname",			/* 189 = sunos_uname */
};
