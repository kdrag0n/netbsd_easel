/*	$NetBSD: back.h,v 1.6 1997/10/10 08:59:41 lukem Exp $	*/

/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)back.h	8.1 (Berkeley) 5/31/93
 */

#include <sys/types.h>
#include <sys/uio.h>

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define rnum(r)	(random()%r)
#define D0	dice[0]
#define D1	dice[1]
#define swap	{D0 ^= D1; D1 ^= D0; D0 ^= D1; d0 = 1-d0;}

/*
 *
 * Some numerical conventions:
 *
 *	Arrays have white's value in [0], red in [1].
 *	Numeric values which are one color or the other use
 *	-1 for white, 1 for red.
 *	Hence, white will be negative values, red positive one.
 *	This makes a lot of sense since white is going in decending
 *	order around the board, and red is ascending.
 *
 */

char	EXEC[];			/* object for main program */
char	TEACH[];		/* object for tutorial program */

int	pnum;			/* color of player:
					-1 = white
					 1 = red
					 0 = both
					 2 = not yet init'ed */
char	args[100];		/* args passed to teachgammon and back */
int	acnt;			/* length of args */
int	aflag;			/* flag to ask for rules or instructions */
int	bflag;			/* flag for automatic board printing */
int	cflag;			/* case conversion flag */
int	hflag;			/* flag for cleaning screen */
int	mflag;			/* backgammon flag */
int	raflag;			/* 'roll again' flag for recovered game */
int	rflag;			/* recovered game flag */
int	tflag;			/* cursor addressing flag */
int	rfl;			/* saved value of rflag */
int	iroll;			/* special flag for inputting rolls */
int	board[26];		/* board:  negative values are white,
				   positive are red */
int	dice[2];		/* value of dice */
int	mvlim;			/* 'move limit':  max. number of moves */
int	mvl;			/* working copy of mvlim */
int	p[5];			/* starting position of moves */
int	g[5];			/* ending position of moves (goals) */
int	h[4];			/* flag for each move if a man was hit */
int	cturn;			/* whose turn it currently is:
					-1 = white
					 1 = red
					 0 = just quitted
					-2 = white just lost
					 2 = red just lost */
int	d0;			/* flag if dice have been reversed from
				   original position */
int	table[6][6];		/* odds table for possible rolls */
int	rscore;			/* red's score */
int	wscore;			/* white's score */
int	gvalue;			/* value of game (64 max.) */
int	dlast;			/* who doubled last (0 = neither) */
int	bar;			/* position of bar for current player */
int	home;			/* position of home for current player */
int	off[2];			/* number of men off board */
int	*offptr;		/* pointer to off for current player */
int	*offopp;		/* pointer to off for opponent */
int	in[2];			/* number of men in inner table */
int	*inptr;			/* pointer to in for current player */
int	*inopp;			/* pointer to in for opponent */

int	ncin;			/* number of characters in cin */
char	cin[100];		/* input line of current move
				   (used for reconstructing input after
				   a backspace) */

char	*color[];
				/* colors as strings */
char	**colorptr;		/* color of current player */
char	**Colorptr;		/* color of current player, capitalized */
int	colen;			/* length of color of current player */

struct termios	old, noech, raw;/* original tty status */

int	curr;			/* row position of cursor */
int	curc;			/* column position of cursor */
int	begscr;			/* 'beginning' of screen
				   (not including board) */

void	addbuf __P((int));
void	backone __P((int));
void	bsect __P((int, int, int, int));
void	buflush __P((void));
int	canhit __P((int, int));
int	checkd __P((int));
int	checkmove __P((int));
void	clear __P((void));
void	clend __P((void));
void	cline __P((void));
int	count __P((void));
void	curmove __P((int, int));
int	dotable __P((char, int));
void	errexit __P((const char *));
void	fancyc __P((int));
void	fboard __P((void));
void	fixcol __P((int, int, int, int, int));
void	fixpos __P((int, int, int, int, int));
void	fixtty __P((struct termios *));
void	getarg __P((char ***));
int	getcaps __P((char *));
void	getmove __P((void));
void	getout __P((int));
void	gwrite __P((void));
void	init __P((void));
int	last __P((void));
int	main __P((int, char *[]));
int	makmove __P((int));
int	movallow __P((void));
void	movback __P((int));
void	moverr __P((int));
int	movokay __P((int));
void	newline __P((void));
void	newpos __P((void));
void	nexturn __P((void));
void	norec __P((char *));
void	odds __P((int, int, int));
void	proll __P((void));
int	quit __P((void));
int	readc __P((void));
void	recover __P((char *));
void	refresh __P((void));
void	roll __P((void));
int	rsetbrd __P((void));
void	save __P((int));
void	strset __P((char *, char *));
int	text __P((char **));
void	tos __P((void));
void	wrboard __P((void));
void	wrbsub __P((void));
void	wrhit __P((int));
void	wrint __P((int));
void	writec __P((char));
void	writel __P((char *));
void	wrscore __P((void));
int	yorn __P((char));
