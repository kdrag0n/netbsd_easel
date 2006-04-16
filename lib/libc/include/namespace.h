/*	$NetBSD: namespace.h,v 1.114 2006/04/16 17:03:32 christos Exp $	*/

/*-
 * Copyright (c) 1997-2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include <sys/cdefs.h>

#ifndef __lint__
#define brk		_brk
#define catclose	_catclose
#define catgets		_catgets
#define catopen		_catopen
#define daylight	_daylight
#define err		_err
#define errx		_errx
#ifdef _REENTRANT
#define fileno		_fileno
#endif /* _REENTRANT */
#define fork		_fork
#define fseeko		_fseeko
#define ftello		_ftello
#define getcontext	_getcontext
#define getenv_r	_getenv_r
#define inet_aton	_inet_aton
#define inet_pton	_inet_pton
#define pipe		_pipe
#define sbrk		_sbrk
#define strerror_r	_strerror_r
#define strlcat		_strlcat
#define strlcpy		_strlcpy
#define strtof		_strtof
#define strtoimax	_strtoimax
#define strtold		_strtold
#define strtoll		_strtoll
#define strtoull	_strtoull
#define strtoumax	_strtoumax
#define sys_errlist	_sys_errlist
#define sys_nerr	_sys_nerr
#define sys_siglist	_sys_siglist
#define	sys_nsig	_sys_nsig
#define sysconf		__sysconf
#define verr		_verr
#define verrx		_verrx
#define vwarn		_vwarn
#define vwarnx		_vwarnx
#define warn		_warn
#define warnx		_warnx

#ifdef __weak_alias
#define MD2Data			_MD2Data
#define MD2End			_MD2End
#define MD2FileChunk		_MD2FileChunk
#define MD2File			_MD2File
#define MD2Final		_MD2Final
#define MD2Init			_MD2Init
#define MD2Transform		_MD2Transform
#define MD2Update		_MD2Update
#define MD4Data			_MD4Data
#define MD4End			_MD4End
#define MD4FileChunk		_MD4FileChunk
#define MD4File			_MD4File
#define MD4Final		_MD4Final
#define MD4Init			_MD4Init
#define MD4Transform		_MD4Transform
#define MD4Update		_MD4Update
#define MD5Data			_MD5Data
#define MD5End			_MD5End
#define MD5FileChunk		_MD5FileChunk
#define MD5File			_MD5File
#define MD5Final		_MD5Final
#define MD5Init			_MD5Init
#define MD5Transform		_MD5Transform
#define MD5Update		_MD5Update
#define RMD160Data		_RMD160Data
#define RMD160End		_RMD160End
#define RMD160FileChunk		_RMD160FileChunk
#define RMD160File		_RMD160File
#define RMD160Final		_RMD160Final
#define RMD160Init		_RMD160Init
#define RMD160Transform		_RMD160Transform
#define RMD160Update		_RMD160Update
#define SHA1Data		_SHA1Data
#define SHA1End			_SHA1End
#define SHA1FileChunk		_SHA1FileChunk
#define SHA1File		_SHA1File
#define SHA1Final		_SHA1Final
#define SHA1Init		_SHA1Init
#define SHA1Transform		_SHA1Transform
#define SHA1Update		_SHA1Update
#define SHA256_Data		_SHA256_Data
#define SHA256_End		_SHA256_End
#define SHA256_FileChunk	_SHA256_FileChunk
#define SHA256_File		_SHA256_File
#define SHA256_Final		_SHA256_Final
#define SHA256_Init		_SHA256_Init
#define SHA256_Transform	_SHA256_Transform
#define SHA256_Update		_SHA256_Update
#define SHA384_Data		_SHA384_Data
#define SHA384_End		_SHA384_End
#define SHA384_FileChunk	_SHA384_FileChunk
#define SHA384_File		_SHA384_File
#define SHA384_Final		_SHA384_Final
#define SHA384_Init		_SHA384_Init
#define SHA384_Transform	_SHA384_Transform
#define SHA384_Update		_SHA384_Update
#define SHA512_Data		_SHA512_Data
#define SHA512_End		_SHA512_End
#define SHA512_FileChunk	_SHA512_FileChunk
#define SHA512_File		_SHA512_File
#define SHA512_Final		_SHA512_Final
#define SHA512_Init		_SHA512_Init
#define SHA512_Transform	_SHA512_Transform
#define SHA512_Update		_SHA512_Update
#define a64l			_a64l
#define adjtime			_adjtime
#define alarm			_alarm
#define alphasort		_alphasort
#define arc4random		_arc4random
#define asctime_r		_asctime_r
#define asprintf		_asprintf
#define atoll			_atoll
#define authnone_create		_authnone_create
#define authunix_create		_authunix_create
#define authunix_create_default _authunix_create_default
#define basename		_basename
#define bindresvport		_bindresvport
#define bindresvport_sa		_bindresvport_sa
#define bm_comp			_bm_comp
#define bm_exec			_bm_exec
#define bm_free			_bm_free
#define callrpc			_callrpc
#define cfgetispeed		_cfgetispeed
#define cfgetospeed		_cfgetospeed
#define cfmakeraw		_cfmakeraw
#define cfsetispeed		_cfsetispeed
#define cfsetospeed		_cfsetospeed
#define cfsetspeed		_cfsetspeed
#define cgetcap			_cgetcap
#define cgetclose		_cgetclose
#define cgetent			_cgetent
#define cgetfirst		_cgetfirst
#define cgetmatch		_cgetmatch
#define cgetnext		_cgetnext
#define cgetnum			_cgetnum
#define cgetset			_cgetset
#define cgetstr			_cgetstr
#define cgetustr		_cgetustr
#define clnt_broadcast		_clnt_broadcast
#define clnt_create		_clnt_create
#define clnt_create_vers	_clnt_create_vers
#define clnt_dg_create		_clnt_dg_create
#define clnt_pcreateerror	_clnt_pcreateerror
#define clnt_perrno		_clnt_perrno
#define clnt_perror		_clnt_perror
#define clnt_raw_create		_clnt_raw_create
#define clnt_tli_create		_clnt_tli_create
#define clnt_tp_create		_clnt_tp_create
#define clnt_spcreateerror	_clnt_spcreateerror
#define clnt_sperrno		_clnt_sperrno
#define clnt_sperror		_clnt_sperror
#define clnt_vc_create		_clnt_vc_create
#define clntraw_create		_clntraw_create
#define clnttcp_create		_clnttcp_create
#define clntudp_bufcreate	_clntudp_bufcreate
#define clntudp_create		_clntudp_create
#define clock_settime		_clock_settime
#define closedir		_closedir
#define closelog		_closelog
#define confstr			_confstr
#define ctermid			_ctermid
#define ctime_r			_ctime_r
#define daemon			_daemon
#define dbopen			_dbopen
#define devname			_devname
#define dirname			_dirname
#define dn_expand		_dn_expand
#define drand48			_drand48
#define endfsent		_endfsent
#define endgrent		_endgrent
#define endhostent		_endhostent
#define endnetconfig		_endnetconfig
#define endnetent		_endnetent
#define endnetgrent		_endnetgrent
#define endnetpath		_endnetpath
#define endprotoent		_endprotoent
#define endprotoent_r		_endprotoent_r
#define endpwent		_endpwent
#define endrpcent		_endrpcent
#define endservent		_endservent
#define endservent_r		_endservent_r
#define endttyent		_endttyent
#define endusershell		_endusershell
#define erand48			_erand48
#define ether_aton		_ether_aton
#define ether_hostton		_ether_hostton
#define ether_line		_ether_line
#define ether_ntoa		_ether_ntoa
#define ether_ntohost		_ether_ntohost
#define execl			_execl
#define execle			_execle
#define execlp			_execlp
#define execv			_execv
#define execvp			_execvp
#define fdopen			_fdopen
#define fgetln			_fgetln
#define fgetwln			_fgetwln
#define fhstatvfs		_fhstatvfs
#define flockfile		_flockfile
#define ftrylockfile		_ftrylockfile
#define funlockfile		_funlockfile
#define fnmatch			_fnmatch
#define fparseln		_fparseln
#define fpgetmask		_fpgetmask
#define fpgetround		_fpgetround
#define fpgetsticky		_fpgetsticky
#define fpsetmask		_fpsetmask
#define fpsetround		_fpsetround
#define fpsetsticky		_fpsetsticky
#define freenetconfigent	_freenetconfigent
#define freeaddrinfo		_freeaddrinfo
#define freeifaddrs		_freeifaddrs
#define fstatvfs		_fstatvfs
#define ftok			_ftok
#define ftruncate		_ftruncate
#define fts_children		_fts_children
#define fts_close		_fts_close
#define fts_open		_fts_open
#define fts_read		_fts_read
#define fts_set			_fts_set
#define gai_strerror		_gai_strerror
#define get_myaddress		_get_myaddress
#define getaddrinfo		_getaddrinfo
#define getbsize		_getbsize
#define getcwd			_getcwd
#define getdevmajor		_getdevmajor
#define getdiskbyname		_getdiskbyname
#define getdomainname		_getdomainname
#define getfsent		_getfsent
#define getfsfile		_getfsfile
#define getfsspec		_getfsspec
#define getgrent		_getgrent
#define getgrent_r		_getgrent_r
#define getgrgid		_getgrgid
#define getgrgid_r		_getgrgid_r
#define getgrnam		_getgrnam
#define getgrnam_r		_getgrnam_r
#define getgrouplist		_getgrouplist
#define getgroupmembership	_getgroupmembership
#define gethostbyaddr		_gethostbyaddr
#define gethostbyname		_gethostbyname
#define gethostent		_gethostent
#define gethostname		_gethostname
#define getifaddrs		_getifaddrs
#define getloadavg		_getloadavg
#define getlogin		_getlogin
#define getmntinfo		_getmntinfo
#define getmode			_getmode
#define getnameinfo		_getnameinfo
#define getnetbyaddr		_getnetbyaddr
#define getnetbyname		_getnetbyname
#define getnetconfig		_getnetconfig
#define getnetconfigent		_getnetconfigent
#define getnetent		_getnetent
#define getnetgrent		_getnetgrent
#define getnetpath		_getnetpath
#define getopt			_getopt
#define getopt_long		_getopt_long
#define getpagesize		_getpagesize
#define getpass			_getpass
#define getprogname		_getprogname
#define getprotobyname		_getprotobyname
#define getprotobyname_r	_getprotobyname_r
#define getprotobynumber	_getprotobynumber
#define getprotobynumber_r	_getprotobynumber_r
#define getprotoent		_getprotoent
#define getprotoent_r		_getprotoent_r
#define getpwent		_getpwent
#define getpwent_r		_getpwent_r
#define getpwnam		_getpwnam
#define getpwnam_r		_getpwnam_r
#define getpwuid		_getpwuid
#define getpwuid_r		_getpwuid_r
#define getrpcbyname		_getrpcbyname
#define getrpcbyname_r		_getrpcbyname_r
#define getrpcbynumber		_getrpcbynumber
#define getrpcbynumber_r	_getrpcbynumber_r
#define getrpcent		_getrpcent
#define getrpcent_r		_getrpcent_r
#define getrpcport		_getrpcport
#define getservbyname		_getservbyname
#define getservbyname_r		_getservbyname_r
#define getservbyport		_getservbyport
#define getservbyport_r		_getservbyport_r
#define getservent		_getservent
#define getservent_r		_getservent_r
#define getsubopt		_getsubopt
#define getttyent		_getttyent
#define getttynam		_getttynam
#define getusershell		_getusershell
#define glob			_glob
#define globfree		_globfree
#define gmtime_r		_gmtime_r
#define group_from_gid		_group_from_gid
#define heapsort		_heapsort
#define herror			_herror
#define hes_error		_hes_error
#define hes_free		_hes_free
#define hes_init		_hes_init
#define hes_resolve		_hes_resolve
#define hes_to_bind		_hes_to_bind
#define hesiod_end		_hesiod_end
#define hesiod_free_list	_hesiod_free_list
#define hesiod_init		_hesiod_init
#define hesiod_resolve		_hesiod_resolve
#define hesiod_to_bind		_hesiod_to_bind
#define iconv			_iconv
#define iconv_open		_iconv_open
#define iconv_close		_iconv_close
#define if_freenameindex	_if_freenameindex
#define if_indextoname		_if_indextoname
#define if_nameindex		_if_nameindex
#define if_nametoindex		_if_nametoindex
#define in6addr_any		_in6addr_any
#define in6addr_linklocal_allnodes	_in6addr_linklocal_allnodes
#define in6addr_linklocal_allrouters	_in6addr_linklocal_allrouters
#define in6addr_loopback	_in6addr_loopback
#define in6addr_nodelocal_allnodes	_in6addr_nodelocal_allnodes
#define inet6_option_alloc	_inet6_option_alloc
#define inet6_option_append	_inet6_option_append
#define inet6_option_find	_inet6_option_find
#define inet6_option_init	_inet6_option_init
#define inet6_option_next	_inet6_option_next
#define inet6_option_space	_inet6_option_space
#define inet6_rthdr_add		_inet6_rthdr_add
#define inet6_rthdr_getaddr	_inet6_rthdr_getaddr
#define inet6_rthdr_getflags	_inet6_rthdr_getflags
#define inet6_rthdr_init	_inet6_rthdr_init
#define inet6_rthdr_lasthop	_inet6_rthdr_lasthop
#define inet6_rthdr_segments	_inet6_rthdr_segments
#define inet6_rthdr_space	_inet6_rthdr_space
#define inet_cidr_ntop		_inet_cidr_ntop
#define inet_cidr_pton		_inet_cidr_pton
#define inet_lnaof		_inet_lnaof
#define inet_makeaddr		_inet_makeaddr
#define inet_net_ntop		_inet_net_ntop
#define inet_net_pton		_inet_net_pton
#define inet_neta		_inet_neta
#define inet_netof		_inet_netof
#define inet_network		_inet_network
#define inet_nsap_addr		_inet_nsap_addr
#define inet_nsap_ntoa		_inet_nsap_ntoa
#define inet_ntoa		_inet_ntoa
#define inet_ntop		_inet_ntop
#define initgroups		_initgroups
#define initstate		_initstate
#define innetgr			_innetgr
#define isatty			_isatty
#define jrand48			_jrand48
#define kill			_kill
#define l64a			_l64a
#define l64a_r			_l64a_r
#define lcong48			_lcong48
#define llabs			_llabs
#define lldiv			_lldiv
#define localtime_r		_localtime_r
#define lockf			_lockf
#define lrand48			_lrand48
#define lseek			_lseek
#define mergesort		_mergesort
#define mkstemp			_mkstemp
#define mmap			_mmap
#define mpool_close		_mpool_close
#define mpool_filter		_mpool_filter
#define mpool_get		_mpool_get
#define mpool_new		_mpool_new
#define mpool_open		_mpool_open
#define mpool_put		_mpool_put
#define mpool_sync		_mpool_sync
#define mrand48			_mrand48
#define nc_perror		_nc_perror
#define nc_sperror		_nc_sperror
#define nice			_nice
#if 0
#define nlist			_nlist
#endif
#define nrand48			_nrand48
#define ntp_adjtime		_ntp_adjtime
#define nsdispatch		_nsdispatch
#define offtime			_offtime
#define opendir			_opendir
#define openlog			_openlog
#define pause			_pause
#define pclose			_pclose
#define pmap_getmaps		_pmap_getmaps
#define pmap_getport		_pmap_getport
#define pmap_rmtcall		_pmap_rmtcall
#define pmap_set		_pmap_set
#define pmap_unset		_pmap_unset
#define popen			_popen
#define posix2time		_posix2time
#define pread			_pread
#define psignal			_psignal
#define pthread_atfork		_pthread_atfork
#define putenv			_putenv
#define pwcache_groupdb		_pwcache_groupdb
#define pwcache_userdb		_pwcache_userdb
#define pwrite			_pwrite
#define qabs			_qabs
#define qdiv			_qdiv
#define radixsort		_radixsort
#define random			_random
#define randomid		_randomid
#define randomid_new		_randomid_new
#define randomid_delete		_randomid_delete
#define readdir			_readdir
#define readdir_r		_readdir_r
#define realpath		_realpath
#define regcomp			_regcomp
#define regerror		_regerror
#define regexec			_regexec
#define regfree			_regfree
#define registerrpc		_registerrpc
#define res_init		_res_init
#define res_mkquery		_res_mkquery
#define res_query		_res_query
#define res_search		_res_search
#define rewinddir		_rewinddir
#define rpc_broadcast		_rpc_broadcast
#define rpc_broadcast_exp	_rpc_broadcast_exp
#define rpc_call		_rpc_call
#define rpc_control		_rpc_control
#define rpc_reg			_rpc_reg
#define rpcb_getmaps		_rpcb_getmaps
#define rpcb_gettime		_rpcb_gettime
#define rpcb_rmtcall		_rpcb_rmtcall
#define rpcb_set		_rpcb_set
#define rpcb_taddr2uaddr	_rpcb_taddr2uaddr
#define rpcb_uaddr2taddr	_rpcb_uaddr2taddr
#define rpcb_unset		_rpcb_unset
#define scandir			_scandir
#define seed48			_seed48
#define seekdir			_seekdir
#define send			_send
#define setdomainname		_setdomainname
#define setenv			_setenv
#define setfsent		_setfsent
#define setgrent		_setgrent
#define setgroupent		_setgroupent
#define sethostent		_sethostent
#define sethostname		_sethostname
#define setlogin		_setlogin
#define setlogmask		_setlogmask
#define setmode			_setmode
#define setnetconfig		_setnetconfig
#define setnetent		_setnetent
#define setnetgrent		_setnetgrent
#define setpassent		_setpassent
#define setnetpath		_setnetpath
#define setproctitle		_setproctitle
#define setprotoent		_setprotoent
#define setprotoent_r		_setprotoent_r
#define setpwent		_setpwent
#define setrpcent		_setrpcent
#define setservent		_setservent
#define setservent_r		_setservent_r
#define setstate		_setstate
#define setttyent		_setttyent
#define settimeofday		_settimeofday
#define setusershell		_setusershell
#define shm_open		_shm_open
#define shm_unlink		_shm_unlink
#define shquote			_shquote
#define siginterrupt		_siginterrupt
#define signal			_signal
#define sl_add			_sl_add
#define sl_create		_sl_create
#define sl_find			_sl_find
#define sl_free			_sl_free
#define sl_init			_sl_init
#define sleep			_sleep
#define snprintf		_snprintf
#define sradixsort		_sradixsort
#define srand48			_srand48
#define srandom			_srandom
#define statvfs(a, b)		_statvfs(a, b)
#define strcasecmp		_strcasecmp
#define strdup			_strdup
#define strncasecmp		_strncasecmp
#define strptime		_strptime
#define strsep			_strsep
#define strsignal		_strsignal
#define strsuftoll	 	_strsuftoll
#define strsuftollx	 	_strsuftollx
#define strsvis			_strsvis
#define strsvisx		_strsvisx
#define strtok_r		_strtok_r
#define strunvis		_strunvis
#define strvis			_strvis
#define strvisx			_strvisx
#define svc_auth_reg		_svc_auth_reg
#define svc_create		_svc_create
#define svc_dg_create		_svc_dg_create
#define svc_exit		_svc_exit
#define svc_fd_create		_svc_fd_create
#define svc_getreq		_svc_getreq
#define svc_getreqset		_svc_getreqset
#define svc_getreq_common	_svc_getreq_common
#define svc_raw_create		_svc_raw_create
#define svc_register		_svc_register
#define svc_reg			_svc_reg
#define svc_run			_svc_run
#define svc_sendreply		_svc_sendreply
#define svc_tli_create		_svc_tli_create
#define svc_tp_create		_svc_tp_create
#define svc_unregister		_svc_unregister
#define svc_unreg		_svc_unreg
#define svc_vc_create		_svc_vc_create
#define svcerr_auth		_svcerr_auth
#define svcerr_decode		_svcerr_decode
#define svcerr_noproc		_svcerr_noproc
#define svcerr_noprog		_svcerr_noprog
#define svcerr_progvers		_svcerr_progvers
#define svcerr_systemerr	_svcerr_systemerr
#define svcerr_weakauth		_svcerr_weakauth
#define svcfd_create		_svcfd_create
#define svcraw_create		_svcraw_create
#define svctcp_create		_svctcp_create
#define svcudp_bufcreate	_svcudp_bufcreate
#define svcudp_create		_svcudp_create
#define svcudp_enablecache	_svcudp_enablecache
#define svis			_svis
#define sysarch			_sys_sysarch
#define sysctl			_sysctl
#define sysctlbyname		_sysctlbyname
#define sysctlgetmibinfo	_sysctlgetmibinfo
#define sysctlnametomib		_sysctlnametomib
#define syslog			_syslog
#define taddr2uaddr		_taddr2uaddr
#define tcdrain			_tcdrain
#define tcflow			_tcflow
#define tcflush			_tcflush
#define tcgetattr		_tcgetattr
#define tcgetpgrp		_tcgetpgrp
#define tcgetsid		_tcgetsid
#define tcsendbreak		_tcsendbreak
#define tcsetattr		_tcsetattr
#define tcsetpgrp		_tcsetpgrp
#define telldir			_telldir
#define time			_time
#define time2posix		_time2posix
#define timegm			_timegm
#define timelocal		_timelocal
#define timeoff			_timeoff
#define times			_times
#define ttyname			_ttyname
#define ttyname_r		_ttyname_r
#define ttyslot			_ttyslot
#define tzname			_tzname
#define tzset			_tzset
#define tzsetwall		_tzsetwall
#define uaddr2taddr		_uaddr2taddr
#define ualarm			_ualarm
#define uname			_uname
#define unsetenv		_unsetenv
#define unvis			_unvis
#define user_from_uid		_user_from_uid
#define usleep			_usleep
#define utime			_utime
#define uuid_create_nil		_uuid_create_nil
#define uuid_is_nil		_uuid_is_nil
#define valloc			_valloc
#define vis			_vis
#define vsnprintf		_vsnprintf
#define vsyslog			_vsyslog
#define wait			_wait
#define wait3			_wait3
#define waitpid			_waitpid
#define wcstof			_wcstof
#define wcstod			_wcstod
#define wcstold			_wcstold
#define wcwidth			_wcwidth
#define xdr_accepted_reply	_xdr_accepted_reply
#define xdr_array		_xdr_array
#define xdr_authunix_parms	_xdr_authunix_parms
#define xdr_bool		_xdr_bool
#define xdr_bytes		_xdr_bytes
#define xdr_callhdr		_xdr_callhdr
#define xdr_callmsg		_xdr_callmsg
#define xdr_char		_xdr_char
#define xdr_datum		_xdr_datum
#define xdr_des_block		_xdr_des_block
#define xdr_domainname		_xdr_domainname
#define xdr_double		_xdr_double
#define xdr_enum		_xdr_enum
#define xdr_float		_xdr_float
#define xdr_free		_xdr_free
#define	xdr_hyper		_xdr_hyper
#define xdr_int			_xdr_int
#define xdr_int16_t		_xdr_int16_t
#define xdr_int32_t		_xdr_int32_t
#define xdr_int64_t		_xdr_int64_t
#define xdr_long		_xdr_long
#define	xdr_longlong_t		_xdr_longlong_t
#define xdr_mapname		_xdr_mapname
#define xdr_netbuf		_xdr_netbuf
#define xdr_netobj		_xdr_netobj
#define xdr_opaque		_xdr_opaque
#define xdr_opaque_auth		_xdr_opaque_auth
#define xdr_peername		_xdr_peername
#define xdr_pmap		_xdr_pmap
#define xdr_pmaplist		_xdr_pmaplist
#define xdr_pointer		_xdr_pointer
#define xdr_reference		_xdr_reference
#define xdr_rejected_reply	_xdr_rejected_reply
#define xdr_replymsg		_xdr_replymsg
#define xdr_rmtcall_args	_xdr_rmtcall_args
#define xdr_rmtcallres		_xdr_rmtcallres
#define xdr_rpcb		_xdr_rpcb
#define xdr_rpcb_entry		_xdr_rpcb_entry
#define xdr_rpcb_entry_list_ptr	_xdr_rpcb_entry_list_ptr
#define xdr_rpcb_rmtcallargs	_xdr_rpcb_rmtcallargs
#define xdr_rpcb_rmtcallres	_xdr_rpcb_rmtcallres
#define xdr_rpcb_stat		_xdr_rpcb_stat
#define xdr_rpcb_stat_byvers	_xdr_rpcb_stat_byvers
#define xdr_rpcblist		_xdr_rpcblist
#define xdr_rpcblist_ptr	_xdr_rpcblist_ptr
#define xdr_rpcbs_addrlist	_xdr_rpcbs_addrlist
#define xdr_rpcbs_addrlist_ptr	_xdr_rpcbs_addrlist_ptr
#define xdr_rpcbs_proc		_xdr_rpcbs_proc
#define xdr_rpcbs_rmtcalllist	_xdr_rpcbs_rmtcalllist
#define xdr_rpcbs_rmtcalllist_ptr	_xdr_rpcbs_rmtcalllist_ptr
#define xdr_rpcbs		_xdr_rpcbs
#define xdr_rpcbs		_xdr_rpcbs
#define xdr_short		_xdr_short
#define xdr_string		_xdr_string
#define xdr_u_char		_xdr_u_char
#define	xdr_u_hyper		_xdr_u_hyper
#define xdr_u_int		_xdr_u_int
#define xdr_u_int16_t		_xdr_u_int16_t
#define xdr_u_int32_t		_xdr_u_int32_t
#define xdr_u_int64_t		_xdr_u_int64_t
#define xdr_u_long		_xdr_u_long
#define	xdr_u_longlong_t	_xdr_u_longlong_t
#define xdr_u_short		_xdr_u_short
#define xdr_union		_xdr_union
#define xdr_vector		_xdr_vector
#define xdr_void		_xdr_void
#define xdr_wrapstring		_xdr_wrapstring
#define xdr_yp_inaddr		_xdr_yp_inaddr
#define xdr_ypall		_xdr_ypall
#define xdr_ypbind_resp		_xdr_ypbind_resp
#define xdr_ypbind_setdom	_xdr_ypbind_setdom
#define xdr_ypdomain_wrap_string	_xdr_ypdomain_wrap_string
#define xdr_ypmap_parms		_xdr_ypmap_parms
#define xdr_ypmap_wrap_string	_xdr_ypmap_wrap_string
#define xdr_ypmaplist		_xdr_ypmaplist
#define xdr_ypowner_wrap_string _xdr_ypowner_wrap_string
#define xdr_yppushresp_xfr	_xdr_yppushresp_xfr
#define xdr_ypreq_key		_xdr_ypreq_key
#define xdr_ypreq_nokey		_xdr_ypreq_nokey
#define xdr_ypreq_xfr		_xdr_ypreq_xfr
#define xdr_ypresp_key_val	_xdr_ypresp_key_val
#define xdr_ypresp_maplist	_xdr_ypresp_maplist
#define xdr_ypresp_master	_xdr_ypresp_master
#define xdr_ypresp_order	_xdr_ypresp_order
#define xdr_ypresp_val		_xdr_ypresp_val
#define xdrmem_create		_xdrmem_create
#define xdrrec_create		_xdrrec_create
#define xdrrec_endofrecord	_xdrrec_endofrecord
#define xdrrec_eof		_xdrrec_eof
#define xdrrec_skiprecord	_xdrrec_skiprecord
#define xdrstdio_create		_xdrstdio_create
#define xprt_register		_xprt_register
#define xprt_unregister		_xprt_unregister
#define yp_all			_yp_all
#define yp_bind			_yp_bind
#define yp_first		_yp_first
#define yp_get_default_domain	_yp_get_default_domain
#define yp_maplist		_yp_maplist
#define yp_master		_yp_master
#define yp_match		_yp_match
#define yp_next			_yp_next
#define yp_order		_yp_order
#define yp_unbind		_yp_unbind
#define yperr_string		_yperr_string
#define ypprot_err		_ypprot_err
#define dlopen			__dlopen
#define dlclose			__dlclose
#define dlsym			__dlsym
#define dlerror			__dlerror
#define dladdr			__dladdr
#define fmtcheck		__fmtcheck

/* rpc locks */
#define authdes_lock		__rpc_authdes_lock
#define authnone_lock		__rpc_authnone_lock
#define authsvc_lock		__rpc_authsvc_lock
#define clnt_fd_lock		__rpc_clnt_fd_lock
#define clntraw_lock		__rpc_clntraw_lock
#define dname_lock		__rpc_dname_lock
#define dupreq_lock		__rpc_dupreq_lock
#define keyserv_lock		__rpc_keyserv_lock
#define libnsl_trace_lock	__rpc_libnsl_trace_lock
#define loopnconf_lock		__rpc_loopnconf_lock
#define ops_lock		__rpc_ops_lock
#define portnum_lock		__rpc_portnum_lock
#define proglst_lock		__rpc_proglst_lock
#define rpcbaddr_cache_lock	__rpc_rpcbaddr_cache_lock
#define rpcsoc_lock		__rpc_rpcsoc_lock
#define svc_fd_lock		__rpc_svc_fd_lock
#define svc_lock		__rpc_svc_lock
#define svcraw_lock		__rpc_svcraw_lock
#define xprtlist_lock		__rpc_xprtlist_lock

#define __learn_tree		___learn_tree
#endif /* __weak_alias */
#endif /* !__lint__ */

#endif /* _NAMESPACE_H_ */
