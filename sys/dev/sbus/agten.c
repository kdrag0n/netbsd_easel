/*	$NetBSD: agten.c,v 1.2 2007/08/26 07:24:28 macallan Exp $ */

/*-
 * Copyright (c) 2007 Michael Lorenz
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
 * 3. Neither the name of The NetBSD Foundation nor the names of its
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: agten.c,v 1.2 2007/08/26 07:24:28 macallan Exp $");

/*
 * a driver for the Fujitsu AG-10e SBus framebuffer
 *
 * this thing is Frankenstein's Monster among graphics boards.
 * it contains three graphics chips:
 * a GLint - 24bit stuff, double-buffered
 * an Imagine 128 which provides an 8bit overlay
 * a Weitek P9100 which provides WIDs
 * so here we need to mess only with the P9100 and the I128 - for X we just
 * hide the overlay and let the Xserver mess with the GLint
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/proc.h>
#include <sys/mutex.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/malloc.h>

#include <dev/sun/fbio.h>
#include <dev/sun/fbvar.h>
#include <dev/sun/btreg.h>
#include <dev/sun/btvar.h>

#include <machine/bus.h>
#include <machine/autoconf.h>

#include <dev/sbus/sbusvar.h>

#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wsdisplayvar.h>
#include <dev/rasops/rasops.h>
#include <dev/wsfont/wsfont.h>

#include <dev/wscons/wsdisplay_vconsvar.h>

#include <dev/sbus/p9100reg.h>
#include <dev/ic/ibm561reg.h>
#include <dev/ic/i128reg.h>
#include <dev/ic/i128var.h>

#include "opt_agten.h"

static int	agten_match(struct device *, struct cfdata *, void *);
static void	agten_attach(struct device *, struct device *, void *);

#if 0
static void	agten_unblank(struct device *);
#endif

static int	agten_ioctl(void *, void *, u_long, void *, int, struct lwp *);
static paddr_t	agten_mmap(void *, void *, off_t, int);
static void	agten_init_screen(void *, struct vcons_screen *, int, long *);

struct agten_softc {
	struct device	sc_dev;		/* base device */
	struct sbusdev	sc_sd;		/* sbus device */
	struct fbdevice	sc_fb;		/* frame buffer device */

	struct vcons_screen sc_console_screen;
	struct wsscreen_descr sc_defaultscreen_descr;
	const struct wsscreen_descr *sc_screens[1];
	struct wsscreen_list sc_screenlist;

	bus_space_tag_t	sc_bustag;

	bus_space_handle_t 	sc_i128_fbh;
	bus_size_t		sc_i128_fbsz;
	bus_space_handle_t 	sc_i128_regh;
	bus_space_handle_t 	sc_p9100_regh;

	uint32_t	sc_width;
	uint32_t	sc_height;	/* panel width / height */
	uint32_t	sc_stride;
	uint32_t	sc_depth;
	
	union	bt_cmap sc_cmap;	/* Brooktree color map */

	int sc_mode;
	int sc_video, sc_powerstate;
	uint32_t sc_bg;
	struct vcons_data vd;
};

CFATTACH_DECL(agten, sizeof(struct agten_softc),
    agten_match, agten_attach, NULL, NULL);


static int	agten_putcmap(struct agten_softc *, struct wsdisplay_cmap *);
static int 	agten_getcmap(struct agten_softc *, struct wsdisplay_cmap *);
static int 	agten_putpalreg(struct agten_softc *, uint8_t, uint8_t,
			    uint8_t, uint8_t);
static void	agten_init(struct agten_softc *);

static void	agten_copycols(void *, int, int, int, int);
static void	agten_erasecols(void *, int, int, int, long);
static void	agten_copyrows(void *, int, int, int);
static void	agten_eraserows(void *, int, int, long);

extern const u_char rasops_cmap[768];

struct wsdisplay_accessops agten_accessops = {
	agten_ioctl,
	agten_mmap,
	NULL,	/* alloc_screen */
	NULL,	/* free_screen */
	NULL,	/* show_screen */
	NULL, 	/* load_font */
	NULL,	/* pollc */
	NULL	/* scroll */
};

static inline void
agten_write_dac(struct agten_softc *sc, int reg, uint8_t val)
{
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh,
	    0x200 + (reg << 2), (uint32_t)val << 16);
}

static inline void
agten_write_idx(struct agten_softc *sc, int offset)
{
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh,
	    0x200 + (IBM561_ADDR_LOW << 2), (offset & 0xff) << 16);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh,
	    0x200 + (IBM561_ADDR_HIGH << 2), ((offset >> 8) & 0xff) << 16);
}

static inline void
agten_write_dac_10(struct agten_softc *sc, int reg, uint16_t val)
{
	agten_write_dac(sc, reg, (val >> 2) & 0xff);
	agten_write_dac(sc, reg, (val & 0x3) << 6);
}
	
static int
agten_match(struct device *dev, struct cfdata *cf, void *aux)
{
	struct sbus_attach_args *sa = aux;

	if (strcmp("PFU,aga", sa->sa_name) == 0)
		return 100;
	return 0;
}

static void
agten_attach(struct device *parent, struct device *dev, void *aux)
{
	struct agten_softc *sc = (struct agten_softc *)dev;
	struct sbus_attach_args *sa = aux;
	struct fbdevice *fb = &sc->sc_fb;
	struct wsemuldisplaydev_attach_args aa;
	struct rasops_info *ri;
	long defattr;
	uint32_t reg;
	int node = sa->sa_node;
	int console;
 
	sc->sc_defaultscreen_descr = (struct wsscreen_descr){
		"default",
		0, 0,
		NULL,
		8, 16,
		WSSCREEN_WSCOLORS | WSSCREEN_HILIT,
		NULL
	};
	sc->sc_screens[0] = &sc->sc_defaultscreen_descr;
	sc->sc_screenlist = (struct wsscreen_list){1, sc->sc_screens};
	sc->sc_mode = WSDISPLAYIO_MODE_EMUL;
	sc->sc_bustag = sa->sa_bustag;
#if 0
	sc->sc_shadowfb = malloc(sc->sc_fbsize, M_DEVBUF, M_WAITOK);

	dict = device_properties(&sc->sc_dev);

	prop_dictionary_get_bool(dict, "is_console", &console);
#endif

	reg = prom_getpropint(node, "i128_fb_physaddr", -1);
	sc->sc_i128_fbsz = prom_getpropint(node, "i128_fb_size", -1);
	if (sbus_bus_map(sc->sc_bustag,
	    sa->sa_reg[0].oa_space, sa->sa_reg[0].oa_base + reg,
	    sc->sc_i128_fbsz, BUS_SPACE_MAP_LINEAR, &sc->sc_i128_fbh) != 0) {

		aprint_error("%s: unable to map the framebuffer\n",
		    dev->dv_xname);
		return;
	}
	fb->fb_pixels = bus_space_vaddr(sc->sc_bustag, sc->sc_i128_fbh);

	reg = prom_getpropint(node, "p9100_reg_physaddr", -1);
	if (sbus_bus_map(sc->sc_bustag,
	    sa->sa_reg[0].oa_space, sa->sa_reg[0].oa_base + reg,
	    0x8000, 0, &sc->sc_p9100_regh) != 0) {

		aprint_error("%s: unable to map P9100 registers\n",
		    dev->dv_xname);
		return;
	}

	reg = prom_getpropint(node, "i128_reg_physaddr", -1);
	if (sbus_bus_map(sc->sc_bustag,
	    sa->sa_reg[0].oa_space, sa->sa_reg[0].oa_base + reg,
	    0x10000, 0, &sc->sc_i128_regh) != 0) {

		aprint_error("%s: unable to map I128 registers\n",
		    dev->dv_xname);
		return;
	}

	sbus_establish(&sc->sc_sd, &sc->sc_dev);
#if 0
	bus_intr_establish(sc->sc_bustag, sa->sa_pri, IPL_BIO,
	    agten_intr, sc);
#endif

	sc->sc_width = prom_getpropint(node, "ffb_width", 800);
	sc->sc_height = prom_getpropint(node, "ffb_height", 600);
	sc->sc_depth = prom_getpropint(node, "ffb_depth", 8);
	sc->sc_stride = sc->sc_width * (sc->sc_depth >> 3);

	agten_init(sc);

	console = fb_is_console(node);

	vcons_init(&sc->vd, sc, &sc->sc_defaultscreen_descr,
	    &agten_accessops);
	sc->vd.init_screen = agten_init_screen;

	ri = &sc->sc_console_screen.scr_ri;

	if (console) {
		vcons_init_screen(&sc->vd, &sc->sc_console_screen, 1,
		    &defattr);
		sc->sc_console_screen.scr_flags |= VCONS_SCREEN_IS_STATIC;

		sc->sc_defaultscreen_descr.textops = &ri->ri_ops;
		sc->sc_defaultscreen_descr.capabilities = ri->ri_caps;
		sc->sc_defaultscreen_descr.nrows = ri->ri_rows;
		sc->sc_defaultscreen_descr.ncols = ri->ri_cols;
		wsdisplay_cnattach(&sc->sc_defaultscreen_descr, ri, 0, 0,
		    defattr);
		i128_rectfill(sc->sc_bustag, sc->sc_i128_regh, 0, 0,
		    sc->sc_width, sc->sc_height,
		    ri->ri_devcmap[(defattr >> 16) & 0xff]);
	} else {
		/*
		 * since we're not the console we can postpone the rest
		 * until someone actually allocates a screen for us
		 */
	}

	/* Initialize the default color map. */

	aa.console = console;
	aa.scrdata = &sc->sc_screenlist;
	aa.accessops = &agten_accessops;
	aa.accesscookie = &sc->vd;

	config_found(&sc->sc_dev, &aa, wsemuldisplaydevprint);
}

static int
agten_ioctl(void *v, void *vs, u_long cmd, void *data, int flag,
	struct lwp *l)
{
	struct vcons_data *vd = v;
	struct agten_softc *sc = vd->cookie;
	struct wsdisplay_fbinfo *wdf;
	struct vcons_screen *ms = vd->active;

	switch (cmd) {

		case WSDISPLAYIO_GINFO:
			if (ms == NULL)
				return ENODEV;
			wdf = (void *)data;
			wdf->height = ms->scr_ri.ri_height;
			wdf->width = ms->scr_ri.ri_width;
			wdf->depth = ms->scr_ri.ri_depth;
			wdf->cmsize = 256;
			return 0;

		case WSDISPLAYIO_GETCMAP:
			return agten_getcmap(sc,
			    (struct wsdisplay_cmap *)data);

		case WSDISPLAYIO_PUTCMAP:
			return agten_putcmap(sc,
			    (struct wsdisplay_cmap *)data);

		case WSDISPLAYIO_LINEBYTES:
			*(u_int *)data = sc->sc_stride;
			return 0;

		case WSDISPLAYIO_SMODE:
			{
				int new_mode = *(int*)data;
				if (new_mode != sc->sc_mode) {
					sc->sc_mode = new_mode;
					if(new_mode == WSDISPLAYIO_MODE_EMUL) {
						agten_init(sc);
						vcons_redraw_screen(ms);
					}
				}
			}
			return 0;
	}
	return EPASSTHROUGH;
}

static paddr_t
agten_mmap(void *v, void *vs, off_t offset, int prot)
{
	struct vcons_data *vd = v;
	struct agten_softc *sc = vd->cookie;

	if (offset < sc->sc_i128_fbsz)
		return bus_space_mmap(sc->sc_bustag, sc->sc_i128_fbh, offset,
		    prot, BUS_SPACE_MAP_LINEAR);
	return -1;
}

static void
agten_init_screen(void *cookie, struct vcons_screen *scr,
    int existing, long *defattr)
{
	struct agten_softc *sc = cookie;
	struct rasops_info *ri = &scr->scr_ri;

	ri->ri_depth = sc->sc_depth;
	ri->ri_width = sc->sc_width;
	ri->ri_height = sc->sc_height;
	ri->ri_stride = sc->sc_stride;
	ri->ri_flg = RI_CENTER | RI_FULLCLEAR;

	ri->ri_bits = (char *)sc->sc_fb.fb_pixels;

	if (existing) {
		ri->ri_flg |= RI_CLEAR;
	}

	rasops_init(ri, sc->sc_height / 8, sc->sc_width / 8);
	ri->ri_caps = WSSCREEN_WSCOLORS;

	rasops_reconfig(ri, sc->sc_height / ri->ri_font->fontheight,
		    sc->sc_width / ri->ri_font->fontwidth);

	ri->ri_hw = scr;
	ri->ri_ops.copyrows  = agten_copyrows;
	ri->ri_ops.eraserows = agten_eraserows;
	ri->ri_ops.copycols  = agten_copycols;
	ri->ri_ops.erasecols = agten_erasecols;

}

static int
agten_putcmap(struct agten_softc *sc, struct wsdisplay_cmap *cm)
{
	u_int index = cm->index;
	u_int count = cm->count;
	int i, error;
	u_char rbuf[256], gbuf[256], bbuf[256];
	u_char *r, *g, *b;

	if (cm->index >= 256 || cm->count > 256 ||
	    (cm->index + cm->count) > 256)
		return EINVAL;
	error = copyin(cm->red, &rbuf[index], count);
	if (error)
		return error;
	error = copyin(cm->green, &gbuf[index], count);
	if (error)
		return error;
	error = copyin(cm->blue, &bbuf[index], count);
	if (error)
		return error;

	r = &rbuf[index];
	g = &gbuf[index];
	b = &bbuf[index];

	for (i = 0; i < count; i++) {
		agten_putpalreg(sc, index, *r, *g, *b);
		index++;
		r++, g++, b++;
	}
	return 0;
}

static int
agten_getcmap(struct agten_softc *sc, struct wsdisplay_cmap *cm)
{
	u_int index = cm->index;
	u_int count = cm->count;
	int error, i;
	uint8_t red[256], green[256], blue[256];

	if (index >= 255 || count > 256 || index + count > 256)
		return EINVAL;

	i = index;
	while (i < (index + count)) {
		red[i] = sc->sc_cmap.cm_map[i][0];
		green[i] = sc->sc_cmap.cm_map[i][1];
		blue[i] = sc->sc_cmap.cm_map[i][2];
		i++;
	}
	error = copyout(&red[index],   cm->red,   count);
	if (error)
		return error;
	error = copyout(&green[index], cm->green, count);
	if (error)
		return error;
	error = copyout(&blue[index],  cm->blue,  count);
	if (error)
		return error;

	return 0;
}

static int
agten_putpalreg(struct agten_softc *sc, uint8_t idx, uint8_t r, uint8_t g,
    uint8_t b)
{

	sc->sc_cmap.cm_map[idx][0] = r;
	sc->sc_cmap.cm_map[idx][1] = g;
	sc->sc_cmap.cm_map[idx][2] = b;
	agten_write_idx(sc, IBM561_CMAP_TABLE + idx);
	agten_write_dac(sc, IBM561_CMD_CMAP, r);
	agten_write_dac(sc, IBM561_CMD_CMAP, g);
	agten_write_dac(sc, IBM561_CMD_CMAP, b);
	return 0;
}

static void
agten_init(struct agten_softc *sc)
{
	int i, j;
	uint32_t src, srcw;
	volatile uint32_t junk;

	/* first we set up the colour map */
	j = 0;
	for (i = 0; i < 256; i++) {

		agten_putpalreg(sc, i, rasops_cmap[j], rasops_cmap[j + 1],
		    rasops_cmap[j + 2]);
		j += 3;
	}

	/* now set up some window attributes */
	agten_write_idx(sc, IBM561_OL_WINTYPE);
	agten_write_dac_10(sc, IBM561_CMD_FB_WAT, 0x00);	
	for (i = 1; i < 256; i++)
		agten_write_dac_10(sc, IBM561_CMD_FB_WAT, 0x01);

	src = 0;
	srcw = sc->sc_width << 16 | sc->sc_height;
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, FOREGROUND_COLOR,
	    0x0);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, BACKGROUND_COLOR,
	    0x0);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, RASTER_OP, ROP_PAT);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, COORD_INDEX, 0);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, RECT_RTW_XY, src);
	bus_space_write_4(sc->sc_bustag, sc->sc_p9100_regh, RECT_RTW_XY, srcw);
	junk = bus_space_read_4(sc->sc_bustag, sc->sc_p9100_regh, COMMAND_QUAD);

	i128_init(sc->sc_bustag, sc->sc_i128_regh, sc->sc_stride, 8);
}

static void
agten_copycols(void *cookie, int row, int srccol, int dstcol, int ncols)
{
	struct rasops_info *ri = cookie;
	struct vcons_screen *scr = ri->ri_hw;
	struct agten_softc *sc = scr->scr_cookie;
	int32_t xs, xd, y, width, height;

	xs = ri->ri_xorigin + ri->ri_font->fontwidth * srccol;
	xd = ri->ri_xorigin + ri->ri_font->fontwidth * dstcol;
	y = ri->ri_yorigin + ri->ri_font->fontheight * row;
	width = ri->ri_font->fontwidth * ncols;
	height = ri->ri_font->fontheight;
	i128_bitblt(sc->sc_bustag, sc->sc_i128_regh, xs, y, xd, y, width,
	    height, CR_COPY);
}

static void
agten_erasecols(void *cookie, int row, int startcol, int ncols, long fillattr)
{
	struct rasops_info *ri = cookie;
	struct vcons_screen *scr = ri->ri_hw;
	struct agten_softc *sc = scr->scr_cookie;
	int32_t x, y, width, height, bg;

	x = ri->ri_xorigin + ri->ri_font->fontwidth * startcol;
	y = ri->ri_yorigin + ri->ri_font->fontheight * row;
	width = ri->ri_font->fontwidth * ncols;
	height = ri->ri_font->fontheight;
	bg = (uint32_t)ri->ri_devcmap[(fillattr >> 16) & 0xff];
	i128_rectfill(sc->sc_bustag, sc->sc_i128_regh, x, y, width, height, bg);
}

static void
agten_copyrows(void *cookie, int srcrow, int dstrow, int nrows)
{
	struct rasops_info *ri = cookie;
	struct vcons_screen *scr = ri->ri_hw;
	struct agten_softc *sc = scr->scr_cookie;
	int32_t x, ys, yd, width, height;

	x = ri->ri_xorigin;
	ys = ri->ri_yorigin + ri->ri_font->fontheight * srcrow;
	yd = ri->ri_yorigin + ri->ri_font->fontheight * dstrow;
	width = ri->ri_emuwidth;
	height = ri->ri_font->fontheight * nrows;
	i128_bitblt(sc->sc_bustag, sc->sc_i128_regh, x, ys, x, yd, width,
	    height, CR_COPY);
}

static void
agten_eraserows(void *cookie, int row, int nrows, long fillattr)
{
	struct rasops_info *ri = cookie;
	struct vcons_screen *scr = ri->ri_hw;
	struct agten_softc *sc = scr->scr_cookie;
	int32_t x, y, width, height, bg;

	if ((row == 0) && (nrows == ri->ri_rows)) {
		x = y = 0;
		width = ri->ri_width;
		height = ri->ri_height;
	} else {
		x = ri->ri_xorigin;
		y = ri->ri_yorigin + ri->ri_font->fontheight * row;
		width = ri->ri_emuwidth;
		height = ri->ri_font->fontheight * nrows;
	}
	bg = (uint32_t)ri->ri_devcmap[(fillattr >> 16) & 0xff];
	i128_rectfill(sc->sc_bustag, sc->sc_i128_regh, x, y, width, height, bg);
}
