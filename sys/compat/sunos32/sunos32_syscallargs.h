/* $NetBSD: sunos32_syscallargs.h,v 1.7 2002/01/03 01:11:30 mrg Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from;	NetBSD: syscalls.master,v 1.6 2002/01/03 00:59:01 mrg Exp 
 */

#ifndef _SUNOS32_SYS__SYSCALLARGS_H_
#define	_SUNOS32_SYS__SYSCALLARGS_H_

#ifdef	syscallarg
#undef	syscallarg
#endif

#define	syscallarg(x)							\
	union {								\
		register32_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register32_t) < sizeof (x))	\
				? 0					\
				: sizeof (register32_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

struct sunos32_sys_open_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct sunos32_sys_wait4_args {
	syscallarg(int) pid;
	syscallarg(netbsd32_intp) status;
	syscallarg(int) options;
	syscallarg(netbsd32_rusagep_t) rusage;
};

struct sunos32_sys_creat_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) mode;
};

struct sunos32_sys_execv_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(netbsd32_charpp) argp;
};

struct sunos32_sys_mknod_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct sunos32_sys_stime_args {
	syscallarg(sunos32_time_tp) tp;
};

struct sunos32_sys_ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(netbsd32_caddr_t) addr;
	syscallarg(int) data;
	syscallarg(netbsd32_charp) addr2;
};

struct sunos32_sys_access_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(int) flags;
};

struct sunos32_sys_stat_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(netbsd32_stat43p_t) ub;
};

struct sunos32_sys_lstat_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(netbsd32_stat43p_t) ub;
};

struct sunos32_sys_mctl_args {
	syscallarg(netbsd32_voidp) addr;
	syscallarg(int) len;
	syscallarg(int) func;
	syscallarg(netbsd32_voidp) arg;
};

struct sunos32_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_u_long) com;
	syscallarg(netbsd32_caddr_t) data;
};

struct sunos32_sys_reboot_args {
	syscallarg(int) howto;
	syscallarg(netbsd32_charp) bootstr;
};

struct sunos32_sys_execve_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(netbsd32_charpp) argp;
	syscallarg(netbsd32_charpp) envp;
};

struct sunos32_sys_omsync_args {
	syscallarg(netbsd32_caddr_t) addr;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) flags;
};

struct sunos32_sys_mmap_args {
	syscallarg(netbsd32_voidp) addr;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(netbsd32_long) pos;
};

struct sunos32_sys_setpgrp_args {
	syscallarg(int) pid;
	syscallarg(int) pgid;
};

struct sunos32_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};

struct sunos32_sys_setsockopt_args {
	syscallarg(int) s;
	syscallarg(int) level;
	syscallarg(int) name;
	syscallarg(netbsd32_caddr_t) val;
	syscallarg(int) valsize;
};

struct sunos32_sys_sigvec_args {
	syscallarg(int) signum;
	syscallarg(netbsd32_sigvecp_t) nsv;
	syscallarg(netbsd32_sigvecp_t) osv;
};

struct sunos32_sys_sigsuspend_args {
	syscallarg(int) mask;
};

struct sunos32_sys_sigreturn_args {
	syscallarg(netbsd32_sigcontextp_t) sigcntxp;
};

struct sunos32_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};

struct sunos32_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};

struct sunos32_sys_nfssvc_args {
	syscallarg(int) fd;
};

struct sunos32_sys_statfs_args {
	syscallarg(const netbsd32_charp) path;
	syscallarg(sunos32_statfsp_t) buf;
};

struct sunos32_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(sunos32_statfsp_t) buf;
};

struct sunos32_sys_unmount_args {
	syscallarg(netbsd32_charp) path;
};

struct sunos32_sys_quotactl_args {
	syscallarg(int) cmd;
	syscallarg(netbsd32_charp) special;
	syscallarg(int) uid;
	syscallarg(netbsd32_caddr_t) addr;
};

struct sunos32_sys_exportfs_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(netbsd32_charp) ex;
};

struct sunos32_sys_mount_args {
	syscallarg(netbsd32_charp) type;
	syscallarg(netbsd32_charp) path;
	syscallarg(int) flags;
	syscallarg(netbsd32_caddr_t) data;
};

struct sunos32_sys_ustat_args {
	syscallarg(int) dev;
	syscallarg(sunos32_ustatp_t) buf;
};

struct sunos32_sys_auditsys_args {
	syscallarg(netbsd32_charp) record;
};

struct sunos32_sys_getdents_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_charp) buf;
	syscallarg(int) nbytes;
};

struct sunos32_sys_sigpending_args {
	syscallarg(netbsd32_intp) mask;
};

struct sunos32_sys_sysconf_args {
	syscallarg(int) name;
};

struct sunos32_sys_uname_args {
	syscallarg(sunos32_utsnamep_t) name;
};

/*
 * System call prototypes.
 */

int	sys_nosys(struct proc *, void *, register_t *);
int	netbsd32_exit(struct proc *, void *, register_t *);
int	sys_fork(struct proc *, void *, register_t *);
int	netbsd32_read(struct proc *, void *, register_t *);
int	netbsd32_write(struct proc *, void *, register_t *);
int	sunos32_sys_open(struct proc *, void *, register_t *);
int	netbsd32_close(struct proc *, void *, register_t *);
int	sunos32_sys_wait4(struct proc *, void *, register_t *);
int	sunos32_sys_creat(struct proc *, void *, register_t *);
int	netbsd32_link(struct proc *, void *, register_t *);
int	netbsd32_unlink(struct proc *, void *, register_t *);
int	sunos32_sys_execv(struct proc *, void *, register_t *);
int	netbsd32_chdir(struct proc *, void *, register_t *);
int	sunos32_sys_mknod(struct proc *, void *, register_t *);
int	netbsd32_chmod(struct proc *, void *, register_t *);
int	netbsd32_chown(struct proc *, void *, register_t *);
int	netbsd32_break(struct proc *, void *, register_t *);
int	compat_43_netbsd32_olseek(struct proc *, void *, register_t *);
int	sys_getpid_with_ppid(struct proc *, void *, register_t *);
int	netbsd32_setuid(struct proc *, void *, register_t *);
int	sys_getuid_with_euid(struct proc *, void *, register_t *);
int	sunos32_sys_stime(struct proc *, void *, register_t *);
int	sunos32_sys_ptrace(struct proc *, void *, register_t *);
int	sunos32_sys_access(struct proc *, void *, register_t *);
int	sys_sync(struct proc *, void *, register_t *);
int	netbsd32_kill(struct proc *, void *, register_t *);
int	sunos32_sys_stat(struct proc *, void *, register_t *);
int	sunos32_sys_lstat(struct proc *, void *, register_t *);
int	netbsd32_dup(struct proc *, void *, register_t *);
int	sys_pipe(struct proc *, void *, register_t *);
int	netbsd32_profil(struct proc *, void *, register_t *);
int	netbsd32_setgid(struct proc *, void *, register_t *);
int	sys_getgid_with_egid(struct proc *, void *, register_t *);
int	netbsd32_acct(struct proc *, void *, register_t *);
int	sunos32_sys_mctl(struct proc *, void *, register_t *);
int	sunos32_sys_ioctl(struct proc *, void *, register_t *);
int	sunos32_sys_reboot(struct proc *, void *, register_t *);
int	netbsd32_symlink(struct proc *, void *, register_t *);
int	netbsd32_readlink(struct proc *, void *, register_t *);
int	sunos32_sys_execve(struct proc *, void *, register_t *);
int	netbsd32_umask(struct proc *, void *, register_t *);
int	netbsd32_chroot(struct proc *, void *, register_t *);
int	compat_43_netbsd32_fstat43(struct proc *, void *, register_t *);
int	compat_43_sys_getpagesize(struct proc *, void *, register_t *);
int	sunos32_sys_omsync(struct proc *, void *, register_t *);
int	sys_vfork(struct proc *, void *, register_t *);
int	netbsd32_sbrk(struct proc *, void *, register_t *);
int	netbsd32_sstk(struct proc *, void *, register_t *);
int	sunos32_sys_mmap(struct proc *, void *, register_t *);
int	netbsd32_ovadvise(struct proc *, void *, register_t *);
int	netbsd32_munmap(struct proc *, void *, register_t *);
int	netbsd32_mprotect(struct proc *, void *, register_t *);
int	netbsd32_madvise(struct proc *, void *, register_t *);
int	sunos32_sys_vhangup(struct proc *, void *, register_t *);
int	netbsd32_mincore(struct proc *, void *, register_t *);
int	netbsd32_getgroups(struct proc *, void *, register_t *);
int	netbsd32_setgroups(struct proc *, void *, register_t *);
int	sys_getpgrp(struct proc *, void *, register_t *);
int	sunos32_sys_setpgrp(struct proc *, void *, register_t *);
int	netbsd32_setitimer(struct proc *, void *, register_t *);
int	compat_12_netbsd32_oswapon(struct proc *, void *, register_t *);
int	netbsd32_getitimer(struct proc *, void *, register_t *);
int	compat_43_netbsd32_ogethostname(struct proc *, void *, register_t *);
int	compat_43_netbsd32_osethostname(struct proc *, void *, register_t *);
int	compat_43_sys_getdtablesize(struct proc *, void *, register_t *);
int	netbsd32_dup2(struct proc *, void *, register_t *);
int	sunos32_sys_fcntl(struct proc *, void *, register_t *);
int	netbsd32_select(struct proc *, void *, register_t *);
int	netbsd32_fsync(struct proc *, void *, register_t *);
int	netbsd32_setpriority(struct proc *, void *, register_t *);
int	sunos32_sys_socket(struct proc *, void *, register_t *);
int	netbsd32_connect(struct proc *, void *, register_t *);
int	compat_43_netbsd32_oaccept(struct proc *, void *, register_t *);
int	netbsd32_getpriority(struct proc *, void *, register_t *);
int	compat_43_netbsd32_osend(struct proc *, void *, register_t *);
int	compat_43_netbsd32_orecv(struct proc *, void *, register_t *);
int	netbsd32_bind(struct proc *, void *, register_t *);
int	sunos32_sys_setsockopt(struct proc *, void *, register_t *);
int	netbsd32_listen(struct proc *, void *, register_t *);
int	sunos32_sys_sigvec(struct proc *, void *, register_t *);
int	compat_43_netbsd32_sigblock(struct proc *, void *, register_t *);
int	compat_43_netbsd32_sigsetmask(struct proc *, void *, register_t *);
int	sunos32_sys_sigsuspend(struct proc *, void *, register_t *);
int	compat_43_netbsd32_osigstack(struct proc *, void *, register_t *);
int	compat_43_netbsd32_orecvmsg(struct proc *, void *, register_t *);
int	compat_43_netbsd32_osendmsg(struct proc *, void *, register_t *);
int	netbsd32_gettimeofday(struct proc *, void *, register_t *);
int	netbsd32_getrusage(struct proc *, void *, register_t *);
int	netbsd32_getsockopt(struct proc *, void *, register_t *);
int	netbsd32_readv(struct proc *, void *, register_t *);
int	netbsd32_writev(struct proc *, void *, register_t *);
int	netbsd32_settimeofday(struct proc *, void *, register_t *);
int	netbsd32_fchown(struct proc *, void *, register_t *);
int	netbsd32_fchmod(struct proc *, void *, register_t *);
int	compat_43_netbsd32_orecvfrom(struct proc *, void *, register_t *);
int	netbsd32_setreuid(struct proc *, void *, register_t *);
int	netbsd32_setregid(struct proc *, void *, register_t *);
int	netbsd32_rename(struct proc *, void *, register_t *);
int	compat_43_netbsd32_otruncate(struct proc *, void *, register_t *);
int	compat_43_netbsd32_oftruncate(struct proc *, void *, register_t *);
int	netbsd32_flock(struct proc *, void *, register_t *);
int	netbsd32_sendto(struct proc *, void *, register_t *);
int	netbsd32_shutdown(struct proc *, void *, register_t *);
int	sunos32_sys_socketpair(struct proc *, void *, register_t *);
int	netbsd32_mkdir(struct proc *, void *, register_t *);
int	netbsd32_rmdir(struct proc *, void *, register_t *);
int	netbsd32_utimes(struct proc *, void *, register_t *);
int	sunos32_sys_sigreturn(struct proc *, void *, register_t *);
int	netbsd32_adjtime(struct proc *, void *, register_t *);
int	compat_43_netbsd32_ogetpeername(struct proc *, void *, register_t *);
int	compat_43_sys_gethostid(struct proc *, void *, register_t *);
int	sunos32_sys_getrlimit(struct proc *, void *, register_t *);
int	sunos32_sys_setrlimit(struct proc *, void *, register_t *);
int	compat_43_netbsd32_killpg(struct proc *, void *, register_t *);
int	compat_43_netbsd32_ogetsockname(struct proc *, void *, register_t *);
int	netbsd32_poll(struct proc *, void *, register_t *);
#ifdef NFSSERVER
int	sunos32_sys_nfssvc(struct proc *, void *, register_t *);
#else
#endif
int	compat_43_netbsd32_ogetdirentries(struct proc *, void *, register_t *);
int	sunos32_sys_statfs(struct proc *, void *, register_t *);
int	sunos32_sys_fstatfs(struct proc *, void *, register_t *);
int	sunos32_sys_unmount(struct proc *, void *, register_t *);
#ifdef NFS
int	async_daemon(struct proc *, void *, register_t *);
int	sys_getfh(struct proc *, void *, register_t *);
#else
#endif
int	compat_09_netbsd32_ogetdomainname(struct proc *, void *, register_t *);
int	compat_09_netbsd32_osetdomainname(struct proc *, void *, register_t *);
int	sunos32_sys_quotactl(struct proc *, void *, register_t *);
int	sunos32_sys_exportfs(struct proc *, void *, register_t *);
int	sunos32_sys_mount(struct proc *, void *, register_t *);
int	sunos32_sys_ustat(struct proc *, void *, register_t *);
#ifdef SYSVSEM
int	netbsd32_compat_10_sys_semsys(struct proc *, void *, register_t *);
#else
#endif
#ifdef SYSVMSG
int	netbsd32_compat_10_sys_msgsys(struct proc *, void *, register_t *);
#else
#endif
#ifdef SYSVSHM
int	netbsd32_compat_10_sys_shmsys(struct proc *, void *, register_t *);
#else
#endif
int	sunos32_sys_auditsys(struct proc *, void *, register_t *);
int	sunos32_sys_getdents(struct proc *, void *, register_t *);
int	sys_setsid(struct proc *, void *, register_t *);
int	netbsd32_fchdir(struct proc *, void *, register_t *);
int	netbsd32_fchroot(struct proc *, void *, register_t *);
int	sunos32_sys_sigpending(struct proc *, void *, register_t *);
int	netbsd32_setpgid(struct proc *, void *, register_t *);
int	netbsd32_pathconf(struct proc *, void *, register_t *);
int	netbsd32_fpathconf(struct proc *, void *, register_t *);
int	sunos32_sys_sysconf(struct proc *, void *, register_t *);
int	sunos32_sys_uname(struct proc *, void *, register_t *);
#endif /* _SUNOS32_SYS__SYSCALLARGS_H_ */
