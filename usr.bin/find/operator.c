/*	$NetBSD: operator.c,v 1.6 1998/02/21 22:47:21 christos Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Cimarron D. Taylor of the University of California, Berkeley.
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

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "from: @(#)operator.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD: operator.c,v 1.6 1998/02/21 22:47:21 christos Exp $");
#endif
#endif /* not lint */

#include <sys/types.h>

#include <err.h>
#include <fts.h>
#include <stdio.h>

#include "find.h"
    
static PLAN *yanknode __P((PLAN **));
static PLAN *yankexpr __P((PLAN **));

/*
 * yanknode --
 *	destructively removes the top from the plan
 */
static PLAN *
yanknode(planp)    
	PLAN **planp;		/* pointer to top of plan (modified) */
{
	PLAN *node;		/* top node removed from the plan */
    
	if ((node = (*planp)) == NULL)
		return (NULL);
	(*planp) = (*planp)->next;
	node->next = NULL;
	return (node);
}
 
/*
 * yankexpr --
 *	Removes one expression from the plan.  This is used mainly by
 *	paren_squish.  In comments below, an expression is either a
 *	simple node or a N_EXPR node containing a list of simple nodes.
 */
static PLAN *
yankexpr(planp)    
	PLAN **planp;		/* pointer to top of plan (modified) */
{
	PLAN *next;		/* temp node holding subexpression results */
	PLAN *node;		/* pointer to returned node or expression */
	PLAN *tail;		/* pointer to tail of subplan */
	PLAN *subplan;		/* pointer to head of ( ) expression */
    
	/* first pull the top node from the plan */
	if ((node = yanknode(planp)) == NULL)
		return (NULL);
    
	/*
	 * If the node is an '(' then we recursively slurp up expressions
	 * until we find its associated ')'.  If it's a closing paren we
	 * just return it and unwind our recursion; all other nodes are
	 * complete expressions, so just return them.
	 */
	if (node->type == N_OPENPAREN)
		for (tail = subplan = NULL;;) {
			if ((next = yankexpr(planp)) == NULL)
				err(1, "(: missing closing ')'");
			/*
			 * If we find a closing ')' we store the collected
			 * subplan in our '(' node and convert the node to
			 * a N_EXPR.  The ')' we found is ignored.  Otherwise,
			 * we just continue to add whatever we get to our
			 * subplan.
			 */
			if (next->type == N_CLOSEPAREN) {
				if (subplan == NULL)
					errx(1, "(): empty inner expression");
				node->p_data[0] = subplan;
				node->type = N_EXPR;
				node->eval = f_expr;
				break;
			} else {
				if (subplan == NULL)
					tail = subplan = next;
				else {
					tail->next = next;
					tail = next;
				}
				tail->next = NULL;
			}
		}
	return (node);
}
 
/*
 * paren_squish --
 *	replaces "parentheisized" plans in our search plan with "expr" nodes.
 */
PLAN *
paren_squish(plan)
	PLAN *plan;		/* plan with ( ) nodes */
{
	PLAN *expr;		/* pointer to next expression */
	PLAN *tail;		/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	result = tail = NULL;

	/*
	 * the basic idea is to have yankexpr do all our work and just
	 * collect it's results together.
	 */
	while ((expr = yankexpr(&plan)) != NULL) {
		/*
		 * if we find an unclaimed ')' it means there is a missing
		 * '(' someplace.
		 */
		if (expr->type == N_CLOSEPAREN)
			errx(1, "): no beginning '('");

		/* add the expression to our result plan */
		if (result == NULL)
			tail = result = expr;
		else {
			tail->next = expr;
			tail = expr;
		}
		tail->next = NULL;
	}
	return (result);
}
 
/*
 * not_squish --
 *	compresses "!" expressions in our search plan.
 */
PLAN *
not_squish(plan)
	PLAN *plan;		/* plan to process */
{
	PLAN *next;		/* next node being processed */
	PLAN *node;		/* temporary node used in N_NOT processing */
	PLAN *tail;		/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	tail = result = next = NULL;
    
	while ((next = yanknode(&plan)) != NULL) {
		/*
		 * if we encounter a ( expression ) then look for nots in
		 * the expr subplan.
		 */
		if (next->type == N_EXPR)
			next->p_data[0] = not_squish(next->p_data[0]);

		/*
		 * if we encounter a not, then snag the next node and place
		 * it in the not's subplan.  As an optimization we compress
		 * several not's to zero or one not.
		 */
		if (next->type == N_NOT) {
			int notlevel = 1;

			node = yanknode(&plan);
			while (node->type == N_NOT) {
				++notlevel;
				node = yanknode(&plan);
			}
			if (node == NULL)
				errx(1, "!: no following expression");
			if (node->type == N_OR)
				errx(1, "!: nothing between ! and -o");
			if (notlevel % 2 != 1)
				next = node;
			else
				next->p_data[0] = node;
		}

		/* add the node to our result plan */
		if (result == NULL)
			tail = result = next;
		else {
			tail->next = next;
			tail = next;
		}
		tail->next = NULL;
	}
	return (result);
}
 
/*
 * or_squish --
 *	compresses -o expressions in our search plan.
 */
PLAN *
or_squish(plan)
	PLAN *plan;		/* plan with ors to be squished */
{
	PLAN *next;		/* next node being processed */
	PLAN *tail;		/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	tail = result = next = NULL;
    
	while ((next = yanknode(&plan)) != NULL) {
		/*
		 * if we encounter a ( expression ) then look for or's in
		 * the expr subplan.
		 */
		if (next->type == N_EXPR)
			next->p_data[0] = or_squish(next->p_data[0]);

		/* if we encounter a not then look for not's in the subplan */
		if (next->type == N_NOT)
			next->p_data[0] = or_squish(next->p_data[0]);

		/*
		 * if we encounter an or, then place our collected plan in the
		 * or's first subplan and then recursively collect the
		 * remaining stuff into the second subplan and return the or.
		 */
		if (next->type == N_OR) {
			if (result == NULL)
				errx(1, "-o: no expression before -o");
			next->p_data[0] = result;
			next->p_data[1] = or_squish(plan);
			if (next->p_data[1] == NULL)
				errx(1, "-o: no expression after -o");
			return (next);
		}

		/* add the node to our result plan */
		if (result == NULL)
			tail = result = next;
		else {
			tail->next = next;
			tail = next;
		}
		tail->next = NULL;
	}
	return (result);
}
