/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.11 1996/09/07 13:23:39 mycroft Exp 
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/exec.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/hpux/hpux.h>
#include <compat/hpux/hpux_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent hpux_sysent[] = {
	{ 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args),
	    sys_exit },				/* 1 = exit */
	{ 0, 0,
	    hpux_sys_fork },			/* 2 = fork */
	{ 3, s(struct hpux_sys_read_args),
	    hpux_sys_read },			/* 3 = read */
	{ 3, s(struct hpux_sys_write_args),
	    hpux_sys_write },			/* 4 = write */
	{ 3, s(struct hpux_sys_open_args),
	    hpux_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args),
	    sys_close },			/* 6 = close */
	{ 1, s(struct hpux_sys_wait_args),
	    hpux_sys_wait },			/* 7 = wait */
	{ 2, s(struct hpux_sys_creat_args),
	    hpux_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args),
	    sys_link },				/* 9 = link */
	{ 1, s(struct hpux_sys_unlink_args),
	    hpux_sys_unlink },			/* 10 = unlink */
	{ 2, s(struct hpux_sys_execv_args),
	    hpux_sys_execv },			/* 11 = execv */
	{ 1, s(struct hpux_sys_chdir_args),
	    hpux_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct hpux_sys_time_6x_args),
	    hpux_sys_time_6x },			/* 13 = time_6x */
	{ 3, s(struct hpux_sys_mknod_args),
	    hpux_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct hpux_sys_chmod_args),
	    hpux_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct hpux_sys_chown_args),
	    hpux_sys_chown },			/* 16 = chown */
	{ 1, s(struct sys_obreak_args),
	    sys_obreak },			/* 17 = obreak */
	{ 2, s(struct hpux_sys_stat_6x_args),
	    hpux_sys_stat_6x },			/* 18 = stat_6x */
	{ 3, s(struct compat_43_sys_lseek_args),
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0,
	    sys_getpid },			/* 20 = getpid */
	{ 0, 0,
	    sys_nosys },			/* 21 = unimplemented mount */
	{ 0, 0,
	    sys_nosys },			/* 22 = unimplemented umount */
	{ 1, s(struct sys_setuid_args),
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 1, s(struct hpux_sys_stime_6x_args),
	    hpux_sys_stime_6x },		/* 25 = stime_6x */
	{ 4, s(struct hpux_sys_ptrace_args),
	    hpux_sys_ptrace },			/* 26 = ptrace */
	{ 1, s(struct hpux_sys_alarm_6x_args),
	    hpux_sys_alarm_6x },		/* 27 = alarm_6x */
	{ 2, s(struct hpux_sys_fstat_6x_args),
	    hpux_sys_fstat_6x },		/* 28 = fstat_6x */
	{ 0, 0,
	    hpux_sys_pause_6x },		/* 29 = pause_6x */
	{ 2, s(struct hpux_sys_utime_6x_args),
	    hpux_sys_utime_6x },		/* 30 = utime_6x */
	{ 2, s(struct hpux_sys_stty_6x_args),
	    hpux_sys_stty_6x },			/* 31 = stty_6x */
	{ 2, s(struct hpux_sys_gtty_6x_args),
	    hpux_sys_gtty_6x },			/* 32 = gtty_6x */
	{ 2, s(struct hpux_sys_access_args),
	    hpux_sys_access },			/* 33 = access */
	{ 1, s(struct hpux_sys_nice_6x_args),
	    hpux_sys_nice_6x },			/* 34 = nice_6x */
	{ 1, s(struct hpux_sys_ftime_6x_args),
	    hpux_sys_ftime_6x },		/* 35 = ftime_6x */
	{ 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct hpux_sys_kill_args),
	    hpux_sys_kill },			/* 37 = kill */
	{ 2, s(struct hpux_sys_stat_args),
	    hpux_sys_stat },			/* 38 = stat */
	{ 0, 0,
	    hpux_sys_setpgrp_6x },		/* 39 = setpgrp_6x */
	{ 2, s(struct hpux_sys_lstat_args),
	    hpux_sys_lstat },			/* 40 = lstat */
	{ 1, s(struct hpux_sys_dup_args),
	    hpux_sys_dup },			/* 41 = dup */
	{ 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 1, s(struct hpux_sys_times_6x_args),
	    hpux_sys_times_6x },		/* 43 = times_6x */
	{ 4, s(struct sys_profil_args),
	    sys_profil },			/* 44 = profil */
	{ 0, 0,
	    sys_nosys },			/* 45 = unimplemented ki_syscall */
	{ 1, s(struct sys_setgid_args),
	    sys_setgid },			/* 46 = setgid */
	{ 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 2, s(struct hpux_sys_ssig_6x_args),
	    hpux_sys_ssig_6x },			/* 48 = ssig_6x */
	{ 0, 0,
	    sys_nosys },			/* 49 = unimplemented reserved for USG */
	{ 0, 0,
	    sys_nosys },			/* 50 = unimplemented reserved for USG */
	{ 0, 0,
	    sys_nosys },			/* 51 = unimplemented acct */
	{ 0, 0,
	    sys_nosys },			/* 52 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 53 = unimplemented */
	{ 3, s(struct hpux_sys_ioctl_args),
	    hpux_sys_ioctl },			/* 54 = ioctl */
	{ 0, 0,
	    sys_nosys },			/* 55 = unimplemented reboot */
	{ 2, s(struct hpux_sys_symlink_args),
	    hpux_sys_symlink },			/* 56 = symlink */
	{ 3, s(struct hpux_sys_utssys_args),
	    hpux_sys_utssys },			/* 57 = utssys */
	{ 3, s(struct hpux_sys_readlink_args),
	    hpux_sys_readlink },		/* 58 = readlink */
	{ 3, s(struct hpux_sys_execve_args),
	    hpux_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args),
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args),
	    sys_chroot },			/* 61 = chroot */
	{ 3, s(struct hpux_sys_fcntl_args),
	    hpux_sys_fcntl },			/* 62 = fcntl */
	{ 2, s(struct hpux_sys_ulimit_args),
	    hpux_sys_ulimit },			/* 63 = ulimit */
	{ 0, 0,
	    sys_nosys },			/* 64 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 65 = unimplemented */
	{ 0, 0,
	    hpux_sys_vfork },			/* 66 = vfork */
	{ 3, s(struct hpux_sys_read_args),
	    hpux_sys_read },			/* 67 = vread */
	{ 3, s(struct hpux_sys_write_args),
	    hpux_sys_write },			/* 68 = vwrite */
	{ 0, 0,
	    sys_nosys },			/* 69 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 70 = unimplemented */
	{ 6, s(struct hpux_sys_mmap_args),
	    hpux_sys_mmap },			/* 71 = mmap */
	{ 0, 0,
	    sys_nosys },			/* 72 = unimplemented */
	{ 2, s(struct sys_munmap_args),
	    sys_munmap },			/* 73 = munmap */
	{ 3, s(struct sys_mprotect_args),
	    sys_mprotect },			/* 74 = mprotect */
	{ 0, 0,
	    sys_nosys },			/* 75 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 76 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 77 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 78 = unimplemented */
	{ 2, s(struct sys_getgroups_args),
	    sys_getgroups },			/* 79 = getgroups */
	{ 2, s(struct sys_setgroups_args),
	    sys_setgroups },			/* 80 = setgroups */
	{ 1, s(struct hpux_sys_getpgrp2_args),
	    hpux_sys_getpgrp2 },		/* 81 = getpgrp2 */
	{ 2, s(struct hpux_sys_setpgrp2_args),
	    hpux_sys_setpgrp2 },		/* 82 = setpgrp2 */
	{ 3, s(struct sys_setitimer_args),
	    sys_setitimer },			/* 83 = setitimer */
	{ 3, s(struct hpux_sys_wait3_args),
	    hpux_sys_wait3 },			/* 84 = wait3 */
	{ 0, 0,
	    sys_nosys },			/* 85 = unimplemented swapon */
	{ 2, s(struct sys_getitimer_args),
	    sys_getitimer },			/* 86 = getitimer */
	{ 0, 0,
	    sys_nosys },			/* 87 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 88 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 89 = unimplemented */
	{ 2, s(struct sys_dup2_args),
	    sys_dup2 },				/* 90 = dup2 */
	{ 0, 0,
	    sys_nosys },			/* 91 = unimplemented */
	{ 2, s(struct hpux_sys_fstat_args),
	    hpux_sys_fstat },			/* 92 = fstat */
	{ 5, s(struct sys_select_args),
	    sys_select },			/* 93 = select */
	{ 0, 0,
	    sys_nosys },			/* 94 = unimplemented */
	{ 1, s(struct sys_fsync_args),
	    sys_fsync },			/* 95 = fsync */
	{ 0, 0,
	    sys_nosys },			/* 96 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 97 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 98 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 99 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 100 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 101 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 102 = unimplemented */
	{ 1, s(struct sys_sigreturn_args),
	    sys_sigreturn },			/* 103 = sigreturn */
	{ 0, 0,
	    sys_nosys },			/* 104 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 105 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 106 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 107 = unimplemented */
	{ 3, s(struct hpux_sys_sigvec_args),
	    hpux_sys_sigvec },			/* 108 = sigvec */
	{ 1, s(struct hpux_sys_sigblock_args),
	    hpux_sys_sigblock },		/* 109 = sigblock */
	{ 1, s(struct hpux_sys_sigsetmask_args),
	    hpux_sys_sigsetmask },		/* 110 = sigsetmask */
	{ 1, s(struct hpux_sys_sigpause_args),
	    hpux_sys_sigpause },		/* 111 = sigpause */
	{ 2, s(struct compat_43_sys_sigstack_args),
	    compat_43_sys_sigstack },		/* 112 = sigstack */
	{ 0, 0,
	    sys_nosys },			/* 113 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 114 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 115 = unimplemented */
	{ 1, s(struct sys_gettimeofday_args),
	    sys_gettimeofday },			/* 116 = gettimeofday */
	{ 0, 0,
	    sys_nosys },			/* 117 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 118 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 119 = unimplemented io_stub */
	{ 3, s(struct hpux_sys_readv_args),
	    hpux_sys_readv },			/* 120 = readv */
	{ 3, s(struct hpux_sys_writev_args),
	    hpux_sys_writev },			/* 121 = writev */
	{ 2, s(struct sys_settimeofday_args),
	    sys_settimeofday },			/* 122 = settimeofday */
	{ 3, s(struct sys_fchown_args),
	    sys_fchown },			/* 123 = fchown */
	{ 2, s(struct sys_fchmod_args),
	    sys_fchmod },			/* 124 = fchmod */
	{ 0, 0,
	    sys_nosys },			/* 125 = unimplemented */
	{ 3, s(struct hpux_sys_setresuid_args),
	    hpux_sys_setresuid },		/* 126 = setresuid */
	{ 3, s(struct hpux_sys_setresgid_args),
	    hpux_sys_setresgid },		/* 127 = setresgid */
	{ 2, s(struct hpux_sys_rename_args),
	    hpux_sys_rename },			/* 128 = rename */
	{ 2, s(struct hpux_sys_truncate_args),
	    hpux_sys_truncate },		/* 129 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args),
	    compat_43_sys_ftruncate },		/* 130 = ftruncate */
	{ 0, 0,
	    sys_nosys },			/* 131 = unimplemented */
	{ 1, s(struct hpux_sys_sysconf_args),
	    hpux_sys_sysconf },			/* 132 = sysconf */
	{ 0, 0,
	    sys_nosys },			/* 133 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 134 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 135 = unimplemented */
	{ 2, s(struct hpux_sys_mkdir_args),
	    hpux_sys_mkdir },			/* 136 = mkdir */
	{ 1, s(struct hpux_sys_rmdir_args),
	    hpux_sys_rmdir },			/* 137 = rmdir */
	{ 0, 0,
	    sys_nosys },			/* 138 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 139 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 140 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 141 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 142 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 143 = unimplemented */
	{ 2, s(struct hpux_sys_getrlimit_args),
	    hpux_sys_getrlimit },		/* 144 = getrlimit */
	{ 2, s(struct hpux_sys_setrlimit_args),
	    hpux_sys_setrlimit },		/* 145 = setrlimit */
	{ 0, 0,
	    sys_nosys },			/* 146 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 147 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 148 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 149 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 150 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 151 = unimplemented privgrp */
	{ 2, s(struct hpux_sys_rtprio_args),
	    hpux_sys_rtprio },			/* 152 = rtprio */
	{ 0, 0,
	    sys_nosys },			/* 153 = unimplemented plock */
	{ 2, s(struct hpux_sys_netioctl_args),
	    hpux_sys_netioctl },		/* 154 = netioctl */
	{ 3, s(struct hpux_sys_lockf_args),
	    hpux_sys_lockf },			/* 155 = lockf */
#ifdef SYSVSEM
	{ 3, s(struct sys_semget_args),
	    sys_semget },			/* 156 = semget */
	{ 4, s(struct sys___semctl_args),
	    sys___semctl },			/* 157 = __semctl */
	{ 3, s(struct sys_semop_args),
	    sys_semop },			/* 158 = semop */
#else
	{ 0, 0,
	    sys_nosys },			/* 156 = unimplemented semget */
	{ 0, 0,
	    sys_nosys },			/* 157 = unimplemented semctl */
	{ 0, 0,
	    sys_nosys },			/* 158 = unimplemented semop */
#endif
#ifdef SYSVMSG
	{ 2, s(struct sys_msgget_args),
	    sys_msgget },			/* 159 = msgget */
	{ 3, s(struct sys_msgctl_args),
	    sys_msgctl },			/* 160 = msgctl */
	{ 4, s(struct sys_msgsnd_args),
	    sys_msgsnd },			/* 161 = msgsnd */
	{ 5, s(struct sys_msgrcv_args),
	    sys_msgrcv },			/* 162 = msgrcv */
#else
	{ 0, 0,
	    sys_nosys },			/* 159 = unimplemented msgget */
	{ 0, 0,
	    sys_nosys },			/* 160 = unimplemented msgctl */
	{ 0, 0,
	    sys_nosys },			/* 161 = unimplemented msgsnd */
	{ 0, 0,
	    sys_nosys },			/* 162 = unimplemented msgrcv */
#endif
#ifdef SYSVSHM
	{ 3, s(struct sys_shmget_args),
	    sys_shmget },			/* 163 = shmget */
	{ 3, s(struct hpux_sys_shmctl_args),
	    hpux_sys_shmctl },			/* 164 = shmctl */
	{ 3, s(struct sys_shmat_args),
	    sys_shmat },			/* 165 = shmat */
	{ 1, s(struct sys_shmdt_args),
	    sys_shmdt },			/* 166 = shmdt */
#else
	{ 0, 0,
	    sys_nosys },			/* 163 = unimplemented shmget */
	{ 0, 0,
	    sys_nosys },			/* 164 = unimplemented shmctl */
	{ 0, 0,
	    sys_nosys },			/* 165 = unimplemented shmat */
	{ 0, 0,
	    sys_nosys },			/* 166 = unimplemented shmdt */
#endif
	{ 1, s(struct hpux_sys_advise_args),
	    hpux_sys_advise },			/* 167 = advise */
	{ 0, 0,
	    sys_nosys },			/* 168 = unimplemented nsp_init */
	{ 0, 0,
	    sys_nosys },			/* 169 = unimplemented cluster */
	{ 0, 0,
	    sys_nosys },			/* 170 = unimplemented mkrnod */
	{ 0, 0,
	    sys_nosys },			/* 171 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 172 = unimplemented unsp_open */
	{ 0, 0,
	    sys_nosys },			/* 173 = unimplemented */
	{ 2, s(struct hpux_sys_getcontext_args),
	    hpux_sys_getcontext },		/* 174 = getcontext */
	{ 0, 0,
	    sys_nosys },			/* 175 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 176 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 178 = unimplemented lsync */
	{ 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 180 = unimplemented mysite */
	{ 0, 0,
	    sys_nosys },			/* 181 = unimplemented sitels */
	{ 0, 0,
	    sys_nosys },			/* 182 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 183 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 184 = unimplemented dskless_stats */
	{ 0, 0,
	    sys_nosys },			/* 185 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 186 = unimplemented setacl */
	{ 0, 0,
	    sys_nosys },			/* 187 = unimplemented fsetacl */
	{ 0, 0,
	    sys_nosys },			/* 188 = unimplemented getacl */
	{ 0, 0,
	    sys_nosys },			/* 189 = unimplemented fgetacl */
	{ 6, s(struct hpux_sys_getaccess_args),
	    hpux_sys_getaccess },		/* 190 = getaccess */
	{ 0, 0,
	    sys_nosys },			/* 191 = unimplemented getaudid */
	{ 0, 0,
	    sys_nosys },			/* 192 = unimplemented setaudid */
	{ 0, 0,
	    sys_nosys },			/* 193 = unimplemented getaudproc */
	{ 0, 0,
	    sys_nosys },			/* 194 = unimplemented setaudproc */
	{ 0, 0,
	    sys_nosys },			/* 195 = unimplemented getevent */
	{ 0, 0,
	    sys_nosys },			/* 196 = unimplemented setevent */
	{ 0, 0,
	    sys_nosys },			/* 197 = unimplemented audwrite */
	{ 0, 0,
	    sys_nosys },			/* 198 = unimplemented audswitch */
	{ 0, 0,
	    sys_nosys },			/* 199 = unimplemented audctl */
	{ 4, s(struct hpux_sys_waitpid_args),
	    hpux_sys_waitpid },			/* 200 = waitpid */
	{ 0, 0,
	    sys_nosys },			/* 201 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 202 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 203 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 204 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 205 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 206 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 207 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 208 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 209 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 210 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 211 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 212 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 213 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 214 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 215 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 216 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 217 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 218 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 219 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 220 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 221 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 222 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 223 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 224 = unimplemented */
	{ 2, s(struct sys_pathconf_args),
	    sys_pathconf },			/* 225 = pathconf */
	{ 2, s(struct sys_fpathconf_args),
	    sys_fpathconf },			/* 226 = fpathconf */
	{ 0, 0,
	    sys_nosys },			/* 227 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 228 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 229 = unimplemented async_daemon */
	{ 0, 0,
	    sys_nosys },			/* 230 = unimplemented nfs_fcntl */
	{ 4, s(struct compat_43_sys_getdirentries_args),
	    compat_43_sys_getdirentries },	/* 231 = getdirentries */
	{ 2, s(struct compat_09_sys_getdomainname_args),
	    compat_09_sys_getdomainname },	/* 232 = getdomainname */
	{ 0, 0,
	    sys_nosys },			/* 233 = unimplemented nfs_getfh */
	{ 0, 0,
	    sys_nosys },			/* 234 = unimplemented vfsmount */
	{ 0, 0,
	    sys_nosys },			/* 235 = unimplemented nfs_svc */
	{ 2, s(struct compat_09_sys_setdomainname_args),
	    compat_09_sys_setdomainname },	/* 236 = setdomainname */
	{ 0, 0,
	    sys_nosys },			/* 237 = unimplemented statfs */
	{ 0, 0,
	    sys_nosys },			/* 238 = unimplemented fstatfs */
	{ 3, s(struct hpux_sys_sigaction_args),
	    hpux_sys_sigaction },		/* 239 = sigaction */
	{ 3, s(struct hpux_sys_sigprocmask_args),
	    hpux_sys_sigprocmask },		/* 240 = sigprocmask */
	{ 1, s(struct hpux_sys_sigpending_args),
	    hpux_sys_sigpending },		/* 241 = sigpending */
	{ 1, s(struct hpux_sys_sigsuspend_args),
	    hpux_sys_sigsuspend },		/* 242 = sigsuspend */
	{ 0, 0,
	    sys_nosys },			/* 243 = unimplemented fsctl */
	{ 0, 0,
	    sys_nosys },			/* 244 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 245 = unimplemented pstat */
	{ 0, 0,
	    sys_nosys },			/* 246 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 247 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 248 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 249 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 250 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 251 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 252 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 253 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 254 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 255 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 256 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 257 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 258 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 259 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 260 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 261 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 262 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 263 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 264 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 265 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 266 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 267 = unimplemented */
	{ 0, 0,
	    compat_43_sys_getdtablesize },	/* 268 = getdtablesize */
	{ 3, s(struct sys_poll_args),
	    sys_poll },				/* 269 = poll */
	{ 0, 0,
	    sys_nosys },			/* 270 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 271 = unimplemented */
	{ 1, s(struct sys_fchdir_args),
	    sys_fchdir },			/* 272 = fchdir */
	{ 0, 0,
	    sys_nosys },			/* 273 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 274 = unimplemented */
	{ 3, s(struct compat_43_sys_accept_args),
	    compat_43_sys_accept },		/* 275 = accept */
	{ 3, s(struct sys_bind_args),
	    sys_bind },				/* 276 = bind */
	{ 3, s(struct sys_connect_args),
	    sys_connect },			/* 277 = connect */
	{ 3, s(struct compat_43_sys_getpeername_args),
	    compat_43_sys_getpeername },	/* 278 = getpeername */
	{ 3, s(struct compat_43_sys_getsockname_args),
	    compat_43_sys_getsockname },	/* 279 = getsockname */
	{ 5, s(struct sys_getsockopt_args),
	    sys_getsockopt },			/* 280 = getsockopt */
	{ 2, s(struct sys_listen_args),
	    sys_listen },			/* 281 = listen */
	{ 4, s(struct compat_43_sys_recv_args),
	    compat_43_sys_recv },		/* 282 = recv */
	{ 6, s(struct compat_43_sys_recvfrom_args),
	    compat_43_sys_recvfrom },		/* 283 = recvfrom */
	{ 3, s(struct compat_43_sys_recvmsg_args),
	    compat_43_sys_recvmsg },		/* 284 = recvmsg */
	{ 4, s(struct compat_43_sys_send_args),
	    compat_43_sys_send },		/* 285 = send */
	{ 3, s(struct compat_43_sys_sendmsg_args),
	    compat_43_sys_sendmsg },		/* 286 = sendmsg */
	{ 6, s(struct sys_sendto_args),
	    sys_sendto },			/* 287 = sendto */
	{ 5, s(struct hpux_sys_setsockopt2_args),
	    hpux_sys_setsockopt2 },		/* 288 = setsockopt2 */
	{ 2, s(struct sys_shutdown_args),
	    sys_shutdown },			/* 289 = shutdown */
	{ 3, s(struct sys_socket_args),
	    sys_socket },			/* 290 = socket */
	{ 4, s(struct sys_socketpair_args),
	    sys_socketpair },			/* 291 = socketpair */
	{ 0, 0,
	    sys_nosys },			/* 292 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 293 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 294 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 295 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 296 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 297 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 298 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 299 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 300 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 301 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 302 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 303 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 304 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 305 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 306 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 307 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 308 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 309 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 310 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 311 = unimplemented */
#ifdef SYSVSEM
	{ 4, s(struct sys___semctl_args),
	    sys___semctl },			/* 312 = nsemctl */
#else
	{ 0, 0,
	    sys_nosys },			/* 312 = unimplemented semctl */
#endif
#ifdef SYSVMSG
	{ 3, s(struct sys_msgctl_args),
	    sys_msgctl },			/* 313 = nmsgctl */
#else
	{ 0, 0,
	    sys_nosys },			/* 313 = unimplemented msgctl */
#endif
#ifdef SYSVSHM
	{ 3, s(struct hpux_sys_nshmctl_args),
	    hpux_sys_nshmctl },			/* 314 = nshmctl */
#else
	{ 0, 0,
	    sys_nosys },			/* 314 = unimplemented shmctl */
#endif
};

