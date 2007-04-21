/*	$NetBSD: fwohcivar.h,v 1.26 2007/04/21 15:27:44 kiyohara Exp $	*/

/*-
 * Copyright (c) 2003 Hidetoshi SHimokawa
 * Copyright (c) 1998-2002 Katsushi Kobayashi and Hidetoshi SHimokawa
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
 *    must display the acknowledgement as bellow:
 *
 *    This product includes software developed by K. Kobayashi and H. Shimokawa
 *
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * $FreeBSD: /repoman/r/ncvs/src/sys/dev/firewire/fwohcivar.h,v 1.15 2007/03/16 04:25:02 simokawa Exp $
 *
 */

#if defined(__DragonFly__) || __FreeBSD_version < 500000 || defined(__NetBSD__)
#define FWOHCI_TASKQUEUE        0
#else
#define FWOHCI_TASKQUEUE        1
#endif
#if FWOHCI_TASKQUEUE
#include <sys/taskqueue.h>
#endif

#if defined(__NetBSD__)
MALLOC_DECLARE(M_FW);
#endif

typedef struct fwohci_softc {
	struct firewire_comm fc;
	bus_space_tag_t bst;
	bus_space_handle_t bsh;
#if defined(__FreeBSD__)
	void *ih;
#if defined(__DragonFly__) || __FreeBSD_version < 500000
	void *ih_cam;
	void *ih_bio;
#endif
	struct resource *bsr;
	struct resource *irq_res;
#elif defined(__NetBSD__)
	bus_size_t bssize;
	void *sc_shutdownhook;
	void *sc_powerhook;
#endif
	struct fwohci_dbch{
		u_int ndb;
		u_int ndesc;
		STAILQ_HEAD(, fwohcidb_tr) db_trq;
		struct fwohcidb_tr *top, *bottom, *pdb_tr;
		struct fw_xferq xferq;
		int flags;
#define	FWOHCI_DBCH_INIT	(1<<0)
#define	FWOHCI_DBCH_FULL	(1<<1)
		/* used only in receive context */
		int buf_offset;	/* signed */
#define FWOHCI_DBCH_MAX_PAGES	32
		/* Context programs buffer */
		struct fwdma_alloc_multi *am;
		fw_bus_dma_tag_t dmat;
	} arrq, arrs, atrq, atrs, it[OHCI_DMA_ITCH], ir[OHCI_DMA_IRCH];
	u_int maxrec;
	uint32_t *sid_buf;
	struct fwdma_alloc sid_dma;
	struct fwdma_alloc crom_dma;
	struct fwdma_alloc dummy_dma;
	uint32_t intmask, irstat, itstat;
#if FWOHCI_TASKQUEUE
	uint32_t intstat;
	struct task fwohci_task_complete;
#endif
	int cycle_lost;
} fwohci_softc_t;

FW_INTR(fwohci);
int fwohci_init (struct fwohci_softc *, device_t);
void fwohci_poll (struct firewire_comm *, int, int);
void fwohci_reset (struct fwohci_softc *, device_t);
FWOHCI_DETACH();
int fwohci_resume (struct fwohci_softc *, device_t);
FWOHCI_STOP();
