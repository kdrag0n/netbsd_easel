dnl $Id: broken.m4,v 1.1.1.3 2001/09/17 12:10:06 assar Exp $
dnl
dnl
dnl Same as AC _REPLACE_FUNCS, just define HAVE_func if found in normal
dnl libraries 

AC_DEFUN(AC_BROKEN,
[for ac_func in $1
do
AC_CHECK_FUNC($ac_func, [
ac_tr_func=HAVE_[]upcase($ac_func)
AC_DEFINE_UNQUOTED($ac_tr_func)],[LIBOBJS[]="$LIBOBJS ${ac_func}.o"])
if false; then
	AC_CHECK_FUNCS($1)
fi
done
AC_SUBST(LIBOBJS)dnl
])
