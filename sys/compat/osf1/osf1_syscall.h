/* $NetBSD: osf1_syscall.h,v 1.41 2000/12/13 01:29:35 mycroft Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.34 2000/12/09 07:10:36 mycroft Exp 
 */

/* syscall: "syscall" ret: "int" args: */
#define	OSF1_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	OSF1_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	OSF1_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	OSF1_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "const char *" "u_int" */
#define	OSF1_SYS_write	4

/* syscall: "close" ret: "int" args: "int" */
#define	OSF1_SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct osf1_rusage *" */
#define	OSF1_SYS_wait4	7

/* syscall: "link" ret: "int" args: "const char *" "const char *" */
#define	OSF1_SYS_link	9

/* syscall: "unlink" ret: "int" args: "const char *" */
#define	OSF1_SYS_unlink	10

/* syscall: "chdir" ret: "int" args: "const char *" */
#define	OSF1_SYS_chdir	12

/* syscall: "fchdir" ret: "int" args: "int" */
#define	OSF1_SYS_fchdir	13

/* syscall: "mknod" ret: "int" args: "const char *" "int" "int" */
#define	OSF1_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "const char *" "int" */
#define	OSF1_SYS_chmod	15

/* syscall: "__posix_chown" ret: "int" args: "const char *" "int" "int" */
#define	OSF1_SYS___posix_chown	16

/* syscall: "obreak" ret: "int" args: "char *" */
#define	OSF1_SYS_obreak	17

/* syscall: "getfsstat" ret: "int" args: "struct osf1_statfs *" "long" "int" */
#define	OSF1_SYS_getfsstat	18

/* syscall: "lseek" ret: "off_t" args: "int" "off_t" "int" */
#define	OSF1_SYS_lseek	19

/* syscall: "getpid_with_ppid" ret: "pid_t" args: */
#define	OSF1_SYS_getpid_with_ppid	20

/* syscall: "mount" ret: "int" args: "int" "const char *" "int" "caddr_t" */
#define	OSF1_SYS_mount	21

/* syscall: "unmount" ret: "int" args: "const char *" "int" */
#define	OSF1_SYS_unmount	22

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	OSF1_SYS_setuid	23

/* syscall: "getuid_with_euid" ret: "uid_t" args: */
#define	OSF1_SYS_getuid_with_euid	24

/* syscall: "recvmsg_xopen" ret: "int" args: "int" "struct osf1_msghdr_xopen *" "int" */
#define	OSF1_SYS_recvmsg_xopen	27

/* syscall: "sendmsg_xopen" ret: "int" args: "int" "const struct osf1_msghdr_xopen *" "int" */
#define	OSF1_SYS_sendmsg_xopen	28

/* syscall: "access" ret: "int" args: "const char *" "int" */
#define	OSF1_SYS_access	33

/* syscall: "sync" ret: "int" args: */
#define	OSF1_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	OSF1_SYS_kill	37

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	OSF1_SYS_setpgid	39

/* syscall: "dup" ret: "int" args: "u_int" */
#define	OSF1_SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	OSF1_SYS_pipe	42

/* syscall: "set_program_attributes" ret: "int" args: "caddr_t" "unsigned long" "caddr_t" "unsigned long" */
#define	OSF1_SYS_set_program_attributes	43

/* syscall: "open" ret: "int" args: "const char *" "int" "int" */
#define	OSF1_SYS_open	45

				/* 46 is obsolete sigaction */
/* syscall: "getgid_with_egid" ret: "gid_t" args: */
#define	OSF1_SYS_getgid_with_egid	47

/* syscall: "sigprocmask" ret: "int" args: "int" "sigset13_t" */
#define	OSF1_SYS_sigprocmask	48

/* syscall: "__getlogin" ret: "int" args: "char *" "u_int" */
#define	OSF1_SYS___getlogin	49

/* syscall: "setlogin" ret: "int" args: "const char *" */
#define	OSF1_SYS_setlogin	50

/* syscall: "acct" ret: "int" args: "const char *" */
#define	OSF1_SYS_acct	51

/* syscall: "classcntl" ret: "int" args: "int" "int" "int" "int" */
#define	OSF1_SYS_classcntl	53

/* syscall: "ioctl" ret: "int" args: "int" "int" "caddr_t" */
#define	OSF1_SYS_ioctl	54

/* syscall: "reboot" ret: "int" args: "int" */
#define	OSF1_SYS_reboot	55

/* syscall: "revoke" ret: "int" args: "const char *" */
#define	OSF1_SYS_revoke	56

/* syscall: "symlink" ret: "int" args: "const char *" "const char *" */
#define	OSF1_SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "const char *" "char *" "int" */
#define	OSF1_SYS_readlink	58

/* syscall: "execve" ret: "int" args: "const char *" "char *const *" "char *const *" */
#define	OSF1_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	OSF1_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "const char *" */
#define	OSF1_SYS_chroot	61

/* syscall: "getpgrp" ret: "int" args: */
#define	OSF1_SYS_getpgrp	63

/* syscall: "getpagesize" ret: "int" args: */
#define	OSF1_SYS_getpagesize	64

/* syscall: "vfork" ret: "int" args: */
#define	OSF1_SYS_vfork	66

/* syscall: "stat" ret: "int" args: "const char *" "struct osf1_stat *" */
#define	OSF1_SYS_stat	67

/* syscall: "lstat" ret: "int" args: "const char *" "struct osf1_stat *" */
#define	OSF1_SYS_lstat	68

/* syscall: "mmap" ret: "caddr_t" args: "caddr_t" "size_t" "int" "int" "int" "off_t" */
#define	OSF1_SYS_mmap	71

/* syscall: "munmap" ret: "int" args: "caddr_t" "size_t" */
#define	OSF1_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "void *" "size_t" "int" */
#define	OSF1_SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "void *" "size_t" "int" */
#define	OSF1_SYS_madvise	75

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	OSF1_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	OSF1_SYS_setgroups	80

/* syscall: "setpgrp" ret: "int" args: "int" "int" */
#define	OSF1_SYS_setpgrp	82

/* syscall: "setitimer" ret: "int" args: "u_int" "struct osf1_itimerval *" "struct osf1_itimerval *" */
#define	OSF1_SYS_setitimer	83

/* syscall: "gethostname" ret: "int" args: "char *" "u_int" */
#define	OSF1_SYS_gethostname	87

/* syscall: "sethostname" ret: "int" args: "const char *" "u_int" */
#define	OSF1_SYS_sethostname	88

/* syscall: "getdtablesize" ret: "int" args: */
#define	OSF1_SYS_getdtablesize	89

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	OSF1_SYS_dup2	90

/* syscall: "fstat" ret: "int" args: "int" "void *" */
#define	OSF1_SYS_fstat	91

/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	OSF1_SYS_fcntl	92

/* syscall: "select" ret: "int" args: "u_int" "fd_set *" "fd_set *" "fd_set *" "struct osf1_timeval *" */
#define	OSF1_SYS_select	93

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	OSF1_SYS_poll	94

/* syscall: "fsync" ret: "int" args: "int" */
#define	OSF1_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	OSF1_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	OSF1_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "caddr_t" "int" */
#define	OSF1_SYS_connect	98

/* syscall: "accept" ret: "int" args: "int" "caddr_t" "int *" */
#define	OSF1_SYS_accept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	OSF1_SYS_getpriority	100

/* syscall: "send" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	OSF1_SYS_send	101

/* syscall: "recv" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	OSF1_SYS_recv	102

/* syscall: "sigreturn" ret: "int" args: "struct sigcontext13 *" */
#define	OSF1_SYS_sigreturn	103

/* syscall: "bind" ret: "int" args: "int" "caddr_t" "int" */
#define	OSF1_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int" */
#define	OSF1_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	OSF1_SYS_listen	106

/* syscall: "sigsuspend" ret: "int" args: "int" */
#define	OSF1_SYS_sigsuspend	111

/* syscall: "sigstack" ret: "int" args: "struct sigstack *" "struct sigstack *" */
#define	OSF1_SYS_sigstack	112

				/* 115 is obsolete vtrace */
/* syscall: "gettimeofday" ret: "int" args: "struct osf1_timeval *" "struct osf1_timezone *" */
#define	OSF1_SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct osf1_rusage *" */
#define	OSF1_SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int *" */
#define	OSF1_SYS_getsockopt	118

/* syscall: "readv" ret: "int" args: "int" "struct osf1_iovec *" "u_int" */
#define	OSF1_SYS_readv	120

/* syscall: "writev" ret: "int" args: "int" "struct osf1_iovec *" "u_int" */
#define	OSF1_SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "struct osf1_timeval *" "struct osf1_timezone *" */
#define	OSF1_SYS_settimeofday	122

/* syscall: "__posix_fchown" ret: "int" args: "int" "int" "int" */
#define	OSF1_SYS___posix_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	OSF1_SYS_fchmod	124

/* syscall: "recvfrom" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int *" */
#define	OSF1_SYS_recvfrom	125

/* syscall: "__posix_rename" ret: "int" args: "const char *" "const char *" */
#define	OSF1_SYS___posix_rename	128

/* syscall: "truncate" ret: "int" args: "const char *" "off_t" */
#define	OSF1_SYS_truncate	129

/* syscall: "ftruncate" ret: "int" args: "int" "off_t" */
#define	OSF1_SYS_ftruncate	130

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	OSF1_SYS_setgid	132

/* syscall: "sendto" ret: "int" args: "int" "caddr_t" "size_t" "int" "struct sockaddr *" "int" */
#define	OSF1_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	OSF1_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	OSF1_SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "const char *" "int" */
#define	OSF1_SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "const char *" */
#define	OSF1_SYS_rmdir	137

/* syscall: "utimes" ret: "int" args: "const char *" "const struct osf1_timeval *" */
#define	OSF1_SYS_utimes	138

				/* 139 is obsolete 4.2 sigreturn */
/* syscall: "getpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	OSF1_SYS_getpeername	141

/* syscall: "gethostid" ret: "int32_t" args: */
#define	OSF1_SYS_gethostid	142

/* syscall: "sethostid" ret: "int" args: "int32_t" */
#define	OSF1_SYS_sethostid	143

/* syscall: "getrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	OSF1_SYS_getrlimit	144

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct rlimit *" */
#define	OSF1_SYS_setrlimit	145

/* syscall: "setsid" ret: "int" args: */
#define	OSF1_SYS_setsid	147

/* syscall: "quota" ret: "int" args: */
#define	OSF1_SYS_quota	149

/* syscall: "getsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	OSF1_SYS_getsockname	150

/* syscall: "sigaction" ret: "int" args: "int" "struct osf1_sigaction *" "struct osf1_sigaction *" */
#define	OSF1_SYS_sigaction	156

/* syscall: "getdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	OSF1_SYS_getdirentries	159

/* syscall: "statfs" ret: "int" args: "const char *" "struct osf1_statfs *" "int" */
#define	OSF1_SYS_statfs	160

/* syscall: "fstatfs" ret: "int" args: "int" "struct osf1_statfs *" "int" */
#define	OSF1_SYS_fstatfs	161

/* syscall: "getdomainname" ret: "int" args: "char *" "int" */
#define	OSF1_SYS_getdomainname	165

/* syscall: "setdomainname" ret: "int" args: "char *" "int" */
#define	OSF1_SYS_setdomainname	166

/* syscall: "uname" ret: "int" args: "struct osf1_uname *" */
#define	OSF1_SYS_uname	207

/* syscall: "__posix_lchown" ret: "int" args: "const char *" "int" "int" */
#define	OSF1_SYS___posix_lchown	208

/* syscall: "shmat" ret: "void *" args: "int" "const void *" "int" */
#define	OSF1_SYS_shmat	209

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct osf1_shmid_ds *" */
#define	OSF1_SYS_shmctl	210

/* syscall: "shmdt" ret: "int" args: "const void *" */
#define	OSF1_SYS_shmdt	211

/* syscall: "shmget" ret: "int" args: "osf1_key_t" "size_t" "int" */
#define	OSF1_SYS_shmget	212

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	OSF1_SYS_getsid	234

/* syscall: "sigaltstack" ret: "int" args: "struct osf1_sigaltstack *" "struct osf1_sigaltstack *" */
#define	OSF1_SYS_sigaltstack	235

/* syscall: "sysinfo" ret: "int" args: "int" "char *" "long" */
#define	OSF1_SYS_sysinfo	241

/* syscall: "pathconf" ret: "long" args: "const char *" "int" */
#define	OSF1_SYS_pathconf	247

/* syscall: "fpathconf" ret: "long" args: "int" "int" */
#define	OSF1_SYS_fpathconf	248

/* syscall: "usleep_thread" ret: "int" args: "struct osf1_timeval *" "struct osf1_timeval *" */
#define	OSF1_SYS_usleep_thread	251

/* syscall: "getsysinfo" ret: "int" args: "u_long" "caddr_t" "u_long" "caddr_t" "u_long" */
#define	OSF1_SYS_getsysinfo	256

/* syscall: "setsysinfo" ret: "int" args: "u_long" "caddr_t" "u_long" "caddr_t" "u_long" */
#define	OSF1_SYS_setsysinfo	257

#define	OSF1_SYS_MAXSYSCALL	262
#define	OSF1_SYS_NSYSENT	512
