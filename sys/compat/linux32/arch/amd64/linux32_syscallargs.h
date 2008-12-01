/* $NetBSD: linux32_syscallargs.h,v 1.44 2008/12/01 14:19:45 njoly Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.41 2008/12/01 14:18:44 njoly Exp
 */

#ifndef _LINUX32_SYS_SYSCALLARGS_H_
#define	_LINUX32_SYS_SYSCALLARGS_H_

#define	LINUX32_SYS_MAXSYSARGS	8

#undef	syscallarg
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

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= LINUX32_SYS_MAXSYSARGS * sizeof (register32_t) ? 1 : -1];

struct netbsd32_exit_args;

struct netbsd32_read_args;

struct netbsd32_write_args;

struct linux32_sys_open_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};
check_syscall_args(linux32_sys_open)

struct netbsd32_close_args;

struct linux32_sys_waitpid_args {
	syscallarg(int) pid;
	syscallarg(netbsd32_intp) status;
	syscallarg(int) options;
};
check_syscall_args(linux32_sys_waitpid)

struct linux32_sys_creat_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(int) mode;
};
check_syscall_args(linux32_sys_creat)

struct netbsd32_link_args;

struct linux32_sys_unlink_args {
	syscallarg(netbsd32_charp) path;
};
check_syscall_args(linux32_sys_unlink)

struct netbsd32_execve_args;

struct netbsd32_chdir_args;

struct linux32_sys_time_args {
	syscallarg(linux32_timep_t) t;
};
check_syscall_args(linux32_sys_time)

struct linux32_sys_mknod_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};
check_syscall_args(linux32_sys_mknod)

struct netbsd32_chmod_args;

struct linux32_sys_lchown16_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_uid16_t) uid;
	syscallarg(linux32_gid16_t) gid;
};
check_syscall_args(linux32_sys_lchown16)

struct linux32_sys_break_args {
	syscallarg(netbsd32_charp) nsize;
};
check_syscall_args(linux32_sys_break)

struct compat_43_netbsd32_olseek_args;

struct netbsd32_setuid_args;

struct linux32_sys_stime_args {
	syscallarg(linux32_timep_t) t;
};
check_syscall_args(linux32_sys_stime)

struct linux32_sys_ptrace_args {
	syscallarg(int) request;
	syscallarg(int) pid;
	syscallarg(int) addr;
	syscallarg(int) data;
};
check_syscall_args(linux32_sys_ptrace)

struct linux32_sys_alarm_args {
	syscallarg(unsigned int) secs;
};
check_syscall_args(linux32_sys_alarm)

struct linux32_sys_utime_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_utimbufp_t) times;
};
check_syscall_args(linux32_sys_utime)

struct netbsd32_access_args;

struct linux32_sys_nice_args {
	syscallarg(int) incr;
};
check_syscall_args(linux32_sys_nice)

struct linux32_sys_kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};
check_syscall_args(linux32_sys_kill)

struct netbsd32___posix_rename_args;

struct netbsd32_mkdir_args;

struct netbsd32_rmdir_args;

struct netbsd32_dup_args;

struct linux32_sys_pipe_args {
	syscallarg(netbsd32_intp) fd;
};
check_syscall_args(linux32_sys_pipe)

struct linux32_sys_times_args {
	syscallarg(linux32_tmsp_t) tms;
};
check_syscall_args(linux32_sys_times)

struct linux32_sys_brk_args {
	syscallarg(netbsd32_charp) nsize;
};
check_syscall_args(linux32_sys_brk)

struct netbsd32_setgid_args;

struct linux32_sys_signal_args {
	syscallarg(int) signum;
	syscallarg(linux32_handler_t) handler;
};
check_syscall_args(linux32_sys_signal)

struct netbsd32_acct_args;

struct linux32_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_u_long) com;
	syscallarg(netbsd32_charp) data;
};
check_syscall_args(linux32_sys_ioctl)

struct linux32_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};
check_syscall_args(linux32_sys_fcntl)

struct netbsd32_setpgid_args;

struct linux32_sys_oldolduname_args {
	syscallarg(linux32_oldold_utsnamep_t) up;
};
check_syscall_args(linux32_sys_oldolduname)

struct netbsd32_umask_args;

struct netbsd32_chroot_args;

struct netbsd32_dup2_args;

struct linux32_sys_setreuid16_args {
	syscallarg(linux32_uid16_t) ruid;
	syscallarg(linux32_uid16_t) euid;
};
check_syscall_args(linux32_sys_setreuid16)

struct linux32_sys_setregid16_args {
	syscallarg(linux32_gid16_t) rgid;
	syscallarg(linux32_gid16_t) egid;
};
check_syscall_args(linux32_sys_setregid16)

struct compat_43_netbsd32_osethostname_args;

struct linux32_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_setrlimit)

struct linux32_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_getrlimit)

struct netbsd32_getrusage_args;

struct linux32_sys_gettimeofday_args {
	syscallarg(netbsd32_timevalp_t) tp;
	syscallarg(netbsd32_timezonep_t) tzp;
};
check_syscall_args(linux32_sys_gettimeofday)

struct linux32_sys_settimeofday_args {
	syscallarg(netbsd32_timevalp_t) tp;
	syscallarg(netbsd32_timezonep_t) tzp;
};
check_syscall_args(linux32_sys_settimeofday)

struct linux32_sys_getgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux32_gid16p_t) gidset;
};
check_syscall_args(linux32_sys_getgroups16)

struct linux32_sys_setgroups16_args {
	syscallarg(int) gidsetsize;
	syscallarg(linux32_gid16p_t) gidset;
};
check_syscall_args(linux32_sys_setgroups16)

struct linux32_sys_oldselect_args {
	syscallarg(linux32_oldselectp_t) lsp;
};
check_syscall_args(linux32_sys_oldselect)

struct netbsd32_symlink_args;

struct compat_43_netbsd32_lstat43_args;

struct netbsd32_readlink_args;

struct linux32_sys_swapon_args {
	syscallarg(netbsd32_charp) name;
};
check_syscall_args(linux32_sys_swapon)

struct linux32_sys_reboot_args {
	syscallarg(int) magic1;
	syscallarg(int) magic2;
	syscallarg(int) cmd;
	syscallarg(netbsd32_voidp) arg;
};
check_syscall_args(linux32_sys_reboot)

struct linux32_sys_readdir_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_voidp) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux32_sys_readdir)

struct linux32_sys_old_mmap_args {
	syscallarg(linux32_oldmmapp) lmp;
};
check_syscall_args(linux32_sys_old_mmap)

struct netbsd32_munmap_args;

struct compat_43_netbsd32_otruncate_args;

struct compat_43_netbsd32_oftruncate_args;

struct netbsd32_fchmod_args;

struct linux32_sys_fchown16_args {
	syscallarg(int) fd;
	syscallarg(linux32_uid16_t) uid;
	syscallarg(linux32_gid16_t) gid;
};
check_syscall_args(linux32_sys_fchown16)

struct linux32_sys_getpriority_args {
	syscallarg(int) which;
	syscallarg(int) who;
};
check_syscall_args(linux32_sys_getpriority)

struct netbsd32_setpriority_args;

struct linux32_sys_statfs_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_statfsp) sp;
};
check_syscall_args(linux32_sys_statfs)

struct linux32_sys_socketcall_args {
	syscallarg(int) what;
	syscallarg(netbsd32_voidp) args;
};
check_syscall_args(linux32_sys_socketcall)

struct netbsd32_setitimer_args;

struct netbsd32_getitimer_args;

struct linux32_sys_olduname_args {
	syscallarg(linux32_oldutsnamep_t) up;
};
check_syscall_args(linux32_sys_olduname)

struct linux32_sys_wait4_args {
	syscallarg(int) pid;
	syscallarg(netbsd32_intp) status;
	syscallarg(int) options;
	syscallarg(netbsd32_rusagep_t) rusage;
};
check_syscall_args(linux32_sys_wait4)

struct linux32_sys_swapoff_args {
	syscallarg(netbsd32_charp) path;
};
check_syscall_args(linux32_sys_swapoff)

struct linux32_sys_sysinfo_args {
	syscallarg(linux32_sysinfop_t) arg;
};
check_syscall_args(linux32_sys_sysinfo)

struct linux32_sys_ipc_args {
	syscallarg(int) what;
	syscallarg(int) a1;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(netbsd32_voidp) ptr;
};
check_syscall_args(linux32_sys_ipc)

struct netbsd32_fsync_args;

struct linux32_sys_sigreturn_args {
	syscallarg(linux32_sigcontextp_t) scp;
};
check_syscall_args(linux32_sys_sigreturn)

struct linux32_sys_clone_args {
	syscallarg(int) flags;
	syscallarg(netbsd32_voidp) stack;
};
check_syscall_args(linux32_sys_clone)

struct linux32_sys_uname_args {
	syscallarg(linux32_utsnamep) up;
};
check_syscall_args(linux32_sys_uname)

struct linux32_sys_mprotect_args {
	syscallarg(netbsd32_voidp) start;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) prot;
};
check_syscall_args(linux32_sys_mprotect)

struct netbsd32_getpgid_args;

struct netbsd32_fchdir_args;

struct linux32_sys_setfsuid_args;

struct linux32_sys_setfsgid_args;

struct linux32_sys_llseek_args {
	syscallarg(int) fd;
	syscallarg(u_int32_t) ohigh;
	syscallarg(u_int32_t) olow;
	syscallarg(netbsd32_caddr_t) res;
	syscallarg(int) whence;
};
check_syscall_args(linux32_sys_llseek)

struct linux32_sys_getdents_args {
	syscallarg(int) fd;
	syscallarg(linux32_direntp_t) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux32_sys_getdents)

struct linux32_sys_select_args {
	syscallarg(int) nfds;
	syscallarg(netbsd32_fd_setp_t) readfds;
	syscallarg(netbsd32_fd_setp_t) writefds;
	syscallarg(netbsd32_fd_setp_t) exceptfds;
	syscallarg(netbsd32_timevalp_t) timeout;
};
check_syscall_args(linux32_sys_select)

struct netbsd32_flock_args;

struct netbsd32___msync13_args;

struct netbsd32_readv_args;

struct netbsd32_writev_args;

struct netbsd32_getsid_args;

struct linux32_sys_fdatasync_args {
	syscallarg(int) fd;
};
check_syscall_args(linux32_sys_fdatasync)

struct linux32_sys___sysctl_args {
	syscallarg(linux32___sysctlp_t) lsp;
};
check_syscall_args(linux32_sys___sysctl)

struct netbsd32_mlock_args;

struct netbsd32_munlock_args;

struct netbsd32_mlockall_args;

struct linux32_sys_sched_getparam_args {
	syscallarg(pid_t) pid;
	syscallarg(linux32_sched_paramp_t) sp;
};
check_syscall_args(linux32_sys_sched_getparam)

struct linux32_sys_sched_setscheduler_args {
	syscallarg(pid_t) pid;
	syscallarg(int) policy;
	syscallarg(linux32_sched_paramp_t) sp;
};
check_syscall_args(linux32_sys_sched_setscheduler)

struct linux32_sys_sched_getscheduler_args {
	syscallarg(pid_t) pid;
};
check_syscall_args(linux32_sys_sched_getscheduler)

struct linux32_sys_sched_get_priority_max_args {
	syscallarg(int) policy;
};
check_syscall_args(linux32_sys_sched_get_priority_max)

struct linux32_sys_sched_get_priority_min_args {
	syscallarg(int) policy;
};
check_syscall_args(linux32_sys_sched_get_priority_min)

struct linux32_sys_nanosleep_args {
	syscallarg(linux32_timespecp_t) rqtp;
	syscallarg(linux32_timespecp_t) rmtp;
};
check_syscall_args(linux32_sys_nanosleep)

struct linux32_sys_mremap_args {
	syscallarg(netbsd32_voidp) old_address;
	syscallarg(netbsd32_size_t) old_size;
	syscallarg(netbsd32_size_t) new_size;
	syscallarg(netbsd32_u_long) flags;
};
check_syscall_args(linux32_sys_mremap)

struct linux32_sys_setresuid16_args {
	syscallarg(linux32_uid16_t) ruid;
	syscallarg(linux32_uid16_t) euid;
	syscallarg(linux32_uid16_t) suid;
};
check_syscall_args(linux32_sys_setresuid16)

struct linux32_sys_getresuid16_args {
	syscallarg(linux32_uid16p_t) ruid;
	syscallarg(linux32_uid16p_t) euid;
	syscallarg(linux32_uid16p_t) suid;
};
check_syscall_args(linux32_sys_getresuid16)

struct netbsd32_poll_args;

struct linux32_sys_setresgid16_args {
	syscallarg(linux32_gid16_t) rgid;
	syscallarg(linux32_gid16_t) egid;
	syscallarg(linux32_gid16_t) sgid;
};
check_syscall_args(linux32_sys_setresgid16)

struct linux32_sys_getresgid16_args {
	syscallarg(linux32_gid16p_t) rgid;
	syscallarg(linux32_gid16p_t) egid;
	syscallarg(linux32_gid16p_t) sgid;
};
check_syscall_args(linux32_sys_getresgid16)

struct linux32_sys_rt_sigreturn_args {
	syscallarg(linux32_ucontextp_t) ucp;
};
check_syscall_args(linux32_sys_rt_sigreturn)

struct linux32_sys_rt_sigaction_args {
	syscallarg(int) signum;
	syscallarg(linux32_sigactionp_t) nsa;
	syscallarg(linux32_sigactionp_t) osa;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigaction)

struct linux32_sys_rt_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(linux32_sigsetp_t) set;
	syscallarg(linux32_sigsetp_t) oset;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigprocmask)

struct linux32_sys_rt_sigpending_args {
	syscallarg(linux32_sigsetp_t) set;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigpending)

struct linux32_sys_rt_sigsuspend_args {
	syscallarg(linux32_sigsetp_t) unewset;
	syscallarg(netbsd32_size_t) sigsetsize;
};
check_syscall_args(linux32_sys_rt_sigsuspend)

struct linux32_sys_pread_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_voidp) buf;
	syscallarg(netbsd32_size_t) nbyte;
	syscallarg(linux32_off_t) offset;
};
check_syscall_args(linux32_sys_pread)

struct linux32_sys_pwrite_args {
	syscallarg(int) fd;
	syscallarg(netbsd32_voidp) buf;
	syscallarg(netbsd32_size_t) nbyte;
	syscallarg(linux32_off_t) offset;
};
check_syscall_args(linux32_sys_pwrite)

struct linux32_sys_chown16_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_uid16_t) uid;
	syscallarg(linux32_gid16_t) gid;
};
check_syscall_args(linux32_sys_chown16)

struct netbsd32___getcwd_args;

struct linux32_sys_ugetrlimit_args {
	syscallarg(int) which;
	syscallarg(netbsd32_orlimitp_t) rlp;
};
check_syscall_args(linux32_sys_ugetrlimit)

struct linux32_sys_mmap2_args {
	syscallarg(netbsd32_u_long) addr;
	syscallarg(netbsd32_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(linux32_off_t) offset;
};
check_syscall_args(linux32_sys_mmap2)

struct linux32_sys_stat64_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_stat64)

struct linux32_sys_lstat64_args {
	syscallarg(netbsd32_charp) path;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_lstat64)

struct linux32_sys_fstat64_args {
	syscallarg(int) fd;
	syscallarg(linux32_stat64p) sp;
};
check_syscall_args(linux32_sys_fstat64)

struct netbsd32___posix_lchown_args;

struct netbsd32_setreuid_args;

struct netbsd32_setregid_args;

struct netbsd32_getgroups_args;

struct netbsd32_setgroups_args;

struct netbsd32___posix_fchown_args;

struct linux32_sys_setresuid_args {
	syscallarg(uid_t) ruid;
	syscallarg(uid_t) euid;
	syscallarg(uid_t) suid;
};
check_syscall_args(linux32_sys_setresuid)

struct linux32_sys_setresgid_args {
	syscallarg(gid_t) rgid;
	syscallarg(gid_t) egid;
	syscallarg(gid_t) sgid;
};
check_syscall_args(linux32_sys_setresgid)

struct netbsd32___posix_chown_args;

struct netbsd32_setuid_args;

struct netbsd32_setgid_args;

struct linux32_sys_setfsuid_args {
	syscallarg(uid_t) uid;
};
check_syscall_args(linux32_sys_setfsuid)

struct linux32_sys_setfsgid_args {
	syscallarg(gid_t) gid;
};
check_syscall_args(linux32_sys_setfsgid)

struct netbsd32_madvise_args;

struct linux32_sys_getdents64_args {
	syscallarg(int) fd;
	syscallarg(linux32_dirent64p_t) dent;
	syscallarg(unsigned int) count;
};
check_syscall_args(linux32_sys_getdents64)
#define linux32_sys_fcntl64 linux32_sys_fcntl
#define linux32_sys_fcntl64_args linux32_sys_fcntl_args

struct linux32_sys_fcntl64_args;

struct linux32_sys_exit_group_args {
	syscallarg(int) error_code;
};
check_syscall_args(linux32_sys_exit_group)

struct linux32_sys_clock_settime_args {
	syscallarg(clockid_t) which;
	syscallarg(linux32_timespecp_t) tp;
};
check_syscall_args(linux32_sys_clock_settime)

struct linux32_sys_clock_gettime_args {
	syscallarg(clockid_t) which;
	syscallarg(linux32_timespecp_t) tp;
};
check_syscall_args(linux32_sys_clock_gettime)

struct linux32_sys_clock_getres_args {
	syscallarg(clockid_t) which;
	syscallarg(linux32_timespecp_t) tp;
};
check_syscall_args(linux32_sys_clock_getres)

/*
 * System call prototypes.
 */

int	linux_sys_nosys(struct lwp *, const void *, register_t *);

int	netbsd32_exit(struct lwp *, const struct netbsd32_exit_args *, register_t *);

int	sys_fork(struct lwp *, const void *, register_t *);

int	netbsd32_read(struct lwp *, const struct netbsd32_read_args *, register_t *);

int	netbsd32_write(struct lwp *, const struct netbsd32_write_args *, register_t *);

int	linux32_sys_open(struct lwp *, const struct linux32_sys_open_args *, register_t *);

int	netbsd32_close(struct lwp *, const struct netbsd32_close_args *, register_t *);

int	linux32_sys_waitpid(struct lwp *, const struct linux32_sys_waitpid_args *, register_t *);

int	linux32_sys_creat(struct lwp *, const struct linux32_sys_creat_args *, register_t *);

int	netbsd32_link(struct lwp *, const struct netbsd32_link_args *, register_t *);

int	linux32_sys_unlink(struct lwp *, const struct linux32_sys_unlink_args *, register_t *);

int	netbsd32_execve(struct lwp *, const struct netbsd32_execve_args *, register_t *);

int	netbsd32_chdir(struct lwp *, const struct netbsd32_chdir_args *, register_t *);

int	linux32_sys_time(struct lwp *, const struct linux32_sys_time_args *, register_t *);

int	linux32_sys_mknod(struct lwp *, const struct linux32_sys_mknod_args *, register_t *);

int	netbsd32_chmod(struct lwp *, const struct netbsd32_chmod_args *, register_t *);

int	linux32_sys_lchown16(struct lwp *, const struct linux32_sys_lchown16_args *, register_t *);

int	linux32_sys_break(struct lwp *, const struct linux32_sys_break_args *, register_t *);

int	compat_43_netbsd32_olseek(struct lwp *, const struct compat_43_netbsd32_olseek_args *, register_t *);

int	linux_sys_getpid(struct lwp *, const void *, register_t *);

int	netbsd32_setuid(struct lwp *, const struct netbsd32_setuid_args *, register_t *);

int	sys_getuid(struct lwp *, const void *, register_t *);

int	linux32_sys_stime(struct lwp *, const struct linux32_sys_stime_args *, register_t *);

int	linux32_sys_ptrace(struct lwp *, const struct linux32_sys_ptrace_args *, register_t *);

int	linux32_sys_alarm(struct lwp *, const struct linux32_sys_alarm_args *, register_t *);

int	linux_sys_pause(struct lwp *, const void *, register_t *);

int	linux32_sys_utime(struct lwp *, const struct linux32_sys_utime_args *, register_t *);

int	netbsd32_access(struct lwp *, const struct netbsd32_access_args *, register_t *);

int	linux32_sys_nice(struct lwp *, const struct linux32_sys_nice_args *, register_t *);

int	sys_sync(struct lwp *, const void *, register_t *);

int	linux32_sys_kill(struct lwp *, const struct linux32_sys_kill_args *, register_t *);

int	netbsd32___posix_rename(struct lwp *, const struct netbsd32___posix_rename_args *, register_t *);

int	netbsd32_mkdir(struct lwp *, const struct netbsd32_mkdir_args *, register_t *);

int	netbsd32_rmdir(struct lwp *, const struct netbsd32_rmdir_args *, register_t *);

int	netbsd32_dup(struct lwp *, const struct netbsd32_dup_args *, register_t *);

int	linux32_sys_pipe(struct lwp *, const struct linux32_sys_pipe_args *, register_t *);

int	linux32_sys_times(struct lwp *, const struct linux32_sys_times_args *, register_t *);

int	linux32_sys_brk(struct lwp *, const struct linux32_sys_brk_args *, register_t *);

int	netbsd32_setgid(struct lwp *, const struct netbsd32_setgid_args *, register_t *);

int	sys_getgid(struct lwp *, const void *, register_t *);

int	linux32_sys_signal(struct lwp *, const struct linux32_sys_signal_args *, register_t *);

int	sys_geteuid(struct lwp *, const void *, register_t *);

int	sys_getegid(struct lwp *, const void *, register_t *);

int	netbsd32_acct(struct lwp *, const struct netbsd32_acct_args *, register_t *);

int	linux32_sys_ioctl(struct lwp *, const struct linux32_sys_ioctl_args *, register_t *);

int	linux32_sys_fcntl(struct lwp *, const struct linux32_sys_fcntl_args *, register_t *);

int	netbsd32_setpgid(struct lwp *, const struct netbsd32_setpgid_args *, register_t *);

int	linux32_sys_oldolduname(struct lwp *, const struct linux32_sys_oldolduname_args *, register_t *);

int	netbsd32_umask(struct lwp *, const struct netbsd32_umask_args *, register_t *);

int	netbsd32_chroot(struct lwp *, const struct netbsd32_chroot_args *, register_t *);

int	netbsd32_dup2(struct lwp *, const struct netbsd32_dup2_args *, register_t *);

int	linux_sys_getppid(struct lwp *, const void *, register_t *);

int	sys_getpgrp(struct lwp *, const void *, register_t *);

int	sys_setsid(struct lwp *, const void *, register_t *);

int	linux32_sys_setreuid16(struct lwp *, const struct linux32_sys_setreuid16_args *, register_t *);

int	linux32_sys_setregid16(struct lwp *, const struct linux32_sys_setregid16_args *, register_t *);

int	compat_43_netbsd32_osethostname(struct lwp *, const struct compat_43_netbsd32_osethostname_args *, register_t *);

int	linux32_sys_setrlimit(struct lwp *, const struct linux32_sys_setrlimit_args *, register_t *);

int	linux32_sys_getrlimit(struct lwp *, const struct linux32_sys_getrlimit_args *, register_t *);

int	netbsd32_getrusage(struct lwp *, const struct netbsd32_getrusage_args *, register_t *);

int	linux32_sys_gettimeofday(struct lwp *, const struct linux32_sys_gettimeofday_args *, register_t *);

int	linux32_sys_settimeofday(struct lwp *, const struct linux32_sys_settimeofday_args *, register_t *);

int	linux32_sys_getgroups16(struct lwp *, const struct linux32_sys_getgroups16_args *, register_t *);

int	linux32_sys_setgroups16(struct lwp *, const struct linux32_sys_setgroups16_args *, register_t *);

int	linux32_sys_oldselect(struct lwp *, const struct linux32_sys_oldselect_args *, register_t *);

int	netbsd32_symlink(struct lwp *, const struct netbsd32_symlink_args *, register_t *);

int	compat_43_netbsd32_lstat43(struct lwp *, const struct compat_43_netbsd32_lstat43_args *, register_t *);

int	netbsd32_readlink(struct lwp *, const struct netbsd32_readlink_args *, register_t *);

int	linux32_sys_swapon(struct lwp *, const struct linux32_sys_swapon_args *, register_t *);

int	linux32_sys_reboot(struct lwp *, const struct linux32_sys_reboot_args *, register_t *);

int	linux32_sys_readdir(struct lwp *, const struct linux32_sys_readdir_args *, register_t *);

int	linux32_sys_old_mmap(struct lwp *, const struct linux32_sys_old_mmap_args *, register_t *);

int	netbsd32_munmap(struct lwp *, const struct netbsd32_munmap_args *, register_t *);

int	compat_43_netbsd32_otruncate(struct lwp *, const struct compat_43_netbsd32_otruncate_args *, register_t *);

int	compat_43_netbsd32_oftruncate(struct lwp *, const struct compat_43_netbsd32_oftruncate_args *, register_t *);

int	netbsd32_fchmod(struct lwp *, const struct netbsd32_fchmod_args *, register_t *);

int	linux32_sys_fchown16(struct lwp *, const struct linux32_sys_fchown16_args *, register_t *);

int	linux32_sys_getpriority(struct lwp *, const struct linux32_sys_getpriority_args *, register_t *);

int	netbsd32_setpriority(struct lwp *, const struct netbsd32_setpriority_args *, register_t *);

int	linux32_sys_statfs(struct lwp *, const struct linux32_sys_statfs_args *, register_t *);

int	linux32_sys_socketcall(struct lwp *, const struct linux32_sys_socketcall_args *, register_t *);

int	netbsd32_setitimer(struct lwp *, const struct netbsd32_setitimer_args *, register_t *);

int	netbsd32_getitimer(struct lwp *, const struct netbsd32_getitimer_args *, register_t *);

int	linux32_sys_olduname(struct lwp *, const struct linux32_sys_olduname_args *, register_t *);

int	linux32_sys_wait4(struct lwp *, const struct linux32_sys_wait4_args *, register_t *);

int	linux32_sys_swapoff(struct lwp *, const struct linux32_sys_swapoff_args *, register_t *);

int	linux32_sys_sysinfo(struct lwp *, const struct linux32_sys_sysinfo_args *, register_t *);

int	linux32_sys_ipc(struct lwp *, const struct linux32_sys_ipc_args *, register_t *);

int	netbsd32_fsync(struct lwp *, const struct netbsd32_fsync_args *, register_t *);

int	linux32_sys_sigreturn(struct lwp *, const struct linux32_sys_sigreturn_args *, register_t *);

int	linux32_sys_clone(struct lwp *, const struct linux32_sys_clone_args *, register_t *);

int	linux32_sys_uname(struct lwp *, const struct linux32_sys_uname_args *, register_t *);

int	linux32_sys_mprotect(struct lwp *, const struct linux32_sys_mprotect_args *, register_t *);

int	netbsd32_getpgid(struct lwp *, const struct netbsd32_getpgid_args *, register_t *);

int	netbsd32_fchdir(struct lwp *, const struct netbsd32_fchdir_args *, register_t *);

int	linux32_sys_setfsuid(struct lwp *, const struct linux32_sys_setfsuid_args *, register_t *);

int	linux32_sys_setfsgid(struct lwp *, const struct linux32_sys_setfsgid_args *, register_t *);

int	linux32_sys_llseek(struct lwp *, const struct linux32_sys_llseek_args *, register_t *);

int	linux32_sys_getdents(struct lwp *, const struct linux32_sys_getdents_args *, register_t *);

int	linux32_sys_select(struct lwp *, const struct linux32_sys_select_args *, register_t *);

int	netbsd32_flock(struct lwp *, const struct netbsd32_flock_args *, register_t *);

int	netbsd32___msync13(struct lwp *, const struct netbsd32___msync13_args *, register_t *);

int	netbsd32_readv(struct lwp *, const struct netbsd32_readv_args *, register_t *);

int	netbsd32_writev(struct lwp *, const struct netbsd32_writev_args *, register_t *);

int	netbsd32_getsid(struct lwp *, const struct netbsd32_getsid_args *, register_t *);

int	linux32_sys_fdatasync(struct lwp *, const struct linux32_sys_fdatasync_args *, register_t *);

int	linux32_sys___sysctl(struct lwp *, const struct linux32_sys___sysctl_args *, register_t *);

int	netbsd32_mlock(struct lwp *, const struct netbsd32_mlock_args *, register_t *);

int	netbsd32_munlock(struct lwp *, const struct netbsd32_munlock_args *, register_t *);

int	netbsd32_mlockall(struct lwp *, const struct netbsd32_mlockall_args *, register_t *);

int	sys_munlockall(struct lwp *, const void *, register_t *);

int	linux32_sys_sched_getparam(struct lwp *, const struct linux32_sys_sched_getparam_args *, register_t *);

int	linux32_sys_sched_setscheduler(struct lwp *, const struct linux32_sys_sched_setscheduler_args *, register_t *);

int	linux32_sys_sched_getscheduler(struct lwp *, const struct linux32_sys_sched_getscheduler_args *, register_t *);

int	linux_sys_sched_yield(struct lwp *, const void *, register_t *);

int	linux32_sys_sched_get_priority_max(struct lwp *, const struct linux32_sys_sched_get_priority_max_args *, register_t *);

int	linux32_sys_sched_get_priority_min(struct lwp *, const struct linux32_sys_sched_get_priority_min_args *, register_t *);

int	linux32_sys_nanosleep(struct lwp *, const struct linux32_sys_nanosleep_args *, register_t *);

int	linux32_sys_mremap(struct lwp *, const struct linux32_sys_mremap_args *, register_t *);

int	linux32_sys_setresuid16(struct lwp *, const struct linux32_sys_setresuid16_args *, register_t *);

int	linux32_sys_getresuid16(struct lwp *, const struct linux32_sys_getresuid16_args *, register_t *);

int	netbsd32_poll(struct lwp *, const struct netbsd32_poll_args *, register_t *);

int	linux32_sys_setresgid16(struct lwp *, const struct linux32_sys_setresgid16_args *, register_t *);

int	linux32_sys_getresgid16(struct lwp *, const struct linux32_sys_getresgid16_args *, register_t *);

int	linux32_sys_rt_sigreturn(struct lwp *, const struct linux32_sys_rt_sigreturn_args *, register_t *);

int	linux32_sys_rt_sigaction(struct lwp *, const struct linux32_sys_rt_sigaction_args *, register_t *);

int	linux32_sys_rt_sigprocmask(struct lwp *, const struct linux32_sys_rt_sigprocmask_args *, register_t *);

int	linux32_sys_rt_sigpending(struct lwp *, const struct linux32_sys_rt_sigpending_args *, register_t *);

int	linux32_sys_rt_sigsuspend(struct lwp *, const struct linux32_sys_rt_sigsuspend_args *, register_t *);

int	linux32_sys_pread(struct lwp *, const struct linux32_sys_pread_args *, register_t *);

int	linux32_sys_pwrite(struct lwp *, const struct linux32_sys_pwrite_args *, register_t *);

int	linux32_sys_chown16(struct lwp *, const struct linux32_sys_chown16_args *, register_t *);

int	netbsd32___getcwd(struct lwp *, const struct netbsd32___getcwd_args *, register_t *);

int	sys___vfork14(struct lwp *, const void *, register_t *);

int	linux32_sys_ugetrlimit(struct lwp *, const struct linux32_sys_ugetrlimit_args *, register_t *);

int	linux32_sys_mmap2(struct lwp *, const struct linux32_sys_mmap2_args *, register_t *);

int	linux32_sys_stat64(struct lwp *, const struct linux32_sys_stat64_args *, register_t *);

int	linux32_sys_lstat64(struct lwp *, const struct linux32_sys_lstat64_args *, register_t *);

int	linux32_sys_fstat64(struct lwp *, const struct linux32_sys_fstat64_args *, register_t *);

int	netbsd32___posix_lchown(struct lwp *, const struct netbsd32___posix_lchown_args *, register_t *);

int	netbsd32_setreuid(struct lwp *, const struct netbsd32_setreuid_args *, register_t *);

int	netbsd32_setregid(struct lwp *, const struct netbsd32_setregid_args *, register_t *);

int	netbsd32_getgroups(struct lwp *, const struct netbsd32_getgroups_args *, register_t *);

int	netbsd32_setgroups(struct lwp *, const struct netbsd32_setgroups_args *, register_t *);

int	netbsd32___posix_fchown(struct lwp *, const struct netbsd32___posix_fchown_args *, register_t *);

int	linux32_sys_setresuid(struct lwp *, const struct linux32_sys_setresuid_args *, register_t *);

int	linux32_sys_setresgid(struct lwp *, const struct linux32_sys_setresgid_args *, register_t *);

int	netbsd32___posix_chown(struct lwp *, const struct netbsd32___posix_chown_args *, register_t *);

int	netbsd32_madvise(struct lwp *, const struct netbsd32_madvise_args *, register_t *);

int	linux32_sys_getdents64(struct lwp *, const struct linux32_sys_getdents64_args *, register_t *);

#define linux32_sys_fcntl64 linux32_sys_fcntl
#define linux32_sys_fcntl64_args linux32_sys_fcntl_args
int	linux32_sys_fcntl64(struct lwp *, const struct linux32_sys_fcntl64_args *, register_t *);

int	linux_sys_gettid(struct lwp *, const void *, register_t *);

int	linux32_sys_exit_group(struct lwp *, const struct linux32_sys_exit_group_args *, register_t *);

int	linux32_sys_clock_settime(struct lwp *, const struct linux32_sys_clock_settime_args *, register_t *);

int	linux32_sys_clock_gettime(struct lwp *, const struct linux32_sys_clock_gettime_args *, register_t *);

int	linux32_sys_clock_getres(struct lwp *, const struct linux32_sys_clock_getres_args *, register_t *);

#endif /* _LINUX32_SYS_SYSCALLARGS_H_ */
