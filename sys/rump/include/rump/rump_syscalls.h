/* $NetBSD: rump_syscalls.h,v 1.8 2009/01/26 12:21:24 pooka Exp $ */

/*
 * System call protos in rump namespace.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.222 2009/01/26 12:20:05 pooka Exp
 */

#ifdef _RUMPKERNEL
#error Interface not supported inside rump kernel
#endif /* _RUMPKERNEL */

ssize_t rump_sys_read(int, void *, size_t);
ssize_t rump_sys_write(int, const void *, size_t);
int rump_sys_open(const char *, int, mode_t);
int rump_sys_close(int);
int rump_sys_link(const char *, const char *);
int rump_sys_unlink(const char *);
int rump_sys_chdir(const char *);
int rump_sys_fchdir(int);
int rump_sys_chmod(const char *, mode_t);
int rump_sys_chown(const char *, uid_t, gid_t);
int rump_sys_unmount(const char *, int);
ssize_t rump_sys_recvmsg(int, struct msghdr *, int);
ssize_t rump_sys_sendmsg(int, const struct msghdr *, int);
ssize_t rump_sys_recvfrom(int, void *, size_t, int, struct sockaddr *, unsigned int *);
int rump_sys_accept(int, struct sockaddr *, unsigned int *);
int rump_sys_getpeername(int, struct sockaddr *, unsigned int *);
int rump_sys_getsockname(int, struct sockaddr *, unsigned int *);
int rump_sys_chflags(const char *, u_long);
void rump_sys_sync(void);
int rump_sys_ioctl(int, u_long, void *);
int rump_sys_symlink(const char *, const char *);
ssize_t rump_sys_readlink(const char *, char *, size_t);
int rump_sys_fsync(int);
int rump_sys_connect(int, const struct sockaddr *, unsigned int);
int rump_sys_bind(int, const struct sockaddr *, unsigned int);
int rump_sys_setsockopt(int, int, int, const void *, unsigned int);
int rump_sys_listen(int, int);
int rump_sys_getsockopt(int, int, int, void *, unsigned int *);
int rump_sys_rename(const char *, const char *);
int rump_sys_mkfifo(const char *, mode_t);
ssize_t rump_sys_sendto(int, const void *, size_t, int, const struct sockaddr *, unsigned int);
int rump_sys_shutdown(int, int);
int rump_sys_socketpair(int, int, int, int *);
int rump_sys_mkdir(const char *, mode_t);
int rump_sys_rmdir(const char *);
int rump_sys_nfssvc(int, void *);
ssize_t rump_sys_pread(int, void *, size_t, int, off_t);
ssize_t rump_sys_pwrite(int, const void *, size_t, int, off_t);
int rump_sys_truncate(const char *, int, off_t);
int rump_sys___sysctl(const int *, u_int, void *, size_t *, const void *, size_t);
int rump_sys_lchmod(const char *, mode_t);
int rump_sys_lchown(const char *, uid_t, gid_t);
int rump_sys_lchflags(const char *, u_long);
int rump_sys_statvfs1(const char *, struct statvfs *, int);
int rump_sys_socket(int, int, int) __RENAME(rump_sys___socket30);
int rump_sys_getfh(const char *, void *, size_t *) __RENAME(rump_sys___getfh30);
int rump_sys_utimes(const char *, const struct timeval *) __RENAME(rump_sys___utimes50);
int rump_sys_lutimes(const char *, const struct timeval *) __RENAME(rump_sys___lutimes50);
int rump_sys_stat(const char *, struct stat *) __RENAME(rump_sys___stat50);
int rump_sys_lstat(const char *, struct stat *) __RENAME(rump_sys___lstat50);
int rump_sys_mknod(const char *, mode_t, dev_t) __RENAME(rump_sys___mknod50);
