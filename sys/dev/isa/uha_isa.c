#include <sys/types.h>
#include <sys/param.h>
#include <sys/device.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/user.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <scsi/scsi_all.h>
#include <scsi/scsiconf.h>

#include <dev/isa/isavar.h>
#include <dev/isa/isadmavar.h>

#include <dev/ic/uhareg.h>
#include <dev/ic/uhavar.h>

#define	UHA_ISA_IOSIZE	16

int	uha_isa_probe __P((struct device *, void *, void *));
void	uha_isa_attach __P((struct device *, struct device *, void *));

struct cfattach uha_isa_ca = {
	sizeof(struct uha_softc), uha_isa_probe, uha_isa_attach
};

#define KVTOPHYS(x)	vtophys(x)

int u14_find __P((bus_chipset_tag_t, bus_io_handle_t, struct uha_softc *));
void u14_start_mbox __P((struct uha_softc *, struct uha_mscp *));
int u14_poll __P((struct uha_softc *, struct scsi_xfer *, int));
int u14_intr __P((void *));
void u14_init __P((struct uha_softc *));

/*
 * Check the slots looking for a board we recognise
 * If we find one, note it's address (slot) and call
 * the actual probe routine to check it out.
 */
int
uha_isa_probe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct isa_attach_args *ia = aux;
	struct uha_softc sc;
	bus_chipset_tag_t bc = ia->ia_bc;
	bus_io_handle_t ioh;
	isa_chipset_tag_t ic = ia->ia_ic;
	int rv;

	if (bus_io_map(bc, ia->ia_iobase, UHA_ISA_IOSIZE, &ioh))
		return (0);

	rv = u14_find(bc, ioh, &sc);

	bus_io_unmap(bc, ioh, UHA_ISA_IOSIZE);

	if (rv) {
		if (ia->ia_irq != -1 && ia->ia_irq != sc.sc_irq)
			return (0);
		if (ia->ia_drq != -1 && ia->ia_drq != sc.sc_drq)
			return (0);
		ia->ia_irq = sc.sc_irq;
		ia->ia_drq = sc.sc_drq;
		ia->ia_msize = 0;
		ia->ia_iosize = UHA_ISA_IOSIZE;
	}
	return (rv);
}

/*
 * Attach all the sub-devices we can find
 */
void
uha_isa_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct isa_attach_args *ia = aux;
	struct uha_softc *sc = (void *)self;
	bus_chipset_tag_t bc = ia->ia_bc;
	bus_io_handle_t ioh;
	isa_chipset_tag_t ic = ia->ia_ic;

	printf("\n");

	if (bus_io_map(bc, ia->ia_iobase, UHA_ISA_IOSIZE, &ioh))
		panic("uha_attach: bus_io_map failed!");

	sc->sc_bc = bc;
	sc->sc_ioh = ioh;
	if (!u14_find(bc, ioh, sc))
		panic("uha_attach: u14_find failed!");

	if (sc->sc_drq != -1)
		isa_dmacascade(sc->sc_drq);

	sc->sc_ih = isa_intr_establish(ic, sc->sc_irq, IST_EDGE, IPL_BIO,
	    u14_intr, sc);
	if (sc->sc_ih == NULL) {
		printf("%s: couldn't establish interrupt\n",
		    sc->sc_dev.dv_xname);
		return;
	}

	/* Save function pointers for later use. */
	sc->start_mbox = u14_start_mbox;
	sc->poll = u14_poll;
	sc->init = u14_init;

	uha_attach(sc);
}

/*
 * Start the board, ready for normal operation
 */
int
u14_find(bc, ioh, sc)
	bus_chipset_tag_t bc;
	bus_io_handle_t ioh;
	struct uha_softc *sc;
{
	u_int16_t model, config;
	int irq, drq;
	int resetcount = 4000;	/* 4 secs? */

	model = (bus_io_read_1(bc, ioh, U14_ID + 0) << 8) |
		(bus_io_read_1(bc, ioh, U14_ID + 1) << 0);
	if ((model & 0xfff0) != 0x5640)
		return (0);

	config = (bus_io_read_1(bc, ioh, U14_CONFIG + 0) << 8) |
		 (bus_io_read_1(bc, ioh, U14_CONFIG + 1) << 0);

	switch (model & 0x000f) {
	case 0x0000:
		switch (config & U14_DMA_MASK) {
		case U14_DMA_CH5:
			drq = 5;
			break;
		case U14_DMA_CH6:
			drq = 6;
			break;
		case U14_DMA_CH7:
			drq = 7;
			break;
		default:
			printf("u14_find: illegal drq setting %x\n",
			    config & U14_DMA_MASK);
			return (0);
		}
		break;
	case 0x0001:
		/* This is a 34f, and doesn't need an ISA DMA channel. */
		drq = -1;
		break;
	}

	switch (config & U14_IRQ_MASK) {
	case U14_IRQ10:
		irq = 10;
		break;
	case U14_IRQ11:
		irq = 11;
		break;
	case U14_IRQ14:
		irq = 14;
		break;
	case U14_IRQ15:
		irq = 15;
		break;
	default:
		printf("u14_find: illegal irq setting %x\n",
		    config & U14_IRQ_MASK);
		return (0);
	}

	bus_io_write_1(bc, ioh, U14_LINT, UHA_ASRST);

	while (--resetcount) {
		if (bus_io_read_1(bc, ioh, U14_LINT))
			break;
		delay(1000);	/* 1 mSec per loop */
	}
	if (!resetcount) {
		printf("u14_find: board timed out during reset\n");
		return (0);
	}

	/* if we want to fill in softc, do so now */
	if (sc != NULL) {
		sc->sc_irq = irq;
		sc->sc_drq = drq;
		sc->sc_scsi_dev = config & U14_HOSTID_MASK;
	}

	return (1);
}

/*
 * Function to send a command out through a mailbox
 */
void
u14_start_mbox(sc, mscp)
	struct uha_softc *sc;
	struct uha_mscp *mscp;
{
	bus_chipset_tag_t bc = sc->sc_bc;
	bus_io_handle_t ioh = sc->sc_ioh;
	int spincount = 100000;	/* 1s should be enough */

	while (--spincount) {
		if ((bus_io_read_1(bc, ioh, U14_LINT) & U14_LDIP) == 0)
			break;
		delay(100);
	}
	if (!spincount) {
		printf("%s: uha_start_mbox, board not responding\n",
		    sc->sc_dev.dv_xname);
		Debugger();
	}

	bus_io_write_4(bc, ioh, U14_OGMPTR, KVTOPHYS(mscp));
	if (mscp->flags & MSCP_ABORT)
		bus_io_write_1(bc, ioh, U14_LINT, U14_ABORT);
	else
		bus_io_write_1(bc, ioh, U14_LINT, U14_OGMFULL);

	if ((mscp->xs->flags & SCSI_POLL) == 0)
		timeout(uha_timeout, mscp, (mscp->timeout * hz) / 1000);
}

/*
 * Function to poll for command completion when in poll mode.
 *
 *	wait = timeout in msec
 */
int
u14_poll(sc, xs, count)
	struct uha_softc *sc;
	struct scsi_xfer *xs;
	int count;
{
	bus_chipset_tag_t bc = sc->sc_bc;
	bus_io_handle_t ioh = sc->sc_ioh;

	while (count) {
		/*
		 * If we had interrupts enabled, would we
		 * have got an interrupt?
		 */
		if (bus_io_read_1(bc, ioh, U14_SINT) & U14_SDIP)
			u14_intr(sc);
		if (xs->flags & ITSDONE)
			return (0);
		delay(1000);
		count--;
	}
	return (1);
}

/*
 * Catch an interrupt from the adaptor
 */
int
u14_intr(arg)
	void *arg;
{
	struct uha_softc *sc = arg;
	bus_chipset_tag_t bc = sc->sc_bc;
	bus_io_handle_t ioh = sc->sc_ioh;
	struct uha_mscp *mscp;
	u_char uhastat;
	u_long mboxval;

#ifdef	UHADEBUG
	printf("%s: uhaintr ", sc->sc_dev.dv_xname);
#endif /*UHADEBUG */

	if ((bus_io_read_1(bc, ioh, U14_SINT) & U14_SDIP) == 0)
		return (0);

	for (;;) {
		/*
		 * First get all the information and then
		 * acknowledge the interrupt
		 */
		uhastat = bus_io_read_1(bc, ioh, U14_SINT);
		mboxval = bus_io_read_4(bc, ioh, U14_ICMPTR);
		/* XXX Send an ABORT_ACK instead? */
		bus_io_write_1(bc, ioh, U14_SINT, U14_ICM_ACK);

#ifdef	UHADEBUG
		printf("status = 0x%x ", uhastat);
#endif /*UHADEBUG*/

		/*
		 * Process the completed operation
		 */
		mscp = uha_mscp_phys_kv(sc, mboxval);
		if (!mscp) {
			printf("%s: BAD MSCP RETURNED!\n",
			    sc->sc_dev.dv_xname);
			continue;	/* whatever it was, it'll timeout */
		}

		untimeout(uha_timeout, mscp);
		uha_done(sc, mscp);

		if ((bus_io_read_1(bc, ioh, U14_SINT) & U14_SDIP) == 0)
			return (1);
	}
}

void
u14_init(sc)
	struct uha_softc *sc;
{
	bus_chipset_tag_t bc = sc->sc_bc;
	bus_io_handle_t ioh = sc->sc_ioh;

	/* make sure interrupts are enabled */
#ifdef UHADEBUG
	printf("u14_init: lmask=%02x, smask=%02x\n",
	    bus_io_read_1(bc, ioh, U14_LMASK),
	    bus_io_read_1(bc, ioh, U14_SMASK));
#endif
	bus_io_write_1(bc, ioh, U14_LMASK, 0xd1);	/* XXX */
	bus_io_write_1(bc, ioh, U14_SMASK, 0x91);	/* XXX */
}
