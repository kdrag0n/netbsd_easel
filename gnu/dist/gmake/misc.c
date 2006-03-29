/* Miscellaneous generic support functions for GNU Make.
Copyright (C) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1997,
2002 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "make.h"
#include "dep.h"
#include "debug.h"

/* Variadic functions.  We go through contortions to allow proper function
   prototypes for both ANSI and pre-ANSI C compilers, and also for those
   which support stdarg.h vs. varargs.h, and finally those which have
   vfprintf(), etc. and those who have _doprnt... or nothing.

   This fancy stuff all came from GNU fileutils, except for the VA_PRINTF and
   VA_END macros used here since we have multiple print functions.  */

#if HAVE_VPRINTF || HAVE_DOPRNT
# define HAVE_STDVARARGS 1
# if __STDC__
#  include <stdarg.h>
#  define VA_START(args, lastarg) va_start(args, lastarg)
# else
#  include <varargs.h>
#  define VA_START(args, lastarg) va_start(args)
# endif
# if HAVE_VPRINTF
#  define VA_PRINTF(fp, lastarg, args) vfprintf((fp), (lastarg), (args))
# else
#  define VA_PRINTF(fp, lastarg, args) _doprnt((lastarg), (args), (fp))
# endif
# define VA_END(args) va_end(args)
#else
/* # undef HAVE_STDVARARGS */
# define va_alist a1, a2, a3, a4, a5, a6, a7, a8
# define va_dcl char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
# define VA_START(args, lastarg)
# define VA_PRINTF(fp, lastarg, args) fprintf((fp), (lastarg), va_alist)
# define VA_END(args)
#endif


/* Compare strings *S1 and *S2.
   Return negative if the first is less, positive if it is greater,
   zero if they are equal.  */

int
alpha_compare (v1, v2)
     const void *v1, *v2;
{
  const char *s1 = *((char **)v1);
  const char *s2 = *((char **)v2);

  if (*s1 != *s2)
    return *s1 - *s2;
  return strcmp (s1, s2);
}

/* Discard each backslash-newline combination from LINE.
   Backslash-backslash-newline combinations become backslash-newlines.
   This is done by copying the text at LINE into itself.  */

void
collapse_continuations (line)
     char *line;
{
  register char *in, *out, *p;
  register int backslash;
  register unsigned int bs_write;

  in = strchr (line, '\n');
  if (in == 0)
    return;

  out = in;
  while (out > line && out[-1] == '\\')
    --out;

  while (*in != '\0')
    {
      /* BS_WRITE gets the number of quoted backslashes at
	 the end just before IN, and BACKSLASH gets nonzero
	 if the next character is quoted.  */
      backslash = 0;
      bs_write = 0;
      for (p = in - 1; p >= line && *p == '\\'; --p)
	{
	  if (backslash)
	    ++bs_write;
	  backslash = !backslash;

	  /* It should be impossible to go back this far without exiting,
	     but if we do, we can't get the right answer.  */
	  if (in == out - 1)
	    abort ();
	}

      /* Output the appropriate number of backslashes.  */
      while (bs_write-- > 0)
	*out++ = '\\';

      /* Skip the newline.  */
      ++in;

      /* If the newline is quoted, discard following whitespace
	 and any preceding whitespace; leave just one space.  */
      if (backslash)
	{
	  in = next_token (in);
	  while (out > line && isblank ((unsigned char)out[-1]))
	    --out;
	  *out++ = ' ';
	}
      else
	/* If the newline isn't quoted, put it in the output.  */
	*out++ = '\n';

      /* Now copy the following line to the output.
	 Stop when we find backslashes followed by a newline.  */
      while (*in != '\0')
	if (*in == '\\')
	  {
	    p = in + 1;
	    while (*p == '\\')
	      ++p;
	    if (*p == '\n')
	      {
		in = p;
		break;
	      }
	    while (in < p)
	      *out++ = *in++;
	  }
	else
	  *out++ = *in++;
    }

  *out = '\0';
}


/* Remove comments from LINE.
   This is done by copying the text at LINE onto itself.  */

void
remove_comments (line)
     char *line;
{
  char *comment;

  comment = find_char_unquote (line, '#', 0, 0);

  if (comment != 0)
    /* Cut off the line at the #.  */
    *comment = '\0';
}

/* Print N spaces (used in debug for target-depth).  */

void
print_spaces (n)
     unsigned int n;
{
  while (n-- > 0)
    putchar (' ');
}


/* Return a newly-allocated string whose contents
   concatenate those of s1, s2, s3.  */

char *
concat (s1, s2, s3)
     const char *s1, *s2, *s3;
{
  unsigned int len1, len2, len3;
  char *result;

  len1 = *s1 != '\0' ? strlen (s1) : 0;
  len2 = *s2 != '\0' ? strlen (s2) : 0;
  len3 = *s3 != '\0' ? strlen (s3) : 0;

  result = (char *) xmalloc (len1 + len2 + len3 + 1);

  if (*s1 != '\0')
    bcopy (s1, result, len1);
  if (*s2 != '\0')
    bcopy (s2, result + len1, len2);
  if (*s3 != '\0')
    bcopy (s3, result + len1 + len2, len3);
  *(result + len1 + len2 + len3) = '\0';

  return result;
}

/* Print a message on stdout.  */

void
#if __STDC__ && HAVE_STDVARARGS
message (int prefix, const char *fmt, ...)
#else
message (prefix, fmt, va_alist)
     int prefix;
     const char *fmt;
     va_dcl
#endif
{
#if HAVE_STDVARARGS
  va_list args;
#endif

  log_working_directory (1);

  if (fmt != 0)
    {
      if (prefix)
	{
	  if (makelevel == 0)
	    printf ("%s: ", program);
	  else
	    printf ("%s[%u]: ", program, makelevel);
	}
      VA_START (args, fmt);
      VA_PRINTF (stdout, fmt, args);
      VA_END (args);
      putchar ('\n');
    }

  fflush (stdout);
}

/* Print an error message.  */

void
#if __STDC__ && HAVE_STDVARARGS
error (const struct floc *flocp, const char *fmt, ...)
#else
error (flocp, fmt, va_alist)
     const struct floc *flocp;
     const char *fmt;
     va_dcl
#endif
{
#if HAVE_STDVARARGS
  va_list args;
#endif

  log_working_directory (1);

  if (flocp && flocp->filenm)
    fprintf (stderr, "%s:%lu: ", flocp->filenm, flocp->lineno);
  else if (makelevel == 0)
    fprintf (stderr, "%s: ", program);
  else
    fprintf (stderr, "%s[%u]: ", program, makelevel);

  VA_START(args, fmt);
  VA_PRINTF (stderr, fmt, args);
  VA_END (args);

  putc ('\n', stderr);
  fflush (stderr);
}

/* Print an error message and exit.  */

void
#if __STDC__ && HAVE_STDVARARGS
fatal (const struct floc *flocp, const char *fmt, ...)
#else
fatal (flocp, fmt, va_alist)
     const struct floc *flocp;
     const char *fmt;
     va_dcl
#endif
{
#if HAVE_STDVARARGS
  va_list args;
#endif

  log_working_directory (1);

  if (flocp && flocp->filenm)
    fprintf (stderr, "%s:%lu: *** ", flocp->filenm, flocp->lineno);
  else if (makelevel == 0)
    fprintf (stderr, "%s: *** ", program);
  else
    fprintf (stderr, "%s[%u]: *** ", program, makelevel);

  VA_START(args, fmt);
  VA_PRINTF (stderr, fmt, args);
  VA_END (args);

  fputs (_(".  Stop.\n"), stderr);

  die (2);
}

#ifndef HAVE_STRERROR

#undef	strerror

char *
strerror (errnum)
     int errnum;
{
  extern int errno, sys_nerr;
#ifndef __DECC
  extern char *sys_errlist[];
#endif
  static char buf[] = "Unknown error 12345678901234567890";

  if (errno < sys_nerr)
    return sys_errlist[errnum];

  sprintf (buf, _("Unknown error %d"), errnum);
  return buf;
}
#endif

/* Print an error message from errno.  */

void
perror_with_name (str, name)
     const char *str, *name;
{
  error (NILF, _("%s%s: %s"), str, name, strerror (errno));
}

/* Print an error message from errno and exit.  */

void
pfatal_with_name (name)
     const char *name;
{
  fatal (NILF, _("%s: %s"), name, strerror (errno));

  /* NOTREACHED */
}

/* Like malloc but get fatal error if memory is exhausted.  */
/* Don't bother if we're using dmalloc; it provides these for us.  */

#ifndef HAVE_DMALLOC_H

#undef xmalloc
#undef xrealloc
#undef xstrdup

char *
xmalloc (size)
     unsigned int size;
{
  char *result = (char *) malloc (size);
  if (result == 0)
    fatal (NILF, _("virtual memory exhausted"));
  return result;
}


char *
xrealloc (ptr, size)
     char *ptr;
     unsigned int size;
{
  char *result;

  /* Some older implementations of realloc() don't conform to ANSI.  */
  result = ptr ? realloc (ptr, size) : malloc (size);
  if (result == 0)
    fatal (NILF, _("virtual memory exhausted"));
  return result;
}


char *
xstrdup (ptr)
     const char *ptr;
{
  char *result;

#ifdef HAVE_STRDUP
  result = strdup (ptr);
#else
  result = (char *) malloc (strlen (ptr) + 1);
#endif

  if (result == 0)
    fatal (NILF, _("virtual memory exhausted"));

#ifdef HAVE_STRDUP
  return result;
#else
  return strcpy(result, ptr);
#endif
}

#endif  /* HAVE_DMALLOC_H */

char *
savestring (str, length)
     const char *str;
     unsigned int length;
{
  register char *out = (char *) xmalloc (length + 1);
  if (length > 0)
    bcopy (str, out, length);
  out[length] = '\0';
  return out;
}

/* Search string BIG (length BLEN) for an occurrence of
   string SMALL (length SLEN).  Return a pointer to the
   beginning of the first occurrence, or return nil if none found.  */

char *
sindex (big, blen, small, slen)
     const char *big;
     unsigned int blen;
     const char *small;
     unsigned int slen;
{
  if (!blen)
    blen = strlen (big);
  if (!slen)
    slen = strlen (small);

  if (slen && blen >= slen)
    {
      register unsigned int b;

      /* Quit when there's not enough room left for the small string.  */
      --slen;
      blen -= slen;

      for (b = 0; b < blen; ++b, ++big)
        if (*big == *small && strneq (big + 1, small + 1, slen))
          return (char *)big;
    }

  return 0;
}

/* Limited INDEX:
   Search through the string STRING, which ends at LIMIT, for the character C.
   Returns a pointer to the first occurrence, or nil if none is found.
   Like INDEX except that the string searched ends where specified
   instead of at the first null.  */

char *
lindex (s, limit, c)
     register const char *s, *limit;
     int c;
{
  while (s < limit)
    if (*s++ == c)
      return (char *)(s - 1);

  return 0;
}

/* Return the address of the first whitespace or null in the string S.  */

char *
end_of_token (s)
     char *s;
{
  while (*s != '\0' && !isblank ((unsigned char)*s))
    ++s;
  return s;
}

#ifdef WINDOWS32
/*
 * Same as end_of_token, but take into account a stop character
 */
char *
end_of_token_w32 (s, stopchar)
     char *s;
     char stopchar;
{
  register char *p = s;
  register int backslash = 0;

  while (*p != '\0' && *p != stopchar
	 && (backslash || !isblank ((unsigned char)*p)))
    {
      if (*p++ == '\\')
        {
          backslash = !backslash;
          while (*p == '\\')
            {
              backslash = !backslash;
              ++p;
            }
        }
      else
        backslash = 0;
    }

  return p;
}
#endif

/* Return the address of the first nonwhitespace or null in the string S.  */

char *
next_token (s)
     const char *s;
{
  while (isblank ((unsigned char)*s))
    ++s;
  return (char *)s;
}

/* Find the next token in PTR; return the address of it, and store the
   length of the token into *LENGTHPTR if LENGTHPTR is not nil.  */

char *
find_next_token (ptr, lengthptr)
     char **ptr;
     unsigned int *lengthptr;
{
  char *p = next_token (*ptr);
  char *end;

  if (*p == '\0')
    return 0;

  *ptr = end = end_of_token (p);
  if (lengthptr != 0)
    *lengthptr = end - p;
  return p;
}

/* Copy a chain of `struct dep', making a new chain
   with the same contents as the old one.  */

struct dep *
copy_dep_chain (d)
     register struct dep *d;
{
  register struct dep *c;
  struct dep *firstnew = 0;
  struct dep *lastnew = 0;

  while (d != 0)
    {
      c = (struct dep *) xmalloc (sizeof (struct dep));
      bcopy ((char *) d, (char *) c, sizeof (struct dep));
      if (c->name != 0)
	c->name = xstrdup (c->name);
      c->next = 0;
      if (firstnew == 0)
	firstnew = lastnew = c;
      else
	lastnew = lastnew->next = c;

      d = d->next;
    }

  return firstnew;
}

#ifdef	iAPX286
/* The losing compiler on this machine can't handle this macro.  */

char *
dep_name (dep)
     struct dep *dep;
{
  return dep->name == 0 ? dep->file->name : dep->name;
}
#endif

#ifdef	GETLOADAVG_PRIVILEGED

#ifdef POSIX

/* Hopefully if a system says it's POSIX.1 and has the setuid and setgid
   functions, they work as POSIX.1 says.  Some systems (Alpha OSF/1 1.2,
   for example) which claim to be POSIX.1 also have the BSD setreuid and
   setregid functions, but they don't work as in BSD and only the POSIX.1
   way works.  */

#undef HAVE_SETREUID
#undef HAVE_SETREGID

#else	/* Not POSIX.  */

/* Some POSIX.1 systems have the seteuid and setegid functions.  In a
   POSIX-like system, they are the best thing to use.  However, some
   non-POSIX systems have them too but they do not work in the POSIX style
   and we must use setreuid and setregid instead.  */

#undef HAVE_SETEUID
#undef HAVE_SETEGID

#endif	/* POSIX.  */

#ifndef	HAVE_UNISTD_H
extern int getuid (), getgid (), geteuid (), getegid ();
extern int setuid (), setgid ();
#ifdef HAVE_SETEUID
extern int seteuid ();
#else
#ifdef	HAVE_SETREUID
extern int setreuid ();
#endif	/* Have setreuid.  */
#endif	/* Have seteuid.  */
#ifdef HAVE_SETEGID
extern int setegid ();
#else
#ifdef	HAVE_SETREGID
extern int setregid ();
#endif	/* Have setregid.  */
#endif	/* Have setegid.  */
#endif	/* No <unistd.h>.  */

/* Keep track of the user and group IDs for user- and make- access.  */
static int user_uid = -1, user_gid = -1, make_uid = -1, make_gid = -1;
#define	access_inited	(user_uid != -1)
static enum { make, user } current_access;


/* Under -d, write a message describing the current IDs.  */

static void
log_access (flavor)
     char *flavor;
{
  if (! ISDB (DB_JOBS))
    return;

  /* All the other debugging messages go to stdout,
     but we write this one to stderr because it might be
     run in a child fork whose stdout is piped.  */

  fprintf (stderr, _("%s: user %lu (real %lu), group %lu (real %lu)\n"),
	   flavor, (unsigned long) geteuid (), (unsigned long) getuid (),
           (unsigned long) getegid (), (unsigned long) getgid ());
  fflush (stderr);
}


static void
init_access ()
{
#ifndef VMS
  user_uid = getuid ();
  user_gid = getgid ();

  make_uid = geteuid ();
  make_gid = getegid ();

  /* Do these ever fail?  */
  if (user_uid == -1 || user_gid == -1 || make_uid == -1 || make_gid == -1)
    pfatal_with_name ("get{e}[gu]id");

  log_access (_("Initialized access"));

  current_access = make;
#endif
}

#endif	/* GETLOADAVG_PRIVILEGED */

/* Give the process appropriate permissions for access to
   user data (i.e., to stat files, or to spawn a child process).  */
void
user_access ()
{
#ifdef	GETLOADAVG_PRIVILEGED

  if (!access_inited)
    init_access ();

  if (current_access == user)
    return;

  /* We are in "make access" mode.  This means that the effective user and
     group IDs are those of make (if it was installed setuid or setgid).
     We now want to set the effective user and group IDs to the real IDs,
     which are the IDs of the process that exec'd make.  */

#ifdef	HAVE_SETEUID

  /* Modern systems have the seteuid/setegid calls which set only the
     effective IDs, which is ideal.  */

  if (seteuid (user_uid) < 0)
    pfatal_with_name ("user_access: seteuid");

#else	/* Not HAVE_SETEUID.  */

#ifndef	HAVE_SETREUID

  /* System V has only the setuid/setgid calls to set user/group IDs.
     There is an effective ID, which can be set by setuid/setgid.
     It can be set (unless you are root) only to either what it already is
     (returned by geteuid/getegid, now in make_uid/make_gid),
     the real ID (return by getuid/getgid, now in user_uid/user_gid),
     or the saved set ID (what the effective ID was before this set-ID
     executable (make) was exec'd).  */

  if (setuid (user_uid) < 0)
    pfatal_with_name ("user_access: setuid");

#else	/* HAVE_SETREUID.  */

  /* In 4BSD, the setreuid/setregid calls set both the real and effective IDs.
     They may be set to themselves or each other.  So you have two alternatives
     at any one time.  If you use setuid/setgid, the effective will be set to
     the real, leaving only one alternative.  Using setreuid/setregid, however,
     you can toggle between your two alternatives by swapping the values in a
     single setreuid or setregid call.  */

  if (setreuid (make_uid, user_uid) < 0)
    pfatal_with_name ("user_access: setreuid");

#endif	/* Not HAVE_SETREUID.  */
#endif	/* HAVE_SETEUID.  */

#ifdef	HAVE_SETEGID
  if (setegid (user_gid) < 0)
    pfatal_with_name ("user_access: setegid");
#else
#ifndef	HAVE_SETREGID
  if (setgid (user_gid) < 0)
    pfatal_with_name ("user_access: setgid");
#else
  if (setregid (make_gid, user_gid) < 0)
    pfatal_with_name ("user_access: setregid");
#endif
#endif

  current_access = user;

  log_access (_("User access"));

#endif	/* GETLOADAVG_PRIVILEGED */
}

/* Give the process appropriate permissions for access to
   make data (i.e., the load average).  */
void
make_access ()
{
#ifdef	GETLOADAVG_PRIVILEGED

  if (!access_inited)
    init_access ();

  if (current_access == make)
    return;

  /* See comments in user_access, above.  */

#ifdef	HAVE_SETEUID
  if (seteuid (make_uid) < 0)
    pfatal_with_name ("make_access: seteuid");
#else
#ifndef	HAVE_SETREUID
  if (setuid (make_uid) < 0)
    pfatal_with_name ("make_access: setuid");
#else
  if (setreuid (user_uid, make_uid) < 0)
    pfatal_with_name ("make_access: setreuid");
#endif
#endif

#ifdef	HAVE_SETEGID
  if (setegid (make_gid) < 0)
    pfatal_with_name ("make_access: setegid");
#else
#ifndef	HAVE_SETREGID
  if (setgid (make_gid) < 0)
    pfatal_with_name ("make_access: setgid");
#else
  if (setregid (user_gid, make_gid) < 0)
    pfatal_with_name ("make_access: setregid");
#endif
#endif

  current_access = make;

  log_access (_("Make access"));

#endif	/* GETLOADAVG_PRIVILEGED */
}

/* Give the process appropriate permissions for a child process.
   This is like user_access, but you can't get back to make_access.  */
void
child_access ()
{
#ifdef	GETLOADAVG_PRIVILEGED

  if (!access_inited)
    abort ();

  /* Set both the real and effective UID and GID to the user's.
     They cannot be changed back to make's.  */

#ifndef	HAVE_SETREUID
  if (setuid (user_uid) < 0)
    pfatal_with_name ("child_access: setuid");
#else
  if (setreuid (user_uid, user_uid) < 0)
    pfatal_with_name ("child_access: setreuid");
#endif

#ifndef	HAVE_SETREGID
  if (setgid (user_gid) < 0)
    pfatal_with_name ("child_access: setgid");
#else
  if (setregid (user_gid, user_gid) < 0)
    pfatal_with_name ("child_access: setregid");
#endif

  log_access (_("Child access"));

#endif	/* GETLOADAVG_PRIVILEGED */
}

#ifdef NEED_GET_PATH_MAX
unsigned int
get_path_max ()
{
  static unsigned int value;

  if (value == 0)
    {
      long int x = pathconf ("/", _PC_PATH_MAX);
      if (x > 0)
	value = x;
      else
	return MAXPATHLEN;
    }

  return value;
}
#endif


#ifdef HAVE_BROKEN_RESTART

#undef stat
#undef readdir

int
atomic_stat(file, buf)
     const char *file;
     struct stat *buf;
{
  int r;

  while ((r = stat (file, buf)) < 0)
    if (errno != EINTR)
      break;

  return r;
}

struct dirent *
atomic_readdir(dir)
     DIR *dir;
{
  struct dirent *r;

  while ((r = readdir (dir)) == NULL)
    if (errno != EINTR)
      break;

  return r;
}

#endif  /* HAVE_BROKEN_RESTART */
