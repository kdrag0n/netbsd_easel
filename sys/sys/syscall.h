/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.23 1994/10/20 04:23:12 cgd Exp 
 */

#define	SYS_syscall	0
#define	SYS_exit	1
#define	SYS_fork	2
#define	SYS_read	3
#define	SYS_write	4
#define	SYS_open	5
#define	SYS_close	6
#define	SYS_wait4	7
				/* 8 is compat_43 creat */
#define	SYS_link	9
#define	SYS_unlink	10
				/* 11 is obsolete execv */
#define	SYS_chdir	12
#define	SYS_fchdir	13
#define	SYS_mknod	14
#define	SYS_chmod	15
#define	SYS_chown	16
#define	SYS_break	17
#define	SYS_getfsstat	18
				/* 19 is compat_43 lseek */
#define	SYS_getpid	20
#define	SYS_mount	21
#define	SYS_unmount	22
#define	SYS_setuid	23
#define	SYS_getuid	24
#define	SYS_geteuid	25
#define	SYS_ptrace	26
#define	SYS_recvmsg	27
#define	SYS_sendmsg	28
#define	SYS_recvfrom	29
#define	SYS_accept	30
#define	SYS_getpeername	31
#define	SYS_getsockname	32
#define	SYS_access	33
#define	SYS_chflags	34
#define	SYS_fchflags	35
#define	SYS_sync	36
#define	SYS_kill	37
				/* 38 is compat_43 stat */
#define	SYS_getppid	39
				/* 40 is compat_43 lstat */
#define	SYS_dup	41
#define	SYS_pipe	42
#define	SYS_getegid	43
#define	SYS_profil	44
#define	SYS_ktrace	45
#define	SYS_sigaction	46
#define	SYS_getgid	47
#define	SYS_sigprocmask	48
#define	SYS_getlogin	49
#define	SYS_setlogin	50
#define	SYS_acct	51
#define	SYS_sigpending	52
#define	SYS_sigaltstack	53
#define	SYS_ioctl	54
#define	SYS_reboot	55
#define	SYS_revoke	56
#define	SYS_symlink	57
#define	SYS_readlink	58
#define	SYS_execve	59
#define	SYS_umask	60
#define	SYS_chroot	61
				/* 62 is compat_43 fstat */
				/* 63 is compat_43 getkerninfo */
				/* 64 is compat_43 getpagesize */
#define	SYS_msync	65
#define	SYS_vfork	66
				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
#define	SYS_sbrk	69
#define	SYS_sstk	70
				/* 71 is compat_43 mmap */
#define	SYS_vadvise	72
#define	SYS_munmap	73
#define	SYS_mprotect	74
#define	SYS_madvise	75
				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
#define	SYS_mincore	78
#define	SYS_getgroups	79
#define	SYS_setgroups	80
#define	SYS_getpgrp	81
#define	SYS_setpgid	82
#define	SYS_setitimer	83
				/* 84 is compat_43 wait */
#define	SYS_swapon	85
#define	SYS_getitimer	86
				/* 87 is compat_43 gethostname */
				/* 88 is compat_43 sethostname */
				/* 89 is compat_43 getdtablesize */
#define	SYS_dup2	90
#define	SYS_fcntl	92
#define	SYS_select	93
#define	SYS_fsync	95
#define	SYS_setpriority	96
#define	SYS_socket	97
#define	SYS_connect	98
				/* 99 is compat_43 accept */
#define	SYS_getpriority	100
				/* 101 is compat_43 send */
				/* 102 is compat_43 recv */
#define	SYS_sigreturn	103
#define	SYS_bind	104
#define	SYS_setsockopt	105
#define	SYS_listen	106
				/* 107 is obsolete vtimes */
				/* 108 is compat_43 sigvec */
				/* 109 is compat_43 sigblock */
				/* 110 is compat_43 sigsetmask */
#define	SYS_sigsuspend	111
				/* 112 is compat_43 sigstack */
				/* 113 is compat_43 recvmsg */
				/* 114 is compat_43 sendmsg */
#define	SYS_vtrace	115
				/* 115 is obsolete vtrace */
#define	SYS_gettimeofday	116
#define	SYS_getrusage	117
#define	SYS_getsockopt	118
#define	SYS_resuba	119
#define	SYS_readv	120
#define	SYS_writev	121
#define	SYS_settimeofday	122
#define	SYS_fchown	123
#define	SYS_fchmod	124
				/* 125 is compat_43 recvfrom */
				/* 126 is compat_43 setreuid */
				/* 127 is compat_43 setregid */
#define	SYS_rename	128
				/* 129 is compat_43 truncate */
				/* 130 is compat_43 ftruncate */
#define	SYS_flock	131
#define	SYS_mkfifo	132
#define	SYS_sendto	133
#define	SYS_shutdown	134
#define	SYS_socketpair	135
#define	SYS_mkdir	136
#define	SYS_rmdir	137
#define	SYS_utimes	138
				/* 139 is obsolete 4.2 sigreturn */
#define	SYS_adjtime	140
				/* 141 is compat_43 getpeername */
				/* 142 is compat_43 gethostid */
				/* 143 is compat_43 sethostid */
				/* 144 is compat_43 getrlimit */
				/* 145 is compat_43 setrlimit */
				/* 146 is compat_43 killpg */
#define	SYS_setsid	147
#define	SYS_quotactl	148
				/* 149 is compat_43 quota */
				/* 150 is compat_43 getsockname */
#define	SYS_nfssvc	155
				/* 156 is compat_43 getdirentries */
#define	SYS_statfs	157
#define	SYS_fstatfs	158
#define	SYS_getfh	161
				/* 162 is compat_09 getdomainname */
				/* 163 is compat_09 setdomainname */
				/* 164 is compat_09 uname */
#define	SYS_sysarch	165
				/* 169 is compat_10 semsys */
				/* 170 is compat_10 msgsys */
				/* 171 is compat_10 shmsys */
#define	SYS_setgid	181
#define	SYS_setegid	182
#define	SYS_seteuid	183
#define	SYS_lfs_bmapv	184
#define	SYS_lfs_markv	185
#define	SYS_lfs_segclean	186
#define	SYS_lfs_segwait	187
#define	SYS_stat	188
#define	SYS_fstat	189
#define	SYS_lstat	190
#define	SYS_pathconf	191
#define	SYS_fpathconf	192
#define	SYS_getrlimit	194
#define	SYS_setrlimit	195
#define	SYS_getdirentries	196
#define	SYS_mmap	197
#define	SYS___syscall	198
#define	SYS_lseek	199
#define	SYS_truncate	200
#define	SYS_ftruncate	201
#define	SYS___sysctl	202
#define	SYS_mlock	203
#define	SYS_munlock	204
#define	SYS___semctl	220
#define	SYS_semget	221
#define	SYS_semop	222
#define	SYS_semconfig	223
#define	SYS_msgctl	224
#define	SYS_msgget	225
#define	SYS_msgsnd	226
#define	SYS_msgrcv	227
#define	SYS_shmat	228
#define	SYS_shmctl	229
#define	SYS_shmdt	230
#define	SYS_shmget	231
