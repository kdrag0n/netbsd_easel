/*	$NetBSD: grf_subr.c,v 1.2 1996/08/04 06:03:56 scottr Exp $	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/device.h>
#include <sys/systm.h>

#include <machine/grfioctl.h>

#include <mac68k/dev/nubus.h>
#include <mac68k/dev/grfvar.h>

void
grf_establish(sc, sp, g_mode, g_phys)
	struct grfbus_softc *sc;
	nubus_slot *sp;
	int (*g_mode) __P((struct grf_softc *, int, void *));
	caddr_t (*g_phys) __P((struct grf_softc *, vm_offset_t));
{
	struct grfmode *gm = &sc->curr_mode;
	struct grfbus_attach_args ga;

	/* Print hardware characteristics. */
	printf("%s: %d x %d, ", sc->sc_dev.dv_xname, gm->width, gm->height);
	if (gm->psize == 1)
		printf("monochrome\n");
	else
		printf("%d color\n", 1 << gm->psize);

	/* Attach grf semantics to the hardware. */
	ga.ga_name = "grf";
	ga.ga_grfmode = gm;
	ga.ga_slot = sp;
	ga.ga_mode = g_mode;
	ga.ga_phys = g_phys;
	(void)config_found(&sc->sc_dev, &ga, grfbusprint);
}

int
grfbusprint(aux, name)
	void *aux;
	char *name;
{
	struct grfbus_attach_args *ga = aux;

	if (name)
		printf("%s at %s", ga->ga_name, name);

	return (UNCONF);
}
