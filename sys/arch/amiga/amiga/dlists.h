/*
 * Copyright (c) 1994 Christian E. Hopps
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
 *      This product includes software developed by Christian E. Hopps.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	$Id: dlists.h,v 1.2 1994/01/29 06:58:58 chopps Exp $
 */

#if ! defined (_DLISTS_H)
#define _DLISTS_H

#include <stddef.h>

typedef struct dll_node {
    struct dll_node *next;
    struct dll_node *prev;
} dll_node_t;

typedef struct dll_list {
	dll_node_t	*head;
	dll_node_t	*tail;
	dll_node_t	*tailprev;
} dll_list_t;


/* prototypes */
dll_node_t * dremove_tail  (dll_list_t *list);
dll_node_t * dremove_head  (dll_list_t *list);
void dinit_list  (dll_list_t *list);
void dinsert  (dll_node_t *inlist_node, dll_node_t *insert_node);
void dappend  (dll_node_t *inlist_node, dll_node_t *append_node);
void dadd_tail  (dll_list_t *list, dll_node_t *node);
void dadd_head  (dll_list_t *list, dll_node_t *node);

/* Double Linked list macros */
#define IS_DLIST_EMPTY(x) ( ((x)->tailprev) == (dll_node_t *)(x) )

#define dpush(l,x) dadd_head(l,x)
#define dpop(l)  dremove_head(l)
#define dtop(l)  (l->head)

#define denqueue(l,x) dadd_head(l,x)
#define ddequeue(l)  dremove_tail(l)


#endif /* _DLISTS_H */
