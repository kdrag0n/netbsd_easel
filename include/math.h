/*
 * Copyright (c) 1985, 1990 The Regents of the University of California.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)math.h	5.8 (Berkeley) 4/2/91
 *	$Id: math.h,v 1.4 1993/08/04 17:24:44 jtc Exp $
 */

#ifndef	_MATH_H_
#define	_MATH_H_

#if defined(vax) || defined(tahoe)	/* DBL_MAX from float.h */
#define	HUGE_VAL	1.701411834604692294E+38
#else
extern char __infinity[];		/* bytes for IEEE754 +Infinity */
#define HUGE_VAL (*(double *) __infinity)
#endif

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
#if defined(vax) || defined(tahoe)
/*
 * HUGE for the VAX and Tahoe converts to the largest possible F-float value.
 * This implies an understanding of the conversion behavior of atof(3).  It
 * was defined to be the largest float so that overflow didn't occur when it
 * was assigned to a single precision number.  HUGE_VAL is strongly preferred.
 */
#define	HUGE	1.701411733192644270E+38		
#else
#define	HUGE	HUGE_VAL
#endif
#endif

#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#include <sys/cdefs.h>

__BEGIN_DECLS
__pure double	acos __P((double));
__pure double	asin __P((double));
__pure double	atan __P((double));
__pure double	atan2 __P((double, double));
__pure double	ceil __P((double));
__pure double	cos __P((double));
__pure double	cosh __P((double));
__pure double	exp __P((double));
__pure double	fabs __P((double));
__pure double	floor __P((double));
__pure double	fmod __P((double, double));
double	frexp __P((double, int *));
__pure double	ldexp __P((double, int));
__pure double	log __P((double));
__pure double	log10 __P((double));
double	modf __P((double, double *));
__pure double	pow __P((double, double));
__pure double	sin __P((double));
__pure double	sinh __P((double));
__pure double	sqrt __P((double));
__pure double	tan __P((double));
__pure double	tanh __P((double));

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
__pure double	acosh __P((double));
__pure double	asinh __P((double));
__pure double	atanh __P((double));
__pure double	cabs();		/* we can't describe cabs()'s argument properly */
__pure double	cbrt __P((double));
__pure double	copysign __P((double, double));
__pure double	drem __P((double, double));
__pure double	erf __P((double));
__pure double	erfc __P((double));
__pure double	expm1 __P((double));
__pure int	finite __P((double));
double	hypot __P((double, double));
#if defined(vax) || defined(tahoe)
__pure double	infnan __P((int));
#endif
__pure int	isinf __P((double));
__pure int	isnan __P((double));
__pure double	j0 __P((double));
__pure double	j1 __P((double));
__pure double	jn __P((int, double));
__pure double	lgamma __P((double));
__pure double	log1p __P((double));
__pure double	logb __P((double));
__pure double	rint __P((double));
__pure double	scalb __P((double, int));
__pure double	y0 __P((double));
__pure double	y1 __P((double));
__pure double	yn __P((int, double));
#endif

__END_DECLS

#endif /* _MATH_H_ */
