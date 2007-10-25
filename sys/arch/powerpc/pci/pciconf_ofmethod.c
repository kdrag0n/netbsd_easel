/* $NetBSD: pciconf_ofmethod.c,v 1.1 2007/10/25 16:55:51 garbled Exp $ */

/*-
 * Copyright (c) 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jochen Kunz and Tim Rightnour
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generic PowerPC functions for talking to a PCI Bridge via the RTAS or
 * OF methods of the device.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: pciconf_ofmethod.c,v 1.1 2007/10/25 16:55:51 garbled Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/device.h>

#include <uvm/uvm_extern.h>

#define _POWERPC_BUS_DMA_PRIVATE
#include <machine/bus.h>
#include <machine/intr.h>
#include <machine/pio.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_pci.h>

#if NISA > 0
#include <dev/isa/isavar.h>
#endif

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pcidevs.h>

void
genppc_pci_ofmethod_attach_hook(struct device *parent, struct device *self,
    struct pcibus_attach_args *pba)
{

	if (pba->pba_bus != 0)
		return;

	aprint_normal(": OFW method configuration space access");
}

pcitag_t
genppc_pci_ofmethod_make_tag(void *v, int bus, int dev, int func)
{
	return ((bus << OFW_PCI_PHYS_HI_BUSSHIFT) & OFW_PCI_PHYS_HI_BUSMASK) |
	    ((dev << OFW_PCI_PHYS_HI_DEVICESHIFT) & OFW_PCI_PHYS_HI_DEVICEMASK)|
	    ((func << OFW_PCI_PHYS_HI_FUNCTIONSHIFT) &
	    OFW_PCI_PHYS_HI_FUNCTIONMASK);
}

void
genppc_pci_ofmethod_decompose_tag(void *v, pcitag_t tag, int *bp, int *dp,
    int *fp)
{

	if (bp != NULL)
		*bp = OFW_PCI_PHYS_HI_BUS(tag);
	if (dp != NULL)
		*dp = OFW_PCI_PHYS_HI_DEVICE(tag);
	if (fp != NULL)
		*fp = OFW_PCI_PHYS_HI_FUNCTION(tag);
	return;
}

pcireg_t
genppc_pci_ofmethod_conf_read(void *v, pcitag_t tag, int reg)
{
	pci_chipset_tag_t pc = (pci_chipset_tag_t)v;
	pcireg_t data;

	tag &= OFW_PCI_PHYS_HI_BUSMASK | OFW_PCI_PHYS_HI_DEVICEMASK |
	    OFW_PCI_PHYS_HI_FUNCTIONMASK;
	tag |= reg & OFW_PCI_PHYS_HI_REGISTERMASK;
	if (OF_call_method("config-l@", pc->pc_ihandle, 1, 1, tag, &data) < 0)
		return (pcireg_t) -1;
	/*printf("data=0x%x\n", data); */
	return data;
}

void
genppc_pci_ofmethod_conf_write(void *v, pcitag_t tag, int reg, pcireg_t data)
{
	pci_chipset_tag_t pc = (pci_chipset_tag_t)v;

	tag &= OFW_PCI_PHYS_HI_BUSMASK | OFW_PCI_PHYS_HI_DEVICEMASK |
	    OFW_PCI_PHYS_HI_FUNCTIONMASK;
	tag |= reg & OFW_PCI_PHYS_HI_REGISTERMASK;
	OF_call_method("config-l!", pc->pc_ihandle, 2, 0, data, tag);
}
