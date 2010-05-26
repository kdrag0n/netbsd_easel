/*	$NetBSD: rumpvfs_if_priv.h,v 1.6 2010/05/26 21:51:58 pooka Exp $	*/

/*
 * Automatically generated.  DO NOT EDIT.
 * from: NetBSD: rumpvfs.ifspec,v 1.4 2010/05/26 21:50:56 pooka Exp 
 * by:   NetBSD: makerumpif.sh,v 1.4 2009/10/15 00:29:19 pooka Exp 
 */

void rump_getvninfo(struct vnode *, enum vtype *, off_t *, dev_t *);
struct vfsops * rump_vfslist_iterate(struct vfsops *);
struct vfsops * rump_vfs_getopsbyname(const char *);
struct vattr * rump_vattr_init(void);
void rump_vattr_settype(struct vattr *, enum vtype);
void rump_vattr_setmode(struct vattr *, mode_t);
void rump_vattr_setrdev(struct vattr *, dev_t);
void rump_vattr_free(struct vattr *);
void rump_vp_incref(struct vnode *);
int rump_vp_getref(struct vnode *);
void rump_vp_rele(struct vnode *);
void rump_vp_interlock(struct vnode *);
int rump_etfs_register(const char *, const char *, enum rump_etfs_type);
int rump_etfs_register_withsize(const char *, const char *, enum rump_etfs_type, uint64_t, uint64_t);
int rump_etfs_remove(const char *);
void rump_freecn(struct componentname *, int);
int rump_checksavecn(struct componentname *);
int rump_namei(uint32_t, uint32_t, const char *, struct vnode **, struct vnode **, struct componentname **);
struct componentname * rump_makecn(u_long, u_long, const char *, size_t, struct kauth_cred *, struct lwp *);
int rump_vfs_unmount(struct mount *, int);
int rump_vfs_root(struct mount *, struct vnode **, int);
int rump_vfs_statvfs(struct mount *, struct statvfs *);
int rump_vfs_sync(struct mount *, int, struct kauth_cred *);
int rump_vfs_fhtovp(struct mount *, struct fid *, struct vnode **);
int rump_vfs_vptofh(struct vnode *, struct fid *, size_t *);
int rump_vfs_extattrctl(struct mount *, int, struct vnode *, int, const char *);
void rump_vfs_syncwait(struct mount *);
int rump_vfs_getmp(const char *, struct mount **);
void rump_vfs_mount_print(const char *, int);
void rump_rcvp_set(struct vnode *, struct vnode *);
struct vnode * rump_cdir_get(void);
int rump_syspuffs_glueinit(int, int *);
int rump_sys___stat30(const char *, struct stat *);
int rump_sys___lstat30(const char *, struct stat *);
void rump_vattr50_to_vattr(const struct vattr *, struct vattr *);
void rump_vattr_to_vattr50(const struct vattr *, struct vattr *);
