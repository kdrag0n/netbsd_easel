/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.20 1996/12/06 03:25:07 christos Exp 
 */

#define	SVR4_SYS_syscall	0
#define	SVR4_SYS_exit	1
#define	SVR4_SYS_fork	2
#define	SVR4_SYS_read	3
#define	SVR4_SYS_write	4
#define	SVR4_SYS_open	5
#define	SVR4_SYS_close	6
#define	SVR4_SYS_wait	7
#define	SVR4_SYS_creat	8
#define	SVR4_SYS_link	9
#define	SVR4_SYS_unlink	10
#define	SVR4_SYS_execv	11
#define	SVR4_SYS_chdir	12
#define	SVR4_SYS_time	13
#define	SVR4_SYS_mknod	14
#define	SVR4_SYS_chmod	15
#define	SVR4_SYS_chown	16
#define	SVR4_SYS_break	17
#define	SVR4_SYS_stat	18
#define	SVR4_SYS_lseek	19
#define	SVR4_SYS_getpid	20
#define	SVR4_SYS_setuid	23
#define	SVR4_SYS_getuid	24
#define	SVR4_SYS_alarm	27
#define	SVR4_SYS_fstat	28
#define	SVR4_SYS_pause	29
#define	SVR4_SYS_utime	30
#define	SVR4_SYS_access	33
#define	SVR4_SYS_sync	36
#define	SVR4_SYS_kill	37
#define	SVR4_SYS_pgrpsys	39
#define	SVR4_SYS_dup	41
#define	SVR4_SYS_pipe	42
#define	SVR4_SYS_times	43
#define	SVR4_SYS_setgid	46
#define	SVR4_SYS_getgid	47
#define	SVR4_SYS_signal	48
#define	SVR4_SYS_msgsys	49
#define	SVR4_SYS_sysarch	50
#define	SVR4_SYS_shmsys	52
#define	SVR4_SYS_semsys	53
#define	SVR4_SYS_ioctl	54
#define	SVR4_SYS_utssys	57
#define	SVR4_SYS_fsync	58
#define	SVR4_SYS_execve	59
#define	SVR4_SYS_umask	60
#define	SVR4_SYS_chroot	61
#define	SVR4_SYS_fcntl	62
#define	SVR4_SYS_ulimit	63
				/* 70 is obsolete advfs */
				/* 71 is obsolete unadvfs */
				/* 72 is obsolete rmount */
				/* 73 is obsolete rumount */
				/* 74 is obsolete rfstart */
				/* 75 is obsolete sigret */
				/* 76 is obsolete rdebug */
				/* 77 is obsolete rfstop */
#define	SVR4_SYS_rmdir	79
#define	SVR4_SYS_mkdir	80
#define	SVR4_SYS_getdents	81
				/* 82 is obsolete libattach */
				/* 83 is obsolete libdetach */
#define	SVR4_SYS_getmsg	85
#define	SVR4_SYS_putmsg	86
#define	SVR4_SYS_poll	87
#define	SVR4_SYS_lstat	88
#define	SVR4_SYS_symlink	89
#define	SVR4_SYS_readlink	90
#define	SVR4_SYS_getgroups	91
#define	SVR4_SYS_setgroups	92
#define	SVR4_SYS_fchmod	93
#define	SVR4_SYS_fchown	94
#define	SVR4_SYS_sigprocmask	95
#define	SVR4_SYS_sigsuspend	96
#define	SVR4_SYS_sigaltstack	97
#define	SVR4_SYS_sigaction	98
#define	SVR4_SYS_sigpending	99
#define	SVR4_SYS_context	100
#define	SVR4_SYS_statvfs	103
#define	SVR4_SYS_fstatvfs	104
#define	SVR4_SYS_waitsys	107
#define	SVR4_SYS_hrtsys	109
#define	SVR4_SYS_pathconf	113
#define	SVR4_SYS_mmap	115
#define	SVR4_SYS_mprotect	116
#define	SVR4_SYS_munmap	117
#define	SVR4_SYS_fpathconf	118
#define	SVR4_SYS_vfork	119
#define	SVR4_SYS_fchdir	120
#define	SVR4_SYS_readv	121
#define	SVR4_SYS_writev	122
#define	SVR4_SYS_xstat	123
#define	SVR4_SYS_lxstat	124
#define	SVR4_SYS_fxstat	125
#define	SVR4_SYS_xmknod	126
#define	SVR4_SYS_setrlimit	128
#define	SVR4_SYS_getrlimit	129
#define	SVR4_SYS_memcntl	131
#define	SVR4_SYS_rename	134
#define	SVR4_SYS_uname	135
#define	SVR4_SYS_setegid	136
#define	SVR4_SYS_sysconfig	137
#define	SVR4_SYS_adjtime	138
#define	SVR4_SYS_systeminfo	139
#define	SVR4_SYS_seteuid	141
#define	SVR4_SYS_fchroot	153
#define	SVR4_SYS_utimes	154
#define	SVR4_SYS_vhangup	155
#define	SVR4_SYS_gettimeofday	156
#define	SVR4_SYS_getitimer	157
#define	SVR4_SYS_setitimer	158
#define	SVR4_SYS_acl	185
#define	SVR4_SYS_facl	200
#define	SVR4_SYS_setreuid	202
#define	SVR4_SYS_setregid	203
#define	SVR4_SYS_MAXSYSCALL	213
