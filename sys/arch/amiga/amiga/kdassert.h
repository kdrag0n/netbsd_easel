/*	$NetBSD: kdassert.h,v 1.2 1994/10/26 02:01:54 cgd Exp $	*/

#if ! defined (_KDASSERT_H)
#define _KDASSERT_H
#if defined (DEBUG)
#if defined (__STDC__)
#if defined (__GNUC__)
#define KDASSERT(x) if (!(x)) panic ("kernel assertion:\"%s\" failed\nfile: %s\n" \
	"func: %s\nline: %d\n", #x , __FILE__, __FUNCTION__, __LINE__)
#else /* !__GNUC__ */
#define KDASSERT(x) if (!(x)) panic ("kernel assertion:\"%s\" failed\nfile: %s\n" \
	"line: %d\n", #x, __FILE__, __LINE__)
#endif /* !__GNUC__ */
#else /* !__STDC__ */
#if defined (__GNUC__)
#define KDASSERT(x) if (!(x)) panic ("kernel assertion:\"%s\" failed\nfile: %s\n" \
	"func: %s\nline: %d\n", "x", __FILE__, __FUNCTION__, __LINE__)
#else /* !__GNUC__ */
#define KDASSERT(x) if (!(x)) panic ("kernel assertion:\"%s\" failed\nfile: %s\n" \
	"line: %d\n", "x", __FILE__, __LINE__)
#endif /* !__GNUC__ */
#endif /* !__STDC__ */
#else /* !DEBUG */
#define KDASSERT(x)
#endif /* !DEBUG */
#endif /* _KDASSERT_H
