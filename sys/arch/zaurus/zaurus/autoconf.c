/*	$NetBSD: autoconf.c,v 1.7 2009/03/02 09:33:02 nonaka Exp $	*/

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
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
__KERNEL_RCSID(0, "$NetBSD: autoconf.c,v 1.7 2009/03/02 09:33:02 nonaka Exp $");

#include "opt_md.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/disklabel.h>
#include <sys/conf.h>
#include <sys/malloc.h>
#include <sys/vnode.h>
#include <sys/fcntl.h>
#include <sys/proc.h>
#include <sys/disk.h>
#include <sys/kauth.h>

#include <machine/intr.h>
#include <machine/bootconfig.h>
#include <machine/bootinfo.h>
#include <machine/config_hook.h>

static void handle_wedges(struct device *dv, int par);
static int is_valid_disk(struct device *dv);
static int match_bootdisk(struct device *dv, struct btinfo_bootdisk *bid);
static void findroot(void);

void
cpu_configure(void)
{

	splhigh();
	splserial();

	config_hook_init();

	if (config_rootfound("mainbus", NULL) == NULL)
		panic("no mainbus found");

	/* Configuration is finished, turn on interrupts. */
	spl0();
}

static void
handle_wedges(struct device *dv, int par)
{

	if (config_handle_wedges(dv, par) == 0)
		return;

	booted_device = dv;
	booted_partition = par;
}

static int
is_valid_disk(struct device *dv)
{

	if (device_class(dv) != DV_DISK)
		return 0;
	
	return (device_is_a(dv, "dk") ||
		device_is_a(dv, "sd") ||
		device_is_a(dv, "wd") ||
		device_is_a(dv, "ld"));
}

/*
 * Helper function for findroot():
 * Return non-zero if disk device matches bootinfo.
 */
static int
match_bootdisk(struct device *dv, struct btinfo_bootdisk *bid)
{
	struct vnode *tmpvn;
	int error;
	struct disklabel label;
	int found = 0;

	if (device_is_a(dv, "dk"))
		return 0;

	/*
	 * A disklabel is required here.  The boot loader doesn't refuse
	 * to boot from a disk without a label, but this is normally not
	 * wanted.
	 */
	if (bid->labelsector == -1)
		return 0;

	if ((tmpvn = opendisk(dv)) == NULL)
		return 0;

	error = VOP_IOCTL(tmpvn, DIOCGDINFO, &label, FREAD, NOCRED);
	if (error) {
		/*
		 * XXX Can't happen -- open() would have errored out
		 * or faked one up.
		 */
		printf("match_bootdisk: can't get label for dev %s (%d)\n",
		    device_xname(dv), error);
		goto closeout;
	}

	/* Compare with our data. */
	if (label.d_type == bid->label.type &&
	    label.d_checksum == bid->label.checksum &&
	    strncmp(label.d_packname, bid->label.packname, 16) == 0)
		found = 1;

 closeout:
	VOP_CLOSE(tmpvn, FREAD, NOCRED);
	vput(tmpvn);
	return found;
}

static void
findroot(void)
{
	struct btinfo_bootdisk *bid;
	device_t dv;

	if (booted_device)
		return;

	if ((bid = lookup_bootinfo(BTINFO_BOOTDISK)) != NULL) {
		/*
		 * Scan all disk devices for ones that match the passed data.
		 * Don't break if one is found, to get possible multiple
		 * matches - for problem tracking.  Use the first match anyway
		 * because lower device numbers are more likely to be the
		 * boot device.
		 */
		TAILQ_FOREACH(dv, &alldevs, dv_list) {
			if (device_class(dv) != DV_DISK)
				continue;

			if (is_valid_disk(dv)) {
				/*
				 * Don't trust BIOS device numbers, try
				 * to match the information passed by the
				 * boot loader instead.
				 */
				if ((bid->biosdev & 0x80) == 0 ||
				    match_bootdisk(dv, bid) == 0)
				    	continue;
				goto bootdisk_found;
			}
			continue;

 bootdisk_found:
			if (booted_device) {
				printf("WARNING: double match for boot "
				    "device (%s, %s)\n",
				    device_xname(booted_device),
				    device_xname(dv));
				continue;
			}
			handle_wedges(dv, bid->partition);
		}

		if (booted_device)
			return;
	}
}

void
cpu_rootconf(void)
{

	findroot();

	aprint_normal("boot device: %s\n",
	    booted_device ? device_xname(booted_device) : "<unknown>");
	setroot(booted_device, booted_partition);
}

void
device_register(struct device *dev, void *aux)
{

	/* Nothing to do */
}
