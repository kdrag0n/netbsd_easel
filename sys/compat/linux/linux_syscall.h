/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.4 1995/04/07 22:23:28 fvdl Exp 
 */

#define	LINUX_SYS_syscall	0
#define	LINUX_SYS_exit	1
#define	LINUX_SYS_linux_fork	2
#define	LINUX_SYS_read	3
#define	LINUX_SYS_write	4
#define	LINUX_SYS_linux_open	5
#define	LINUX_SYS_close	6
#define	LINUX_SYS_linux_waitpid	7
#define	LINUX_SYS_linux_creat	8
#define	LINUX_SYS_link	9
#define	LINUX_SYS_linux_unlink	10
#define	LINUX_SYS_linux_execve	11
#define	LINUX_SYS_linux_chdir	12
#define	LINUX_SYS_linux_time	13
#define	LINUX_SYS_linux_mknod	14
#define	LINUX_SYS_linux_chmod	15
#define	LINUX_SYS_linux_chown	16
#define	LINUX_SYS_linux_break	17
				/* 18 is obsolete linux_ostat */
#define	LINUX_SYS_compat_43_lseek	19
#define	LINUX_SYS_getpid	20
#define	LINUX_SYS_setuid	23
#define	LINUX_SYS_getuid	24
#define	LINUX_SYS_linux_alarm	27
				/* 28 is obsolete linux_ofstat */
#define	LINUX_SYS_linux_pause	29
#define	LINUX_SYS_linux_utime	30
#define	LINUX_SYS_linux_access	33
#define	LINUX_SYS_sync	36
#define	LINUX_SYS_linux_kill	37
#define	LINUX_SYS_linux_rename	38
#define	LINUX_SYS_linux_mkdir	39
#define	LINUX_SYS_linux_rmdir	40
#define	LINUX_SYS_dup	41
#define	LINUX_SYS_linux_pipe	42
#define	LINUX_SYS_linux_times	43
#define	LINUX_SYS_linux_brk	45
#define	LINUX_SYS_setgid	46
#define	LINUX_SYS_getgid	47
#define	LINUX_SYS_linux_signal	48
#define	LINUX_SYS_geteuid	49
#define	LINUX_SYS_getegid	50
#define	LINUX_SYS_acct	51
#define	LINUX_SYS_linux_ioctl	54
#define	LINUX_SYS_linux_fcntl	55
#define	LINUX_SYS_setpgid	57
#define	LINUX_SYS_umask	60
#define	LINUX_SYS_chroot	61
#define	LINUX_SYS_dup2	63
#define	LINUX_SYS_getppid	64
#define	LINUX_SYS_getpgrp	65
#define	LINUX_SYS_setsid	66
#define	LINUX_SYS_linux_sigaction	67
#define	LINUX_SYS_linux_siggetmask	68
#define	LINUX_SYS_linux_sigsetmask	69
#define	LINUX_SYS_compat_43_setreuid	70
#define	LINUX_SYS_compat_43_setregid	71
#define	LINUX_SYS_linux_sigsuspend	72
#define	LINUX_SYS_linux_sigpending	73
#define	LINUX_SYS_compat_43_sethostname	74
#define	LINUX_SYS_compat_43_setrlimit	75
#define	LINUX_SYS_compat_43_getrlimit	76
#define	LINUX_SYS_getrusage	77
#define	LINUX_SYS_gettimeofday	78
#define	LINUX_SYS_settimeofday	79
#define	LINUX_SYS_getgroups	80
#define	LINUX_SYS_setgroups	81
#define	LINUX_SYS_linux_select	82
#define	LINUX_SYS_linux_symlink	83
#define	LINUX_SYS_compat_43_lstat	84
#define	LINUX_SYS_linux_readlink	85
#define	LINUX_SYS_linux_uselib	86
#define	LINUX_SYS_swapon	87
#define	LINUX_SYS_reboot	88
#define	LINUX_SYS_linux_readdir	89
#define	LINUX_SYS_linux_mmap	90
#define	LINUX_SYS_munmap	91
#define	LINUX_SYS_linux_truncate	92
#define	LINUX_SYS_compat_43_ftruncate	93
#define	LINUX_SYS_fchmod	94
#define	LINUX_SYS_fchown	95
#define	LINUX_SYS_getpriority	96
#define	LINUX_SYS_setpriority	97
#define	LINUX_SYS_profil	98
#define	LINUX_SYS_linux_statfs	99
#define	LINUX_SYS_linux_fstatfs	100
#define	LINUX_SYS_linux_socketcall	102
#define	LINUX_SYS_setitimer	104
#define	LINUX_SYS_getitimer	105
#define	LINUX_SYS_linux_stat	106
#define	LINUX_SYS_linux_lstat	107
#define	LINUX_SYS_linux_fstat	108
#define	LINUX_SYS_linux_wait4	114
#define	LINUX_SYS_linux_ipc	117
#define	LINUX_SYS_fsync	118
#define	LINUX_SYS_linux_sigreturn	119
#define	LINUX_SYS_compat_09_setdomainname	121
#define	LINUX_SYS_linux_uname	122
#define	LINUX_SYS_mprotect	125
#define	LINUX_SYS_linux_sigprocmask	126
#define	LINUX_SYS_linux_getpgid	132
#define	LINUX_SYS_fchdir	133
#define	LINUX_SYS_linux_llseek	140
