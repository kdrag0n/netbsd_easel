/* $NetBSD: cia_pci.c,v 1.15 1997/09/15 22:35:54 thorpej Exp $ */

/*
 * Copyright (c) 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: cia_pci.c,v 1.15 1997/09/15 22:35:54 thorpej Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <vm/vm.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>
#include <alpha/pci/ciareg.h>
#include <alpha/pci/ciavar.h>

void		cia_attach_hook __P((struct device *, struct device *,
		    struct pcibus_attach_args *));
int		cia_bus_maxdevs __P((void *, int));
pcitag_t	cia_make_tag __P((void *, int, int, int));
void		cia_decompose_tag __P((void *, pcitag_t, int *, int *,
		    int *));
pcireg_t	cia_conf_read __P((void *, pcitag_t, int));
void		cia_conf_write __P((void *, pcitag_t, int, pcireg_t));

void
cia_pci_init(pc, v)
	pci_chipset_tag_t pc;
	void *v;
{

	pc->pc_conf_v = v;
	pc->pc_attach_hook = cia_attach_hook;
	pc->pc_bus_maxdevs = cia_bus_maxdevs;
	pc->pc_make_tag = cia_make_tag;
	pc->pc_decompose_tag = cia_decompose_tag;
	pc->pc_conf_read = cia_conf_read;
	pc->pc_conf_write = cia_conf_write;
}

void
cia_attach_hook(parent, self, pba)
	struct device *parent, *self;
	struct pcibus_attach_args *pba;
{
}

int
cia_bus_maxdevs(cpv, busno)
	void *cpv;
	int busno;
{

	return 32;
}

pcitag_t
cia_make_tag(cpv, b, d, f)
	void *cpv;
	int b, d, f;
{

	return (b << 16) | (d << 11) | (f << 8);
}

void
cia_decompose_tag(cpv, tag, bp, dp, fp)
	void *cpv;
	pcitag_t tag;
	int *bp, *dp, *fp;
{

	if (bp != NULL)
		*bp = (tag >> 16) & 0xff;
	if (dp != NULL)
		*dp = (tag >> 11) & 0x1f;
	if (fp != NULL)
		*fp = (tag >> 8) & 0x7;
}

pcireg_t
cia_conf_read(cpv, tag, offset)
	void *cpv;
	pcitag_t tag;
	int offset;
{
	struct cia_config *ccp = cpv;
	pcireg_t *datap, data;
	int s, secondary, ba;
	int32_t old_haxr2;					/* XXX */

#ifdef DIAGNOSTIC
	s = 0;					/* XXX gcc -Wuninitialized */
	old_haxr2 = 0;				/* XXX gcc -Wuninitialized */
#endif

	/*
	 * Some (apparently-common) revisions of EB164 and AlphaStation
	 * firmware do the Wrong thing with PCI master aborts, which are
	 * caused by accesing the configuration space of devices that
	 * don't exist (for example).
	 *
	 * To work around this, we clear the CIA error register's PCI
	 * master abort bit before touching PCI configuration space and
	 * check it afterwards.  If it indicates a master abort,
	 * the device wasn't there so we return 0xffffffff.
	 */
	/* clear the PCI master abort bit in CIA error register */
	REGVAL(CIA_CSR_CIA_ERR) = CIA_ERR_RCVD_MAS_ABT;
	alpha_mb();
	alpha_pal_draina();	

	/* secondary if bus # != 0 */
	alpha_pci_decompose_tag(&ccp->cc_pc, tag, &secondary, 0, 0);
	if (secondary) {
		s = splhigh();
		old_haxr2 = REGVAL(CIA_CSRS + 0x480);		/* XXX */
		alpha_mb();
		REGVAL(CIA_CSRS + 0x480) = old_haxr2 | 0x1;	/* XXX */
		alpha_mb();
	}

	datap = (pcireg_t *)ALPHA_PHYS_TO_K0SEG(CIA_PCI_CONF |
	    tag << 5UL |					/* XXX */
	    (offset & ~0x03) << 5 |				/* XXX */
	    0 << 5 |						/* XXX */
	    0x3 << 3);						/* XXX */
	data = (pcireg_t)-1;
	if (!(ba = badaddr(datap, sizeof *datap)))
		data = *datap;

	if (secondary) {
		alpha_mb();
		REGVAL(CIA_CSRS + 0x480) = old_haxr2;		/* XXX */
		alpha_mb();
		splx(s);
	}

	alpha_pal_draina();	
	/* check CIA error register for PCI master abort */
	if (REGVAL(CIA_CSR_CIA_ERR) & CIA_ERR_RCVD_MAS_ABT) {
		ba = 1;
		data = 0xffffffff;
	}

#if 0
	printf("cia_conf_read: tag 0x%lx, reg 0x%lx -> %x @ %p%s\n", tag, reg,
	    data, datap, ba ? " (badaddr)" : "");
#endif

	return data;
}

void
cia_conf_write(cpv, tag, offset, data)
	void *cpv;
	pcitag_t tag;
	int offset;
	pcireg_t data;
{
	struct cia_config *ccp = cpv;
	pcireg_t *datap;
	int s, secondary;
	int32_t old_haxr2;					/* XXX */

#ifdef DIAGNOSTIC
	s = 0;					/* XXX gcc -Wuninitialized */
	old_haxr2 = 0;				/* XXX gcc -Wuninitialized */
#endif

	/* secondary if bus # != 0 */
	alpha_pci_decompose_tag(&ccp->cc_pc, tag, &secondary, 0, 0);
	if (secondary) {
		s = splhigh();
		old_haxr2 = REGVAL(CIA_CSRS + 0x480);		/* XXX */
		alpha_mb();
		REGVAL(CIA_CSRS + 0x480) = old_haxr2 | 0x1;	/* XXX */
		alpha_mb();
	}

	datap = (pcireg_t *)ALPHA_PHYS_TO_K0SEG(CIA_PCI_CONF |
	    tag << 5UL |					/* XXX */
	    (offset & ~0x03) << 5 |				/* XXX */
	    0 << 5 |						/* XXX */
	    0x3 << 3);						/* XXX */
	*datap = data;

	if (secondary) {
		alpha_mb();
		REGVAL(CIA_CSRS + 0x480) = old_haxr2;		/* XXX */
		alpha_mb();
		splx(s);
	}

#if 0
	printf("cia_conf_write: tag 0x%lx, reg 0x%lx -> 0x%x @ %p\n", tag,
	    reg, data, datap);
#endif
}
