/* $NetBSD: darwin_syscall.h,v 1.15 2002/12/24 12:15:45 manu Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.8 2002/12/08 21:53:18 manu Exp 
 */

/* syscall: "syscall" ret: "int" args: "int" "..." */
#define	DARWIN_SYS_syscall	0

/* syscall: "exit" ret: "void" args: "int" */
#define	DARWIN_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	DARWIN_SYS_fork	2

/* syscall: "read" ret: "ssize_t" args: "int" "void *" "size_t" */
#define	DARWIN_SYS_read	3

/* syscall: "write" ret: "ssize_t" args: "int" "const void *" "size_t" */
#define	DARWIN_SYS_write	4

/* syscall: "close" ret: "int" args: "int" */
#define	DARWIN_SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	DARWIN_SYS_wait4	7

/* syscall: "fchdir" ret: "int" args: "int" */
#define	DARWIN_SYS_fchdir	13

/* syscall: "break" ret: "int" args: "char *" */
#define	DARWIN_SYS_break	17

/* syscall: "getfsstat" ret: "int" args: "struct statfs *" "long" "int" */
#define	DARWIN_SYS_getfsstat	18

/* syscall: "olseek" ret: "long" args: "int" "long" "int" */
#define	DARWIN_SYS_olseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	DARWIN_SYS_getpid	20

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	DARWIN_SYS_setuid	23

/* syscall: "getuid" ret: "uid_t" args: */
#define	DARWIN_SYS_getuid	24

/* syscall: "geteuid" ret: "uid_t" args: */
#define	DARWIN_SYS_geteuid	25

/* syscall: "ptrace" ret: "int" args: "int" "pid_t" "caddr_t" "int" */
#define	DARWIN_SYS_ptrace	26

/* syscall: "recvmsg" ret: "ssize_t" args: "int" "struct msghdr *" "int" */
#define	DARWIN_SYS_recvmsg	27

/* syscall: "sendmsg" ret: "ssize_t" args: "int" "const struct msghdr *" "int" */
#define	DARWIN_SYS_sendmsg	28

/* syscall: "recvfrom" ret: "ssize_t" args: "int" "void *" "size_t" "int" "struct sockaddr *" "unsigned int *" */
#define	DARWIN_SYS_recvfrom	29

/* syscall: "accept" ret: "int" args: "int" "struct sockaddr *" "unsigned int *" */
#define	DARWIN_SYS_accept	30

/* syscall: "getpeername" ret: "int" args: "int" "struct sockaddr *" "unsigned int *" */
#define	DARWIN_SYS_getpeername	31

/* syscall: "getsockname" ret: "int" args: "int" "struct sockaddr *" "unsigned int *" */
#define	DARWIN_SYS_getsockname	32

/* syscall: "fchflags" ret: "int" args: "int" "u_long" */
#define	DARWIN_SYS_fchflags	35

/* syscall: "sync" ret: "void" args: */
#define	DARWIN_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_kill	37

/* syscall: "getppid" ret: "pid_t" args: */
#define	DARWIN_SYS_getppid	39

/* syscall: "dup" ret: "int" args: "int" */
#define	DARWIN_SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	DARWIN_SYS_pipe	42

/* syscall: "getegid" ret: "gid_t" args: */
#define	DARWIN_SYS_getegid	43

/* syscall: "profil" ret: "int" args: "caddr_t" "size_t" "u_long" "u_int" */
#define	DARWIN_SYS_profil	44

/* syscall: "ktrace" ret: "int" args: "const char *" "int" "int" "int" */
#define	DARWIN_SYS_ktrace	45

				/* 45 is excluded ktrace */
/* syscall: "sigaction" ret: "int" args: "int" "struct darwin___sigaction *" "struct sigaction13 *" */
#define	DARWIN_SYS_sigaction	46

/* syscall: "getgid" ret: "gid_t" args: */
#define	DARWIN_SYS_getgid	47

/* syscall: "sigprocmask13" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_sigprocmask13	48

/* syscall: "__getlogin" ret: "int" args: "char *" "size_t" */
#define	DARWIN_SYS___getlogin	49

/* syscall: "setlogin" ret: "int" args: "const char *" */
#define	DARWIN_SYS_setlogin	50

/* syscall: "sigpending13" ret: "int" args: */
#define	DARWIN_SYS_sigpending13	52

/* syscall: "sigaltstack13" ret: "int" args: "const struct sigaltstack13 *" "struct sigaltstack13 *" */
#define	DARWIN_SYS_sigaltstack13	53

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "..." */
#define	DARWIN_SYS_ioctl	54

/* syscall: "oreboot" ret: "int" args: "int" */
#define	DARWIN_SYS_oreboot	55

/* syscall: "umask" ret: "mode_t" args: "mode_t" */
#define	DARWIN_SYS_umask	60

/* syscall: "fstat43" ret: "int" args: "int" "struct stat43 *" */
#define	DARWIN_SYS_fstat43	62

/* syscall: "ogetpagesize" ret: "int" args: */
#define	DARWIN_SYS_ogetpagesize	64

/* syscall: "msync" ret: "int" args: "caddr_t" "size_t" */
#define	DARWIN_SYS_msync	65

/* syscall: "vfork" ret: "int" args: */
#define	DARWIN_SYS_vfork	66

				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
/* syscall: "sbrk" ret: "int" args: "intptr_t" */
#define	DARWIN_SYS_sbrk	69

/* syscall: "sstk" ret: "int" args: "int" */
#define	DARWIN_SYS_sstk	70

/* syscall: "ommap" ret: "int" args: "caddr_t" "size_t" "int" "int" "int" "long" */
#define	DARWIN_SYS_ommap	71

/* syscall: "vadvise" ret: "int" args: "int" */
#define	DARWIN_SYS_vadvise	72

/* syscall: "munmap" ret: "int" args: "void *" "size_t" */
#define	DARWIN_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "void *" "size_t" "int" */
#define	DARWIN_SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "void *" "size_t" "int" */
#define	DARWIN_SYS_madvise	75

/* syscall: "mincore" ret: "int" args: "void *" "size_t" "char *" */
#define	DARWIN_SYS_mincore	78

/* syscall: "getgroups" ret: "int" args: "int" "gid_t *" */
#define	DARWIN_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "int" "const gid_t *" */
#define	DARWIN_SYS_setgroups	80

/* syscall: "getpgrp" ret: "int" args: */
#define	DARWIN_SYS_getpgrp	81

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_setpgid	82

/* syscall: "setitimer" ret: "int" args: "int" "const struct itimerval *" "struct itimerval *" */
#define	DARWIN_SYS_setitimer	83

/* syscall: "owait" ret: "int" args: */
#define	DARWIN_SYS_owait	84

/* syscall: "getitimer" ret: "int" args: "int" "struct itimerval *" */
#define	DARWIN_SYS_getitimer	86

/* syscall: "ogethostname" ret: "int" args: "char *" "u_int" */
#define	DARWIN_SYS_ogethostname	87

/* syscall: "osethostname" ret: "int" args: "char *" "u_int" */
#define	DARWIN_SYS_osethostname	88

/* syscall: "ogetdtablesize" ret: "int" args: */
#define	DARWIN_SYS_ogetdtablesize	89

/* syscall: "dup2" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_dup2	90

/* syscall: "fcntl" ret: "int" args: "int" "int" "..." */
#define	DARWIN_SYS_fcntl	92

/* syscall: "select" ret: "int" args: "int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	DARWIN_SYS_select	93

/* syscall: "fsync" ret: "int" args: "int" */
#define	DARWIN_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	DARWIN_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	DARWIN_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "const struct sockaddr *" "unsigned int" */
#define	DARWIN_SYS_connect	98

/* syscall: "oaccept" ret: "int" args: "int" "caddr_t" "int *" */
#define	DARWIN_SYS_oaccept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_getpriority	100

/* syscall: "osend" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	DARWIN_SYS_osend	101

/* syscall: "orecv" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	DARWIN_SYS_orecv	102

/* syscall: "sigreturn" ret: "int" args: "struct darwin_ucontext *" */
#define	DARWIN_SYS_sigreturn	103

/* syscall: "bind" ret: "int" args: "int" "const struct sockaddr *" "unsigned int" */
#define	DARWIN_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "const void *" "unsigned int" */
#define	DARWIN_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_listen	106

/* syscall: "osigvec" ret: "int" args: "int" "struct sigvec *" "struct sigvec *" */
#define	DARWIN_SYS_osigvec	108

/* syscall: "osigblock" ret: "int" args: "int" */
#define	DARWIN_SYS_osigblock	109

/* syscall: "osigsetmask" ret: "int" args: "int" */
#define	DARWIN_SYS_osigsetmask	110

/* syscall: "sigsuspend13" ret: "int" args: "int" */
#define	DARWIN_SYS_sigsuspend13	111

/* syscall: "osigstack" ret: "int" args: "struct sigstack *" "struct sigstack *" */
#define	DARWIN_SYS_osigstack	112

/* syscall: "orecvmsg" ret: "int" args: "int" "struct omsghdr *" "int" */
#define	DARWIN_SYS_orecvmsg	113

/* syscall: "osendmsg" ret: "int" args: "int" "caddr_t" "int" */
#define	DARWIN_SYS_osendmsg	114

/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	DARWIN_SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	DARWIN_SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "unsigned int *" */
#define	DARWIN_SYS_getsockopt	118

/* syscall: "readv" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	DARWIN_SYS_readv	120

/* syscall: "writev" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	DARWIN_SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "const struct timeval *" "const struct timezone *" */
#define	DARWIN_SYS_settimeofday	122

/* syscall: "fchown" ret: "int" args: "int" "uid_t" "gid_t" */
#define	DARWIN_SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "mode_t" */
#define	DARWIN_SYS_fchmod	124

/* syscall: "orecvfrom" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int *" */
#define	DARWIN_SYS_orecvfrom	125

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	DARWIN_SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	DARWIN_SYS_setregid	127

/* syscall: "oftruncate" ret: "int" args: "int" "long" */
#define	DARWIN_SYS_oftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_flock	131

/* syscall: "sendto" ret: "ssize_t" args: "int" "const void *" "size_t" "int" "const struct sockaddr *" "unsigned int" */
#define	DARWIN_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	DARWIN_SYS_socketpair	135

/* syscall: "adjtime" ret: "int" args: "const struct timeval *" "struct timeval *" */
#define	DARWIN_SYS_adjtime	140

/* syscall: "ogetpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	DARWIN_SYS_ogetpeername	141

/* syscall: "ogethostid" ret: "int32_t" args: */
#define	DARWIN_SYS_ogethostid	142

/* syscall: "ogetrlimit" ret: "int" args: "int" "struct orlimit *" */
#define	DARWIN_SYS_ogetrlimit	144

/* syscall: "osetrlimit" ret: "int" args: "int" "const struct orlimit *" */
#define	DARWIN_SYS_osetrlimit	145

/* syscall: "okillpg" ret: "int" args: "int" "int" */
#define	DARWIN_SYS_okillpg	146

/* syscall: "setsid" ret: "int" args: */
#define	DARWIN_SYS_setsid	147

/* syscall: "ogetsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	DARWIN_SYS_ogetsockname	150

/* syscall: "nfssvc" ret: "int" args: "int" "void *" */
#define	DARWIN_SYS_nfssvc	155

				/* 155 is excluded nfssvc */
/* syscall: "ogetdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	DARWIN_SYS_ogetdirentries	156

/* syscall: "fstatfs" ret: "int" args: "int" "struct statfs *" */
#define	DARWIN_SYS_fstatfs	158

				/* 161 is excluded getfh */
/* syscall: "ogetdomainname" ret: "int" args: "char *" "int" */
#define	DARWIN_SYS_ogetdomainname	162

/* syscall: "osetdomainname" ret: "int" args: "char *" "int" */
#define	DARWIN_SYS_osetdomainname	163

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	DARWIN_SYS_setgid	181

/* syscall: "setegid" ret: "int" args: "gid_t" */
#define	DARWIN_SYS_setegid	182

/* syscall: "seteuid" ret: "int" args: "uid_t" */
#define	DARWIN_SYS_seteuid	183

/* syscall: "fstat" ret: "int" args: "int" "struct stat12 *" */
#define	DARWIN_SYS_fstat	189

/* syscall: "fpathconf" ret: "long" args: "int" "int" */
#define	DARWIN_SYS_fpathconf	192

/* syscall: "getrlimit" ret: "int" args: "int" "struct rlimit *" */
#define	DARWIN_SYS_getrlimit	194

/* syscall: "setrlimit" ret: "int" args: "int" "const struct rlimit *" */
#define	DARWIN_SYS_setrlimit	195

/* syscall: "getdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	DARWIN_SYS_getdirentries	196

/* syscall: "mmap" ret: "void *" args: "void *" "size_t" "int" "int" "int" "long" "off_t" */
#define	DARWIN_SYS_mmap	197

/* syscall: "lseek" ret: "off_t" args: "int" "int" "off_t" "int" */
#define	DARWIN_SYS_lseek	199

/* syscall: "ftruncate" ret: "int" args: "int" "int" "off_t" */
#define	DARWIN_SYS_ftruncate	201

/* syscall: "__sysctl" ret: "int" args: "int *" "u_int" "void *" "size_t *" "void *" "size_t" */
#define	DARWIN_SYS___sysctl	202

/* syscall: "mlock" ret: "int" args: "const void *" "size_t" */
#define	DARWIN_SYS_mlock	203

/* syscall: "munlock" ret: "int" args: "const void *" "size_t" */
#define	DARWIN_SYS_munlock	204

/* syscall: "undelete" ret: "int" args: "const char *" */
#define	DARWIN_SYS_undelete	205

/* syscall: "load_shared_file" ret: "int" args: "char *" "caddr_t" "u_long" "caddr_t *" "int" "mach_sf_mapping_t *" "int *" */
#define	DARWIN_SYS_load_shared_file	296

#define	DARWIN_SYS_MAXSYSCALL	350
#define	DARWIN_SYS_NSYSENT	512
