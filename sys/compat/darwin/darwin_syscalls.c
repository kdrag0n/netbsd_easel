/* $NetBSD: darwin_syscalls.c,v 1.15 2002/12/24 12:15:46 manu Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.8 2002/12/08 21:53:18 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: darwin_syscalls.c,v 1.15 2002/12/24 12:15:46 manu Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ktrace.h"
#include "opt_nfsserver.h"
#include "opt_ntp.h"
#include "opt_compat_netbsd.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "fs_lfs.h"
#include "fs_nfs.h"
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/common/compat_file.h>
#include <compat/mach/mach_types.h>
#include <compat/mach/mach_vm.h>
#include <compat/darwin/darwin_signal.h>
#include <compat/darwin/darwin_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const darwin_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"wait4",			/* 7 = wait4 */
	"ocreat",			/* 8 = ocreat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"#11 (unimplemented execv)",		/* 11 = unimplemented execv */
	"chdir",			/* 12 = chdir */
	"fchdir",			/* 13 = fchdir */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"break",			/* 17 = break */
	"getfsstat",			/* 18 = getfsstat */
	"olseek",			/* 19 = olseek */
	"getpid",			/* 20 = getpid */
	"mount",			/* 21 = mount */
	"unmount",			/* 22 = unmount */
	"setuid",			/* 23 = setuid */
#ifdef COMPAT_43
	"getuid",			/* 24 = getuid */
#else
	"getuid",			/* 24 = getuid */
#endif
	"geteuid",			/* 25 = geteuid */
	"ptrace",			/* 26 = ptrace */
	"recvmsg",			/* 27 = recvmsg */
	"sendmsg",			/* 28 = sendmsg */
	"recvfrom",			/* 29 = recvfrom */
	"accept",			/* 30 = accept */
	"getpeername",			/* 31 = getpeername */
	"getsockname",			/* 32 = getsockname */
	"access",			/* 33 = access */
	"chflags",			/* 34 = chflags */
	"fchflags",			/* 35 = fchflags */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"stat43",			/* 38 = stat43 */
	"getppid",			/* 39 = getppid */
	"lstat43",			/* 40 = lstat43 */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"getegid",			/* 43 = getegid */
	"profil",			/* 44 = profil */
#if defined(KTRACE) || !defined(_KERNEL)
	"ktrace",			/* 45 = ktrace */
#else
	"#45 (excluded ktrace)",		/* 45 = excluded ktrace */
#endif
	"sigaction",			/* 46 = sigaction */
#ifdef COMPAT_43
	"getgid",			/* 47 = getgid */
#else
	"getgid",			/* 47 = getgid */
#endif
	"sigprocmask13",			/* 48 = sigprocmask13 */
	"__getlogin",			/* 49 = __getlogin */
	"setlogin",			/* 50 = setlogin */
	"acct",			/* 51 = acct */
	"sigpending13",			/* 52 = sigpending13 */
	"sigaltstack13",			/* 53 = sigaltstack13 */
	"ioctl",			/* 54 = ioctl */
	"oreboot",			/* 55 = oreboot */
	"revoke",			/* 56 = revoke */
	"symlink",			/* 57 = symlink */
	"readlink",			/* 58 = readlink */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"fstat43",			/* 62 = fstat43 */
	"#63 (unimplemented)",		/* 63 = unimplemented */
	"ogetpagesize",			/* 64 = ogetpagesize */
	"msync",			/* 65 = msync */
	"vfork",			/* 66 = vfork */
	"#67 (obsolete vread)",		/* 67 = obsolete vread */
	"#68 (obsolete vwrite)",		/* 68 = obsolete vwrite */
	"sbrk",			/* 69 = sbrk */
	"sstk",			/* 70 = sstk */
	"ommap",			/* 71 = ommap */
	"vadvise",			/* 72 = vadvise */
	"munmap",			/* 73 = munmap */
	"mprotect",			/* 74 = mprotect */
	"madvise",			/* 75 = madvise */
	"#76 (unimplemented)",		/* 76 = unimplemented */
	"#77 (unimplemented)",		/* 77 = unimplemented */
	"mincore",			/* 78 = mincore */
	"getgroups",			/* 79 = getgroups */
	"setgroups",			/* 80 = setgroups */
	"getpgrp",			/* 81 = getpgrp */
	"setpgid",			/* 82 = setpgid */
	"setitimer",			/* 83 = setitimer */
	"owait",			/* 84 = owait */
	"swapon",			/* 85 = swapon */
	"getitimer",			/* 86 = getitimer */
	"ogethostname",			/* 87 = ogethostname */
	"osethostname",			/* 88 = osethostname */
	"ogetdtablesize",			/* 89 = ogetdtablesize */
	"dup2",			/* 90 = dup2 */
	"#91 (unimplemented)",		/* 91 = unimplemented */
	"fcntl",			/* 92 = fcntl */
	"select",			/* 93 = select */
	"#94 (unimplemented)",		/* 94 = unimplemented */
	"fsync",			/* 95 = fsync */
	"setpriority",			/* 96 = setpriority */
	"socket",			/* 97 = socket */
	"connect",			/* 98 = connect */
	"oaccept",			/* 99 = oaccept */
	"getpriority",			/* 100 = getpriority */
	"osend",			/* 101 = osend */
	"orecv",			/* 102 = orecv */
	"sigreturn",			/* 103 = sigreturn */
	"bind",			/* 104 = bind */
	"setsockopt",			/* 105 = setsockopt */
	"listen",			/* 106 = listen */
	"#107 (unimplemented)",		/* 107 = unimplemented */
	"osigvec",			/* 108 = osigvec */
	"osigblock",			/* 109 = osigblock */
	"osigsetmask",			/* 110 = osigsetmask */
	"sigsuspend13",			/* 111 = sigsuspend13 */
	"osigstack",			/* 112 = osigstack */
	"orecvmsg",			/* 113 = orecvmsg */
	"osendmsg",			/* 114 = osendmsg */
	"#115 (unimplemented)",		/* 115 = unimplemented */
	"gettimeofday",			/* 116 = gettimeofday */
	"getrusage",			/* 117 = getrusage */
	"getsockopt",			/* 118 = getsockopt */
	"#119 (unimplemented)",		/* 119 = unimplemented */
	"readv",			/* 120 = readv */
	"writev",			/* 121 = writev */
	"settimeofday",			/* 122 = settimeofday */
	"fchown",			/* 123 = fchown */
	"fchmod",			/* 124 = fchmod */
	"orecvfrom",			/* 125 = orecvfrom */
	"setreuid",			/* 126 = setreuid */
	"setregid",			/* 127 = setregid */
	"rename",			/* 128 = rename */
	"otruncate",			/* 129 = otruncate */
	"oftruncate",			/* 130 = oftruncate */
	"flock",			/* 131 = flock */
	"mkfifo",			/* 132 = mkfifo */
	"sendto",			/* 133 = sendto */
	"shutdown",			/* 134 = shutdown */
	"socketpair",			/* 135 = socketpair */
	"mkdir",			/* 136 = mkdir */
	"rmdir",			/* 137 = rmdir */
	"utimes",			/* 138 = utimes */
	"#139 (unimplemented futimes)",		/* 139 = unimplemented futimes */
	"adjtime",			/* 140 = adjtime */
	"ogetpeername",			/* 141 = ogetpeername */
	"ogethostid",			/* 142 = ogethostid */
	"#143 (unimplemented)",		/* 143 = unimplemented */
	"ogetrlimit",			/* 144 = ogetrlimit */
	"osetrlimit",			/* 145 = osetrlimit */
	"okillpg",			/* 146 = okillpg */
	"setsid",			/* 147 = setsid */
	"#148 (unimplemented)",		/* 148 = unimplemented */
	"#149 (unimplemented)",		/* 149 = unimplemented */
	"ogetsockname",			/* 150 = ogetsockname */
	"#151 (unimplemented getpgid)",		/* 151 = unimplemented getpgid */
	"#152 (unimplemented setprivexec)",		/* 152 = unimplemented setprivexec */
	"#153 (unimplemented pread)",		/* 153 = unimplemented pread */
	"#154 (unimplemented pwrite)",		/* 154 = unimplemented pwrite */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	"nfssvc",			/* 155 = nfssvc */
#else
	"#155 (excluded nfssvc)",		/* 155 = excluded nfssvc */
#endif
	"ogetdirentries",			/* 156 = ogetdirentries */
	"statfs",			/* 157 = statfs */
	"fstatfs",			/* 158 = fstatfs */
	"#159 (unimplemented unmount)",		/* 159 = unimplemented unmount */
	"#160 (unimplemented)",		/* 160 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	"getfh",			/* 161 = getfh */
#else
	"#161 (excluded getfh)",		/* 161 = excluded getfh */
#endif
	"ogetdomainname",			/* 162 = ogetdomainname */
	"osetdomainname",			/* 163 = osetdomainname */
	"#164 (unimplemented)",		/* 164 = unimplemented */
	"#165 (unimplemented quotactl)",		/* 165 = unimplemented quotactl */
	"#166 (unimplemented)",		/* 166 = unimplemented */
	"#167 (unimplemented mount)",		/* 167 = unimplemented mount */
	"#168 (unimplemented)",		/* 168 = unimplemented */
	"#169 (unimplemented)",		/* 169 = unimplemented */
	"#170 (unimplemented)",		/* 170 = unimplemented */
	"#171 (unimplemented wait3)",		/* 171 = unimplemented wait3 */
	"#172 (unimplemented)",		/* 172 = unimplemented */
	"#173 (unimplemented)",		/* 173 = unimplemented */
	"#174 (unimplemented)",		/* 174 = unimplemented */
	"#175 (unimplemented)",		/* 175 = unimplemented */
	"#176 (unimplemented add_profil)",		/* 176 = unimplemented add_profil */
	"#177 (unimplemented)",		/* 177 = unimplemented */
	"#178 (unimplemented)",		/* 178 = unimplemented */
	"#179 (unimplemented)",		/* 179 = unimplemented */
	"#180 (unimplemented kdebug_trace)",		/* 180 = unimplemented kdebug_trace */
	"setgid",			/* 181 = setgid */
	"setegid",			/* 182 = setegid */
	"seteuid",			/* 183 = seteuid */
	"#184 (unimplemented)",		/* 184 = unimplemented */
	"#185 (unimplemented)",		/* 185 = unimplemented */
	"#186 (unimplemented)",		/* 186 = unimplemented */
	"#187 (unimplemented)",		/* 187 = unimplemented */
	"stat12",			/* 188 = stat12 */
	"fstat",			/* 189 = fstat */
	"lstat12",			/* 190 = lstat12 */
	"pathconf",			/* 191 = pathconf */
	"fpathconf",			/* 192 = fpathconf */
	"#193 (unimplemented getfsstat)",		/* 193 = unimplemented getfsstat */
	"getrlimit",			/* 194 = getrlimit */
	"setrlimit",			/* 195 = setrlimit */
	"getdirentries",			/* 196 = getdirentries */
	"mmap",			/* 197 = mmap */
	"#198 (unimplemented)",		/* 198 = unimplemented */
	"lseek",			/* 199 = lseek */
	"truncate",			/* 200 = truncate */
	"ftruncate",			/* 201 = ftruncate */
	"__sysctl",			/* 202 = __sysctl */
	"mlock",			/* 203 = mlock */
	"munlock",			/* 204 = munlock */
	"undelete",			/* 205 = undelete */
	"#206 (unimplemented atsocket)",		/* 206 = unimplemented atsocket */
	"#207 (unimplemented atgetmsg)",		/* 207 = unimplemented atgetmsg */
	"#208 (unimplemented atputmsg)",		/* 208 = unimplemented atputmsg */
	"#209 (unimplemented atpsndreq)",		/* 209 = unimplemented atpsndreq */
	"#210 (unimplemented atpsndrsp)",		/* 210 = unimplemented atpsndrsp */
	"#211 (unimplemented atpgetreq)",		/* 211 = unimplemented atpgetreq */
	"#212 (unimplemented atpgetrsp)",		/* 212 = unimplemented atpgetrsp */
	"#213 (unimplemented)",		/* 213 = unimplemented */
	"#214 (unimplemented)",		/* 214 = unimplemented */
	"#215 (unimplemented)",		/* 215 = unimplemented */
	"#216 (unimplemented)",		/* 216 = unimplemented */
	"#217 (unimplemented)",		/* 217 = unimplemented */
	"#218 (unimplemented)",		/* 218 = unimplemented */
	"#219 (unimplemented)",		/* 219 = unimplemented */
	"#220 (unimplemented getattrlist)",		/* 220 = unimplemented getattrlist */
	"#221 (unimplemented setattrlist)",		/* 221 = unimplemented setattrlist */
	"#222 (unimplemented getdirentriesattr)",		/* 222 = unimplemented getdirentriesattr */
	"#223 (unimplemented exchangedata)",		/* 223 = unimplemented exchangedata */
	"#224 (unimplemented checkuseraccess)",		/* 224 = unimplemented checkuseraccess */
	"#225 (unimplemented searchfs)",		/* 225 = unimplemented searchfs */
	"#226 (unimplemented delete)",		/* 226 = unimplemented delete */
	"#227 (unimplemented copyfile)",		/* 227 = unimplemented copyfile */
	"#228 (unimplemented)",		/* 228 = unimplemented */
	"#229 (unimplemented)",		/* 229 = unimplemented */
	"#230 (unimplemented)",		/* 230 = unimplemented */
	"#231 (unimplemented watchevent)",		/* 231 = unimplemented watchevent */
	"#232 (unimplemented waitevent)",		/* 232 = unimplemented waitevent */
	"#233 (unimplemented modwatch)",		/* 233 = unimplemented modwatch */
	"#234 (unimplemented)",		/* 234 = unimplemented */
	"#235 (unimplemented)",		/* 235 = unimplemented */
	"#236 (unimplemented)",		/* 236 = unimplemented */
	"#237 (unimplemented)",		/* 237 = unimplemented */
	"#238 (unimplemented)",		/* 238 = unimplemented */
	"#239 (unimplemented)",		/* 239 = unimplemented */
	"#240 (unimplemented)",		/* 240 = unimplemented */
	"#241 (unimplemented)",		/* 241 = unimplemented */
	"#242 (unimplemented fsctl)",		/* 242 = unimplemented fsctl */
	"#243 (unimplemented)",		/* 243 = unimplemented */
	"#244 (unimplemented)",		/* 244 = unimplemented */
	"#245 (unimplemented)",		/* 245 = unimplemented */
	"#246 (unimplemented)",		/* 246 = unimplemented */
	"#247 (unimplemented)",		/* 247 = unimplemented */
	"#248 (unimplemented)",		/* 248 = unimplemented */
	"#249 (unimplemented)",		/* 249 = unimplemented */
	"#250 (unimplemented minherit)",		/* 250 = unimplemented minherit */
	"#251 (unimplemented semsys)",		/* 251 = unimplemented semsys */
	"#252 (unimplemented msgsys)",		/* 252 = unimplemented msgsys */
	"#253 (unimplemented semop)",		/* 253 = unimplemented semop */
	"#254 (unimplemented semctl)",		/* 254 = unimplemented semctl */
	"#255 (unimplemented semget)",		/* 255 = unimplemented semget */
	"#256 (unimplemented semop)",		/* 256 = unimplemented semop */
	"#257 (unimplemented semconfig)",		/* 257 = unimplemented semconfig */
	"#258 (unimplemented msgctl)",		/* 258 = unimplemented msgctl */
	"#259 (unimplemented msgget)",		/* 259 = unimplemented msgget */
	"#260 (unimplemented msgsnd)",		/* 260 = unimplemented msgsnd */
	"#261 (unimplemented msgrcv)",		/* 261 = unimplemented msgrcv */
	"#262 (unimplemented shmat)",		/* 262 = unimplemented shmat */
	"#263 (unimplemented shmctl)",		/* 263 = unimplemented shmctl */
	"#264 (unimplemented shmdt)",		/* 264 = unimplemented shmdt */
	"#265 (unimplemented shmget)",		/* 265 = unimplemented shmget */
	"#266 (unimplemented shm_open)",		/* 266 = unimplemented shm_open */
	"#267 (unimplemented shm_unlink)",		/* 267 = unimplemented shm_unlink */
	"#268 (unimplemented sem_open)",		/* 268 = unimplemented sem_open */
	"#269 (unimplemented sem_close)",		/* 269 = unimplemented sem_close */
	"#270 (unimplemented sem_unlink)",		/* 270 = unimplemented sem_unlink */
	"#271 (unimplemented sem_wait)",		/* 271 = unimplemented sem_wait */
	"#272 (unimplemented sem_trywait)",		/* 272 = unimplemented sem_trywait */
	"#273 (unimplemented sem_post)",		/* 273 = unimplemented sem_post */
	"#274 (unimplemented sem_getvalue)",		/* 274 = unimplemented sem_getvalue */
	"#275 (unimplemented sem_init)",		/* 275 = unimplemented sem_init */
	"#276 (unimplemented sem_destroy)",		/* 276 = unimplemented sem_destroy */
	"#277 (unimplemented)",		/* 277 = unimplemented */
	"#278 (unimplemented)",		/* 278 = unimplemented */
	"#279 (unimplemented)",		/* 279 = unimplemented */
	"#280 (unimplemented)",		/* 280 = unimplemented */
	"#281 (unimplemented)",		/* 281 = unimplemented */
	"#282 (unimplemented)",		/* 282 = unimplemented */
	"#283 (unimplemented)",		/* 283 = unimplemented */
	"#284 (unimplemented)",		/* 284 = unimplemented */
	"#285 (unimplemented)",		/* 285 = unimplemented */
	"#286 (unimplemented)",		/* 286 = unimplemented */
	"#287 (unimplemented)",		/* 287 = unimplemented */
	"#288 (unimplemented)",		/* 288 = unimplemented */
	"#289 (unimplemented)",		/* 289 = unimplemented */
	"#290 (unimplemented)",		/* 290 = unimplemented */
	"#291 (unimplemented)",		/* 291 = unimplemented */
	"#292 (unimplemented)",		/* 292 = unimplemented */
	"#293 (unimplemented)",		/* 293 = unimplemented */
	"#294 (unimplemented)",		/* 294 = unimplemented */
	"#295 (unimplemented)",		/* 295 = unimplemented */
	"load_shared_file",			/* 296 = load_shared_file */
	"#297 (unimplemented reset_shared_file)",		/* 297 = unimplemented reset_shared_file */
	"#298 (unimplemented new_system_shared_regions)",		/* 298 = unimplemented new_system_shared_regions */
	"#299 (unimplemented)",		/* 299 = unimplemented */
	"#300 (unimplemented)",		/* 300 = unimplemented */
	"#301 (unimplemented)",		/* 301 = unimplemented */
	"#302 (unimplemented)",		/* 302 = unimplemented */
	"#303 (unimplemented)",		/* 303 = unimplemented */
	"#304 (unimplemented)",		/* 304 = unimplemented */
	"#305 (unimplemented)",		/* 305 = unimplemented */
	"#306 (unimplemented)",		/* 306 = unimplemented */
	"#307 (unimplemented)",		/* 307 = unimplemented */
	"#308 (unimplemented)",		/* 308 = unimplemented */
	"#309 (unimplemented)",		/* 309 = unimplemented */
	"#310 (unimplemented getsid)",		/* 310 = unimplemented getsid */
	"#311 (unimplemented)",		/* 311 = unimplemented */
	"#312 (unimplemented)",		/* 312 = unimplemented */
	"#313 (unimplemented)",		/* 313 = unimplemented */
	"#314 (unimplemented)",		/* 314 = unimplemented */
	"#315 (unimplemented)",		/* 315 = unimplemented */
	"#316 (unimplemented)",		/* 316 = unimplemented */
	"#317 (unimplemented)",		/* 317 = unimplemented */
	"#318 (unimplemented)",		/* 318 = unimplemented */
	"#319 (unimplemented)",		/* 319 = unimplemented */
	"#320 (unimplemented)",		/* 320 = unimplemented */
	"#321 (unimplemented)",		/* 321 = unimplemented */
	"#322 (unimplemented)",		/* 322 = unimplemented */
	"#323 (unimplemented)",		/* 323 = unimplemented */
	"#324 (unimplemented mlockall)",		/* 324 = unimplemented mlockall */
	"#325 (unimplemented munlockall)",		/* 325 = unimplemented munlockall */
	"#326 (unimplemented)",		/* 326 = unimplemented */
	"#327 (unimplemented issetugid)",		/* 327 = unimplemented issetugid */
	"#328 (unimplemented __pthread_kill)",		/* 328 = unimplemented __pthread_kill */
	"#329 (unimplemented pthread_sigmask)",		/* 329 = unimplemented pthread_sigmask */
	"#330 (unimplemented sigwait)",		/* 330 = unimplemented sigwait */
	"#331 (unimplemented)",		/* 331 = unimplemented */
	"#332 (unimplemented)",		/* 332 = unimplemented */
	"#333 (unimplemented)",		/* 333 = unimplemented */
	"#334 (unimplemented)",		/* 334 = unimplemented */
	"#335 (unimplemented utrace)",		/* 335 = unimplemented utrace */
	"#336 (unimplemented)",		/* 336 = unimplemented */
	"#337 (unimplemented)",		/* 337 = unimplemented */
	"#338 (unimplemented)",		/* 338 = unimplemented */
	"#339 (unimplemented)",		/* 339 = unimplemented */
	"#340 (unimplemented)",		/* 340 = unimplemented */
	"#341 (unimplemented)",		/* 341 = unimplemented */
	"#342 (unimplemented)",		/* 342 = unimplemented */
	"#343 (unimplemented)",		/* 343 = unimplemented */
	"#344 (unimplemented)",		/* 344 = unimplemented */
	"#345 (unimplemented)",		/* 345 = unimplemented */
	"#346 (unimplemented)",		/* 346 = unimplemented */
	"#347 (unimplemented)",		/* 347 = unimplemented */
	"#348 (unimplemented)",		/* 348 = unimplemented */
	"#349 (unimplemented)",		/* 349 = unimplemented */
};
