/*	$NetBSD: flash.h,v 1.2 2011/03/30 14:34:26 uebayasi Exp $	*/

/*-
 * Copyright (c) 2011 Department of Software Engineering,
 *		      University of Szeged, Hungary
 * Copyright (c) 2011 Adam Hoka <ahoka@NetBSD.org>
 * Copyright (c) 2010 David Tengeri <dtengeri@inf.u-szeged.hu>
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by the Department of Software Engineering, University of Szeged, Hungary
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _FLASH_H_
#define _FLASH_H_

#include <sys/param.h>
#include <sys/device.h>
#include <sys/module.h>
#include <sys/buf.h>
#include <sys/flashio.h>

/**
 *  flash_softc - private structure for flash layer driver
 */

struct flash_softc {
	device_t sc_dev;
	device_t sc_parent_dev;		/* Hardware (parent) device */
	void *hw_softc;			/* Hardware device private softc */
	struct flash_interface *flash_if;	/* Hardware interface */

	bool sc_readonly;		/* read only flash device */
};

struct flash_attach_args {
	struct flash_interface *flash_if;	/* Hardware interface */
};

device_t flash_attach_mi(struct flash_interface *, device_t);
const struct flash_interface *flash_get_interface(dev_t);
const struct flash_softc *flash_get_softc(dev_t);
device_t flash_get_device(dev_t);

/**
 * struct erase_instruction - instructions to erase a flash eraseblock
 * @fd: flash descriptor
 * @addr: start address of the erase operation
 * @len: the erase length
 * @callback: callback operation, called when erase finished
 * @priv: private data
 * @state: the erase operation's result
 */
struct flash_erase_instruction {
	flash_addr_t ei_addr;
	flash_addr_t ei_len;
	void (*ei_callback)(struct flash_erase_instruction *);
	u_long ei_priv;
	u_char ei_state;
};

enum {
	FLASH_PART_READONLY	= (1<<0),
	FLASH_PART_FILESYSTEM	= (1<<2)
};

struct flash_partition {
	flash_addr_t part_offset;
	flash_addr_t part_size;
	int part_flags;
};

/**
 * struct flash_interface - interface for flash operations
 * @type: type of flash device
 * @size: size of flash
 * @page_size: page size of flash
 * @erasesize: erase size of flash
 * @writesize: minimum write size of flash
 * @minor: minor number of the character device attached to this driver
 * @erase: erase operation of the flash
 * @read: read operation of the flash
 * @write: write operation of the flash
 * @block_markbad: marks a block as bad on the flash
 * @block_isbad: checks if a block is bad on flash
 */
struct flash_interface {
	int (*erase)(device_t, struct flash_erase_instruction *);
	int (*read)(device_t, off_t, size_t, size_t *, uint8_t *);
	int (*write)(device_t, off_t, size_t, size_t *, const uint8_t *);
	int (*block_markbad)(device_t, uint64_t);
	int (*block_isbad)(device_t, uint64_t);
	int (*sync)(device_t);

	int (*submit)(device_t, struct buf *);

	/* storage for partition info */
	struct flash_partition partition;

	/* total size of mtd */
	flash_addr_t size;	 
	uint32_t page_size;
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t minor;
	uint8_t	type;
};

/**
 * struct cache - for caching writes on block device
 */
struct flash_cache {
	size_t fc_len;
	flash_addr_t fc_block;
	uint8_t *fc_data;
};

/* flash operations should be used through these */
int flash_erase(device_t, struct flash_erase_instruction *);
int flash_read(device_t, off_t, size_t, size_t *, uint8_t *);
int flash_write(device_t, off_t, size_t, size_t *, const uint8_t *);
int flash_block_markbad(device_t, uint64_t);
int flash_block_isbad(device_t, uint64_t);
int flash_sync(device_t);

/*
 * check_pattern - checks the buffer only contains the byte pattern
 * @buf: the buffer to check
 * @patt: the pattern to match
 * @offset: the starting byte number, the matching starts from here
 * @size: the buffer size
 *
 * This functions checks if the buffer only contains a specified byte pattern.
 * Returns %0 if found something else, %1 otherwise.
 */
static inline int
check_pattern(const void *buf, uint8_t patt, size_t offset, size_t size)
{
	size_t i;
	for (i = offset; i < size; i++) {
		if (((const uint8_t *)buf)[i] != patt)
			return 0;
	}
	return 1;
}

#endif /* _FLASH_H_ */
