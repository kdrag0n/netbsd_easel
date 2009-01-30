/*	$NetBSD: cond.c,v 1.58 2009/01/30 22:35:10 dsl Exp $	*/

/*
 * Copyright (c) 1988, 1989, 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Adam de Boor.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 */

/*
 * Copyright (c) 1988, 1989 by Adam de Boor
 * Copyright (c) 1989 by Berkeley Softworks
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Adam de Boor.
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
 */

#ifndef MAKE_NATIVE
static char rcsid[] = "$NetBSD: cond.c,v 1.58 2009/01/30 22:35:10 dsl Exp $";
#else
#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)cond.c	8.2 (Berkeley) 1/2/94";
#else
__RCSID("$NetBSD: cond.c,v 1.58 2009/01/30 22:35:10 dsl Exp $");
#endif
#endif /* not lint */
#endif

/*-
 * cond.c --
 *	Functions to handle conditionals in a makefile.
 *
 * Interface:
 *	Cond_Eval 	Evaluate the conditional in the passed line.
 *
 */

#include    <ctype.h>
#include    <errno.h>    /* For strtoul() error checking */

#include    "make.h"
#include    "hash.h"
#include    "dir.h"
#include    "buf.h"

/*
 * The parsing of conditional expressions is based on this grammar:
 *	E -> F || E
 *	E -> F
 *	F -> T && F
 *	F -> T
 *	T -> defined(variable)
 *	T -> make(target)
 *	T -> exists(file)
 *	T -> empty(varspec)
 *	T -> target(name)
 *	T -> commands(name)
 *	T -> symbol
 *	T -> $(varspec) op value
 *	T -> $(varspec) == "string"
 *	T -> $(varspec) != "string"
 *	T -> "string"
 *	T -> ( E )
 *	T -> ! T
 *	op -> == | != | > | < | >= | <=
 *
 * 'symbol' is some other symbol to which the default function (condDefProc)
 * is applied.
 *
 * Tokens are scanned from the 'condExpr' string. The scanner (CondToken)
 * will return And for '&' and '&&', Or for '|' and '||', Not for '!',
 * LParen for '(', RParen for ')' and will evaluate the other terminal
 * symbols, using either the default function or the function given in the
 * terminal, and return the result as either True or False.
 *
 * All Non-Terminal functions (CondE, CondF and CondT) return Err on error.
 */
typedef enum {
    And, Or, Not, True, False, LParen, RParen, EndOfFile, None, Err
} Token;

/*-
 * Structures to handle elegantly the different forms of #if's. The
 * last two fields are stored in condInvert and condDefProc, respectively.
 */
static void CondPushBack(Token);
static int CondGetArg(char **, char **, const char *);
static Boolean CondDoDefined(int, const char *);
static int CondStrMatch(const void *, const void *);
static Boolean CondDoMake(int, const char *);
static Boolean CondDoExists(int, const char *);
static Boolean CondDoTarget(int, const char *);
static Boolean CondDoCommands(int, const char *);
static Boolean CondCvtArg(char *, double *);
static Token CondToken(Boolean);
static Token CondT(Boolean);
static Token CondF(Boolean);
static Token CondE(Boolean);
static int do_Cond_EvalExpression(Boolean *);

static const struct If {
    const char	*form;	      /* Form of if */
    int		formlen;      /* Length of form */
    Boolean	doNot;	      /* TRUE if default function should be negated */
    Boolean	(*defProc)(int, const char *); /* Default function to apply */
} ifs[] = {
    { "def",	  3,	  FALSE,  CondDoDefined },
    { "ndef",	  4,	  TRUE,	  CondDoDefined },
    { "make",	  4,	  FALSE,  CondDoMake },
    { "nmake",	  5,	  TRUE,	  CondDoMake },
    { "",	  0,	  FALSE,  CondDoDefined },
    { NULL,	  0,	  FALSE,  NULL }
};

static const struct If *if_info;        /* Info for current statement */
static char 	  *condExpr;	    	/* The expression to parse */
static Token	  condPushBack=None;	/* Single push-back token used in
					 * parsing */

static unsigned int	cond_depth = 0;  	/* current .if nesting level */
static unsigned int	cond_min_depth = 0;  	/* depth at makefile open */

static int
istoken(const char *str, const char *tok, size_t len)
{
	return strncmp(str, tok, len) == 0 && !isalpha((unsigned char)str[len]);
}

/*-
 *-----------------------------------------------------------------------
 * CondPushBack --
 *	Push back the most recent token read. We only need one level of
 *	this, so the thing is just stored in 'condPushback'.
 *
 * Input:
 *	t		Token to push back into the "stream"
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	condPushback is overwritten.
 *
 *-----------------------------------------------------------------------
 */
static void
CondPushBack(Token t)
{
    condPushBack = t;
}

/*-
 *-----------------------------------------------------------------------
 * CondGetArg --
 *	Find the argument of a built-in function.
 *
 * Input:
 *	parens		TRUE if arg should be bounded by parens
 *
 * Results:
 *	The length of the argument and the address of the argument.
 *
 * Side Effects:
 *	The pointer is set to point to the closing parenthesis of the
 *	function call.
 *
 *-----------------------------------------------------------------------
 */
static int
CondGetArg(char **linePtr, char **argPtr, const char *func)
{
    char	  *cp;
    int	    	  argLen;
    Buffer	  buf;
    int           paren_depth;
    char          ch;

    cp = *linePtr;
    if (func != NULL)
	/* Skip opening '(' - verfied by caller */
	cp++;

    if (*cp == '\0') {
	/*
	 * No arguments whatsoever. Because 'make' and 'defined' aren't really
	 * "reserved words", we don't print a message. I think this is better
	 * than hitting the user with a warning message every time s/he uses
	 * the word 'make' or 'defined' at the beginning of a symbol...
	 */
	*argPtr = NULL;
	return (0);
    }

    while (*cp == ' ' || *cp == '\t') {
	cp++;
    }

    /*
     * Create a buffer for the argument and start it out at 16 characters
     * long. Why 16? Why not?
     */
    Buf_Init(&buf, 16);

    paren_depth = 0;
    for (;;) {
	ch = *cp;
	if (ch == 0 || ch == ' ' || ch == '\t')
	    break;
	if ((ch == '&' || ch == '|') && paren_depth == 0)
	    break;
	if (*cp == '$') {
	    /*
	     * Parse the variable spec and install it as part of the argument
	     * if it's valid. We tell Var_Parse to complain on an undefined
	     * variable, so we don't do it too. Nor do we return an error,
	     * though perhaps we should...
	     */
	    char  	*cp2;
	    int		len;
	    void	*freeIt;

	    cp2 = Var_Parse(cp, VAR_CMD, TRUE, &len, &freeIt);
	    Buf_AddBytes(&buf, strlen(cp2), cp2);
	    if (freeIt)
		free(freeIt);
	    cp += len;
	    continue;
	}
	if (ch == '(')
	    paren_depth++;
	else
	    if (ch == ')' && --paren_depth < 0)
		break;
	Buf_AddByte(&buf, *cp);
	cp++;
    }

    *argPtr = Buf_GetAll(&buf, &argLen);
    Buf_Destroy(&buf, FALSE);

    while (*cp == ' ' || *cp == '\t') {
	cp++;
    }

    if (func != NULL && *cp++ != ')') {
	Parse_Error(PARSE_WARNING, "Missing closing parenthesis for %s()",
		     func);
	return (0);
    }

    *linePtr = cp;
    return (argLen);
}

/*-
 *-----------------------------------------------------------------------
 * CondDoDefined --
 *	Handle the 'defined' function for conditionals.
 *
 * Results:
 *	TRUE if the given variable is defined.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondDoDefined(int argLen, const char *arg)
{
    char    *p1;
    Boolean result;

    if (Var_Value(arg, VAR_CMD, &p1) != NULL) {
	result = TRUE;
    } else {
	result = FALSE;
    }
    if (p1)
	free(p1);
    return (result);
}

/*-
 *-----------------------------------------------------------------------
 * CondStrMatch --
 *	Front-end for Str_Match so it returns 0 on match and non-zero
 *	on mismatch. Callback function for CondDoMake via Lst_Find
 *
 * Results:
 *	0 if string matches pattern
 *
 * Side Effects:
 *	None
 *
 *-----------------------------------------------------------------------
 */
static int
CondStrMatch(const void *string, const void *pattern)
{
    return(!Str_Match(string, pattern));
}

/*-
 *-----------------------------------------------------------------------
 * CondDoMake --
 *	Handle the 'make' function for conditionals.
 *
 * Results:
 *	TRUE if the given target is being made.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondDoMake(int argLen, const char *arg)
{
    return Lst_Find(create, arg, CondStrMatch) != NULL;
}

/*-
 *-----------------------------------------------------------------------
 * CondDoExists --
 *	See if the given file exists.
 *
 * Results:
 *	TRUE if the file exists and FALSE if it does not.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondDoExists(int argLen, const char *arg)
{
    Boolean result;
    char    *path;

    path = Dir_FindFile(arg, dirSearchPath);
    if (path != NULL) {
	result = TRUE;
	free(path);
    } else {
	result = FALSE;
    }
    if (DEBUG(COND)) {
	fprintf(debug_file, "exists(%s) result is \"%s\"\n",
	       arg, path ? path : "");
    }    
    return (result);
}

/*-
 *-----------------------------------------------------------------------
 * CondDoTarget --
 *	See if the given node exists and is an actual target.
 *
 * Results:
 *	TRUE if the node exists as a target and FALSE if it does not.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondDoTarget(int argLen, const char *arg)
{
    GNode   *gn;

    gn = Targ_FindNode(arg, TARG_NOCREATE);
    return (gn != NULL) && !OP_NOP(gn->type);
}

/*-
 *-----------------------------------------------------------------------
 * CondDoCommands --
 *	See if the given node exists and is an actual target with commands
 *	associated with it.
 *
 * Results:
 *	TRUE if the node exists as a target and has commands associated with
 *	it and FALSE if it does not.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondDoCommands(int argLen, const char *arg)
{
    GNode   *gn;

    gn = Targ_FindNode(arg, TARG_NOCREATE);
    return (gn != NULL) && !OP_NOP(gn->type) && !Lst_IsEmpty(gn->commands);
}

/*-
 *-----------------------------------------------------------------------
 * CondCvtArg --
 *	Convert the given number into a double.
 *	We try a base 10 or 16 integer conversion first, if that fails
 *	then we try a floating point conversion instead.
 *
 * Results:
 *	Sets 'value' to double value of string.
 *	Returns 'true' if the convertion suceeded
 *
 *-----------------------------------------------------------------------
 */
static Boolean
CondCvtArg(char *str, double *value)
{
    char *eptr, ech;
    unsigned long l_val;
    double d_val;

    errno = 0;
    l_val = strtoul(str, &eptr, str[1] == 'x' ? 16 : 10);
    ech = *eptr;
    if (ech == 0 && errno != ERANGE) {
	d_val = str[0] == '-' ? -(double)-l_val : (double)l_val;
    } else {
	if (ech != 0 && ech != '.' && ech != 'e' && ech != 'E')
	    return FALSE;
	d_val = strtod(str, &eptr);
	if (*eptr)
	    return FALSE;
    }

    *value = d_val;
    return TRUE;
}

/*-
 *-----------------------------------------------------------------------
 * CondGetString --
 *	Get a string from a variable reference or an optionally quoted
 *	string.  This is called for the lhs and rhs of string compares.
 *
 * Results:
 *	Sets freeIt if needed,
 *	Sets quoted if string was quoted,
 *	Returns NULL on error,
 *	else returns string - absent any quotes.
 *
 * Side Effects:
 *	Moves condExpr to end of this token.
 *
 *
 *-----------------------------------------------------------------------
 */
/* coverity:[+alloc : arg-*2] */
static char *
CondGetString(Boolean doEval, Boolean *quoted, void **freeIt)
{
    Buffer buf;
    char *cp;
    char *str;
    int	len;
    int qt;
    char *start;

    Buf_Init(&buf, 0);
    str = NULL;
    *freeIt = NULL;
    *quoted = qt = *condExpr == '"' ? 1 : 0;
    if (qt)
	condExpr++;
    for (start = condExpr; *condExpr && str == NULL; condExpr++) {
	switch (*condExpr) {
	case '\\':
	    if (condExpr[1] != '\0') {
		condExpr++;
		Buf_AddByte(&buf, *condExpr);
	    }
	    break;
	case '"':
	    if (qt) {
		condExpr++;		/* we don't want the quotes */
		goto got_str;
	    } else
		Buf_AddByte(&buf, *condExpr); /* likely? */
	    break;
	case ')':
	case '!':
	case '=':
	case '>':
	case '<':
	case ' ':
	case '\t':
	    if (!qt)
		goto got_str;
	    else
		Buf_AddByte(&buf, *condExpr);
	    break;
	case '$':
	    /* if we are in quotes, then an undefined variable is ok */
	    str = Var_Parse(condExpr, VAR_CMD, (qt ? 0 : doEval),
			    &len, freeIt);
	    if (str == var_Error) {
		if (*freeIt) {
		    free(*freeIt);
		    *freeIt = NULL;
		}
		/*
		 * Even if !doEval, we still report syntax errors, which
		 * is what getting var_Error back with !doEval means.
		 */
		str = NULL;
		goto cleanup;
	    }
	    condExpr += len;
	    /*
	     * If the '$' was first char (no quotes), and we are
	     * followed by space, the operator or end of expression,
	     * we are done.
	     */
	    if ((condExpr == start + len) &&
		(*condExpr == '\0' ||
		 isspace((unsigned char) *condExpr) ||
		 strchr("!=><)", *condExpr))) {
		goto cleanup;
	    }
	    /*
	     * Nope, we better copy str to buf
	     */
	    for (cp = str; *cp; cp++) {
		Buf_AddByte(&buf, *cp);
	    }
	    if (*freeIt) {
		free(*freeIt);
		*freeIt = NULL;
	    }
	    str = NULL;			/* not finished yet */
	    condExpr--;			/* don't skip over next char */
	    break;
	default:
	    Buf_AddByte(&buf, *condExpr);
	    break;
	}
    }
 got_str:
    str = Buf_GetAll(&buf, NULL);
    *freeIt = str;
 cleanup:
    Buf_Destroy(&buf, FALSE);
    return str;
}

/*-
 *-----------------------------------------------------------------------
 * CondToken --
 *	Return the next token from the input.
 *
 * Results:
 *	A Token for the next lexical token in the stream.
 *
 * Side Effects:
 *	condPushback will be set back to None if it is used.
 *
 *-----------------------------------------------------------------------
 */
static Token
compare_expression(Boolean doEval)
{
    Token	t;
    char	*lhs;
    char	*rhs;
    char	*op;
    void	*lhsFree;
    void	*rhsFree;
    Boolean lhsQuoted;
    Boolean rhsQuoted;
    double  	left, right;

    t = Err;
    rhs = NULL;
    lhsFree = rhsFree = FALSE;
    lhsQuoted = rhsQuoted = FALSE;
    
    /*
     * Parse the variable spec and skip over it, saving its
     * value in lhs.
     */
    lhs = CondGetString(doEval, &lhsQuoted, &lhsFree);
    if (!lhs)
	goto done;

    /*
     * Skip whitespace to get to the operator
     */
    while (isspace((unsigned char) *condExpr))
	condExpr++;

    /*
     * Make sure the operator is a valid one. If it isn't a
     * known relational operator, pretend we got a
     * != 0 comparison.
     */
    op = condExpr;
    switch (*condExpr) {
	case '!':
	case '=':
	case '<':
	case '>':
	    if (condExpr[1] == '=') {
		condExpr += 2;
	    } else {
		condExpr += 1;
	    }
	    break;
	default:
	    if (!doEval) {
		t = False;
		goto done;
	    }
	    /* For .ifxxx "..." check for non-empty string. */
	    if (lhsQuoted) {
		t = lhs[0] != 0 ? True : False;
		goto done;
	    }
	    /* For .ifxxx <number> compare against zero */
	    if (CondCvtArg(lhs, &left)) { 
		t = left != 0.0 ? True : False;
		goto done;
	    }
	    /* For .if ${...} check for non-empty string (defProc is ifdef). */
	    if (if_info->form[0] == 0) {
		t = lhs[0] != 0 ? True : False;
		goto done;
	    }
	    /* Otherwise action default test ... */
	    t = if_info->defProc(strlen(lhs), lhs) != if_info->doNot ? True : False;
	    goto done;
    }

    while (isspace((unsigned char)*condExpr))
	condExpr++;

    if (*condExpr == '\0') {
	Parse_Error(PARSE_WARNING,
		    "Missing right-hand-side of operator");
	goto done;
    }

    rhs = CondGetString(doEval, &rhsQuoted, &rhsFree);
    if (!rhs)
	goto done;

    if (rhsQuoted || lhsQuoted) {
do_string_compare:
	if (((*op != '!') && (*op != '=')) || (op[1] != '=')) {
	    Parse_Error(PARSE_WARNING,
    "String comparison operator should be either == or !=");
	    goto done;
	}

	if (DEBUG(COND)) {
	    fprintf(debug_file, "lhs = \"%s\", rhs = \"%s\", op = %.2s\n",
		   lhs, rhs, op);
	}
	/*
	 * Null-terminate rhs and perform the comparison.
	 * t is set to the result.
	 */
	if (*op == '=') {
	    t = strcmp(lhs, rhs) ? False : True;
	} else {
	    t = strcmp(lhs, rhs) ? True : False;
	}
    } else {
	/*
	 * rhs is either a float or an integer. Convert both the
	 * lhs and the rhs to a double and compare the two.
	 */
    
	if (!CondCvtArg(lhs, &left) || !CondCvtArg(rhs, &right))
	    goto do_string_compare;

	if (DEBUG(COND)) {
	    fprintf(debug_file, "left = %f, right = %f, op = %.2s\n", left,
		   right, op);
	}
	switch(op[0]) {
	case '!':
	    if (op[1] != '=') {
		Parse_Error(PARSE_WARNING,
			    "Unknown operator");
		goto done;
	    }
	    t = (left != right ? True : False);
	    break;
	case '=':
	    if (op[1] != '=') {
		Parse_Error(PARSE_WARNING,
			    "Unknown operator");
		goto done;
	    }
	    t = (left == right ? True : False);
	    break;
	case '<':
	    if (op[1] == '=') {
		t = (left <= right ? True : False);
	    } else {
		t = (left < right ? True : False);
	    }
	    break;
	case '>':
	    if (op[1] == '=') {
		t = (left >= right ? True : False);
	    } else {
		t = (left > right ? True : False);
	    }
	    break;
	}
    }

done:
    if (lhsFree)
	free(lhsFree);
    if (rhsFree)
	free(rhsFree);
    return t;
}

static int
get_mpt_arg(char **linePtr, char **argPtr, const char *func)
{
    /*
     * Use Var_Parse to parse the spec in parens and return
     * True if the resulting string is empty.
     */
    int	    length;
    void    *freeIt;
    char    *val;
    char    *cp = *linePtr;

    /* We do all the work here and return the result as the length */
    *argPtr = NULL;

    val = Var_Parse(cp - 1, VAR_CMD, FALSE, &length, &freeIt);
    /*
     * Advance *linePtr to beyond the closing ). Note that
     * we subtract one because 'length' is calculated from 'cp - 1'.
     */
    *linePtr = cp - 1 + length;

    if (val == var_Error) {
	free(freeIt);
	return -1;
    }

    /* A variable is empty when it just contains spaces... 4/15/92, christos */
    while (isspace(*(unsigned char *)val))
	val++;

    /*
     * For consistency with the other functions we can't generate the
     * true/false here.
     */
    length = *val ? 2 : 1;
    if (freeIt)
	free(freeIt);
    return length;
}

static Boolean
CondDoEmpty(int arglen, const char *arg)
{
    return arglen == 1;
}

static Token
compare_function(Boolean doEval)
{
    static const struct fn_def {
	const char  *fn_name;
	int         fn_name_len;
        int         (*fn_getarg)(char **, char **, const char *);
	Boolean     (*fn_proc)(int, const char *);
    } fn_defs[] = {
	{ "defined",   7, CondGetArg, CondDoDefined },
	{ "make",      4, CondGetArg, CondDoMake },
	{ "exists",    6, CondGetArg, CondDoExists },
	{ "empty",     5, get_mpt_arg, CondDoEmpty },
	{ "target",    6, CondGetArg, CondDoTarget },
	{ "commands",  8, CondGetArg, CondDoCommands },
	{ NULL,        0, NULL, NULL },
    };
    const struct fn_def *fn_def;
    Token	t;
    char	*arg = NULL;
    int	arglen;
    char *cp = condExpr;
    char *cp1;

    for (fn_def = fn_defs; fn_def->fn_name != NULL; fn_def++) {
	if (!istoken(cp, fn_def->fn_name, fn_def->fn_name_len))
	    continue;
	cp += fn_def->fn_name_len;
	/* There can only be whitespace before the '(' */
	while (isspace(*(unsigned char *)cp))
	    cp++;
	if (*cp != '(')
	    break;

	arglen = fn_def->fn_getarg(&cp, &arg, fn_def->fn_name);
	if (arglen <= 0) {
	    condExpr = cp;
	    return arglen < 0 ? Err : False;
	}
	/* Evaluate the argument using the required function. */
	t = !doEval || fn_def->fn_proc(arglen, arg) ? True : False;
	if (arg)
	    free(arg);
	condExpr = cp;
	return t;
    }

    /* Push anything numeric through the compare expression */
    cp = condExpr;
    if (isdigit((unsigned char)cp[0]) || strchr("+-", cp[0]))
	return compare_expression(doEval);

    /*
     * Most likely we have a naked token to apply the default function to.
     * However ".if a == b" gets here when the "a" is unquoted and doesn't
     * start with a '$'. This surprises people.
     * If what follows the function argument is a '=' or '!' then the syntax
     * would be invalid if we did "defined(a)" - so instead treat as an
     * expression.
     */
    arglen = CondGetArg(&cp, &arg, NULL);
    for (cp1 = cp; isspace(*(unsigned char *)cp1); cp1++)
	continue;
    if (*cp1 == '=' || *cp1 == '!')
	return compare_expression(doEval);
    condExpr = cp;

    /*
     * Evaluate the argument using the default function.
     * This path always treats .if as .ifdef. To get here the character
     * after .if must have been taken literally, so the argument cannot
     * be empty - even if it contained a variable expansion.
     */
    t = !doEval || if_info->defProc(arglen, arg) != if_info->doNot ? True : False;
    if (arg)
	free(arg);
    return t;
}

static Token
CondToken(Boolean doEval)
{
    Token t;

    t = condPushBack;
    if (t != None) {
	condPushBack = None;
	return t;
    }

    while (*condExpr == ' ' || *condExpr == '\t') {
	condExpr++;
    }

    switch (*condExpr) {

    case '(':
	condExpr++;
	return LParen;

    case ')':
	condExpr++;
	return RParen;

    case '|':
	if (condExpr[1] == '|') {
	    condExpr++;
	}
	condExpr++;
	return Or;

    case '&':
	if (condExpr[1] == '&') {
	    condExpr++;
	}
	condExpr++;
	return And;

    case '!':
	condExpr++;
	return Not;

    case '#':
    case '\n':
    case '\0':
	return EndOfFile;

    case '"':
    case '$':
	return compare_expression(doEval);

    default:
	return compare_function(doEval);
    }
}

/*-
 *-----------------------------------------------------------------------
 * CondT --
 *	Parse a single term in the expression. This consists of a terminal
 *	symbol or Not and a terminal symbol (not including the binary
 *	operators):
 *	    T -> defined(variable) | make(target) | exists(file) | symbol
 *	    T -> ! T | ( E )
 *
 * Results:
 *	True, False or Err.
 *
 * Side Effects:
 *	Tokens are consumed.
 *
 *-----------------------------------------------------------------------
 */
static Token
CondT(Boolean doEval)
{
    Token   t;

    t = CondToken(doEval);

    if (t == EndOfFile) {
	/*
	 * If we reached the end of the expression, the expression
	 * is malformed...
	 */
	t = Err;
    } else if (t == LParen) {
	/*
	 * T -> ( E )
	 */
	t = CondE(doEval);
	if (t != Err) {
	    if (CondToken(doEval) != RParen) {
		t = Err;
	    }
	}
    } else if (t == Not) {
	t = CondT(doEval);
	if (t == True) {
	    t = False;
	} else if (t == False) {
	    t = True;
	}
    }
    return (t);
}

/*-
 *-----------------------------------------------------------------------
 * CondF --
 *	Parse a conjunctive factor (nice name, wot?)
 *	    F -> T && F | T
 *
 * Results:
 *	True, False or Err
 *
 * Side Effects:
 *	Tokens are consumed.
 *
 *-----------------------------------------------------------------------
 */
static Token
CondF(Boolean doEval)
{
    Token   l, o;

    l = CondT(doEval);
    if (l != Err) {
	o = CondToken(doEval);

	if (o == And) {
	    /*
	     * F -> T && F
	     *
	     * If T is False, the whole thing will be False, but we have to
	     * parse the r.h.s. anyway (to throw it away).
	     * If T is True, the result is the r.h.s., be it an Err or no.
	     */
	    if (l == True) {
		l = CondF(doEval);
	    } else {
		(void)CondF(FALSE);
	    }
	} else {
	    /*
	     * F -> T
	     */
	    CondPushBack(o);
	}
    }
    return (l);
}

/*-
 *-----------------------------------------------------------------------
 * CondE --
 *	Main expression production.
 *	    E -> F || E | F
 *
 * Results:
 *	True, False or Err.
 *
 * Side Effects:
 *	Tokens are, of course, consumed.
 *
 *-----------------------------------------------------------------------
 */
static Token
CondE(Boolean doEval)
{
    Token   l, o;

    l = CondF(doEval);
    if (l != Err) {
	o = CondToken(doEval);

	if (o == Or) {
	    /*
	     * E -> F || E
	     *
	     * A similar thing occurs for ||, except that here we make sure
	     * the l.h.s. is False before we bother to evaluate the r.h.s.
	     * Once again, if l is False, the result is the r.h.s. and once
	     * again if l is True, we parse the r.h.s. to throw it away.
	     */
	    if (l == False) {
		l = CondE(doEval);
	    } else {
		(void)CondE(FALSE);
	    }
	} else {
	    /*
	     * E -> F
	     */
	    CondPushBack(o);
	}
    }
    return (l);
}

/*-
 *-----------------------------------------------------------------------
 * Cond_EvalExpression --
 *	Evaluate an expression in the passed line. The expression
 *	consists of &&, ||, !, make(target), defined(variable)
 *	and parenthetical groupings thereof.
 *
 * Results:
 *	COND_PARSE	if the condition was valid grammatically
 *	COND_INVALID  	if not a valid conditional.
 *
 *	(*value) is set to the boolean value of the condition
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
int
Cond_EvalExpression(const struct If *info, char *line, Boolean *value, int eprint)
{
    static const struct If *dflt_info;
    const struct If *sv_if_info = if_info;
    char *sv_condExpr = condExpr;
    Token sv_condPushBack = condPushBack;
    int rval;

    while (*line == ' ' || *line == '\t')
	line++;

    if (info == NULL && (info = dflt_info) == NULL) {
	/* Scan for the entry for .if - it can't be first */
	for (info = ifs; ; info++)
	    if (info->form[0] == 0)
		break;
	dflt_info = info;
    }

    if_info = info != NULL ? info : ifs + 4;
    condExpr = line;
    condPushBack = None;

    rval = do_Cond_EvalExpression(value);

    if (rval == COND_INVALID && eprint)
	Parse_Error(PARSE_FATAL, "Malformed conditional (%s)", line);

    if_info = sv_if_info;
    condExpr = sv_condExpr;
    condPushBack = sv_condPushBack;

    return rval;
}

static int
do_Cond_EvalExpression(Boolean *value)
{

    switch (CondE(TRUE)) {
    case True:
	if (CondToken(TRUE) == EndOfFile) {
	    *value = TRUE;
	    return COND_PARSE;
	}
	break;
    case False:
	if (CondToken(TRUE) == EndOfFile) {
	    *value = FALSE;
	    return COND_PARSE;
	}
	break;
    default:
    case Err:
	break;
    }

    return COND_INVALID;
}


/*-
 *-----------------------------------------------------------------------
 * Cond_Eval --
 *	Evaluate the conditional in the passed line. The line
 *	looks like this:
 *	    .<cond-type> <expr>
 *	where <cond-type> is any of if, ifmake, ifnmake, ifdef,
 *	ifndef, elif, elifmake, elifnmake, elifdef, elifndef
 *	and <expr> consists of &&, ||, !, make(target), defined(variable)
 *	and parenthetical groupings thereof.
 *
 * Input:
 *	line		Line to parse
 *
 * Results:
 *	COND_PARSE	if should parse lines after the conditional
 *	COND_SKIP	if should skip lines after the conditional
 *	COND_INVALID  	if not a valid conditional.
 *
 * Side Effects:
 *	None.
 *
 * Note that the states IF_ACTIVE and ELSE_ACTIVE are only different in order
 * to detect splurious .else lines (as are SKIP_TO_ELSE and SKIP_TO_ENDIF)
 * otherwise .else could be treated as '.elif 1'.
 *
 *-----------------------------------------------------------------------
 */
int
Cond_Eval(char *line)
{
    #define	    MAXIF	64	/* maximum depth of .if'ing */
    enum if_states {
	IF_ACTIVE,		/* .if or .elif part active */
	ELSE_ACTIVE,		/* .else part active */
	SEARCH_FOR_ELIF,	/* searching for .elif/else to execute */
	SKIP_TO_ELSE,           /* has been true, but not seen '.else' */
	SKIP_TO_ENDIF		/* nothing else to execute */
    };
    static enum if_states cond_state[MAXIF + 1] = { IF_ACTIVE };

    const struct If *ifp;
    Boolean 	    isElif;
    Boolean 	    value;
    int	    	    level;  	/* Level at which to report errors. */
    enum if_states  state;

    level = PARSE_FATAL;

    /* skip leading character (the '.') and any whitespace */
    for (line++; *line == ' ' || *line == '\t'; line++)
	continue;

    /* Find what type of if we're dealing with.  */
    if (line[0] == 'e') {
	if (line[1] != 'l') {
	    if (!istoken(line + 1, "ndif", 4))
		return COND_INVALID;
	    /* End of conditional section */
	    if (cond_depth == cond_min_depth) {
		Parse_Error(level, "if-less endif");
		return COND_PARSE;
	    }
	    /* Return state for previous conditional */
	    cond_depth--;
	    if (cond_depth > MAXIF)
		return COND_SKIP;
	    return cond_state[cond_depth] <= ELSE_ACTIVE ? COND_PARSE : COND_SKIP;
	}

	/* Quite likely this is 'else' or 'elif' */
	line += 2;
	if (istoken(line, "se", 2)) {
	    /* It is else... */
	    if (cond_depth == cond_min_depth) {
		Parse_Error(level, "if-less else");
		return COND_PARSE;
	    }

	    if (cond_depth > MAXIF)
		return COND_SKIP;
	    state = cond_state[cond_depth];
	    switch (state) {
	    case SEARCH_FOR_ELIF:
		state = ELSE_ACTIVE;
		break;
	    case ELSE_ACTIVE:
	    case SKIP_TO_ENDIF:
		Parse_Error(PARSE_WARNING, "extra else");
		/* FALLTHROUGH */
	    default:
	    case IF_ACTIVE:
	    case SKIP_TO_ELSE:
		state = SKIP_TO_ENDIF;
		break;
	    }
	    cond_state[cond_depth] = state;
	    return state <= ELSE_ACTIVE ? COND_PARSE : COND_SKIP;
	}
	/* Assume for now it is an elif */
	isElif = TRUE;
    } else
	isElif = FALSE;

    if (line[0] != 'i' || line[1] != 'f')
	/* Not an ifxxx or elifxxx line */
	return COND_INVALID;

    /*
     * Figure out what sort of conditional it is -- what its default
     * function is, etc. -- by looking in the table of valid "ifs"
     */
    line += 2;
    for (ifp = ifs; ; ifp++) {
	if (ifp->form == NULL)
	    return COND_INVALID;
	if (istoken(ifp->form, line, ifp->formlen)) {
	    line += ifp->formlen;
	    break;
	}
    }

    /* Now we know what sort of 'if' it is... */

    if (isElif) {
	if (cond_depth == cond_min_depth) {
	    Parse_Error(level, "if-less elif");
	    return COND_PARSE;
	}
	if (cond_depth > MAXIF)
	    /* Error reported when we saw the .if ... */
	    return COND_SKIP;
	state = cond_state[cond_depth];
	if (state == SKIP_TO_ENDIF || state == ELSE_ACTIVE) {
	    Parse_Error(PARSE_WARNING, "extra elif");
	    cond_state[cond_depth] = SKIP_TO_ENDIF;
	    return COND_SKIP;
	}
	if (state != SEARCH_FOR_ELIF) {
	    /* Either just finished the 'true' block, or already SKIP_TO_ELSE */
	    cond_state[cond_depth] = SKIP_TO_ELSE;
	    return COND_SKIP;
	}
    } else {
	/* Normal .if */
	if (cond_depth >= MAXIF) {
	    cond_depth++;
	    Parse_Error(PARSE_FATAL, "Too many nested if's. %d max.", MAXIF);
	    return COND_SKIP;
	}
	state = cond_state[cond_depth];
	cond_depth++;
	if (state > ELSE_ACTIVE) {
	    /* If we aren't parsing the data, treat as always false */
	    cond_state[cond_depth] = SKIP_TO_ELSE;
	    return COND_SKIP;
	}
    }

    /* And evaluate the conditional expresssion */
    if (Cond_EvalExpression(ifp, line, &value, 1) == COND_INVALID) {
	/* Syntax error in conditional, error message already output. */
	/* Skip everything to matching .endif */
	cond_state[cond_depth] = SKIP_TO_ELSE;
	return COND_SKIP;
    }

    if (!value) {
	cond_state[cond_depth] = SEARCH_FOR_ELIF;
	return COND_SKIP;
    }
    cond_state[cond_depth] = IF_ACTIVE;
    return COND_PARSE;
}



/*-
 *-----------------------------------------------------------------------
 * Cond_End --
 *	Make sure everything's clean at the end of a makefile.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Parse_Error will be called if open conditionals are around.
 *
 *-----------------------------------------------------------------------
 */
void
Cond_restore_depth(unsigned int saved_depth)
{
    int open_conds = cond_depth - cond_min_depth;

    if (open_conds != 0 || saved_depth > cond_depth) {
	Parse_Error(PARSE_FATAL, "%d open conditional%s", open_conds,
		    open_conds == 1 ? "" : "s");
	cond_depth = cond_min_depth;
    }

    cond_min_depth = saved_depth;
}

unsigned int
Cond_save_depth(void)
{
    int depth = cond_min_depth;

    cond_min_depth = cond_depth;
    return depth;
}
