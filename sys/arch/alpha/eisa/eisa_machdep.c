/* $NetBSD: eisa_machdep.c,v 1.3 2000/08/11 00:43:20 thorpej Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#include <sys/cdefs.h>

__KERNEL_RCSID(0, "$NetBSD: eisa_machdep.c,v 1.3 2000/08/11 00:43:20 thorpej Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/queue.h>

#include <machine/intr.h>
#include <machine/rpb.h>

#include <dev/eisa/eisareg.h>
#include <dev/eisa/eisavar.h>

#define	EISA_SLOT_HEADER_SIZE	31
#define	EISA_SLOT_INFO_OFFSET	20

#define	EISA_FUNC_INFO_OFFSET	34
#define	EISA_CONFIG_BLOCK_SIZE	320

#define	ECUF_TYPE_STRING	0x01
#define	ECUF_MEM_ENTRY		0x02
#define	ECUF_IRQ_ENTRY		0x04
#define	ECUF_DMA_ENTRY		0x08
#define	ECUF_IO_ENTRY		0x10
#define	ECUF_INIT_ENTRY		0x20
#define	ECUF_DISABLED		0x80

#define	ECUF_SELECTIONS_SIZE	26
#define	ECUF_TYPE_STRING_SIZE	80
#define	ECUF_MEM_ENTRY_SIZE	7
#define	ECUF_IRQ_ENTRY_SIZE	2
#define	ECUF_DMA_ENTRY_SIZE	2
#define	ECUF_IO_ENTRY_SIZE	3
#define	ECUF_INIT_ENTRY_SIZE	60

#define	ECUF_MEM_ENTRY_CNT	9
#define	ECUF_IRQ_ENTRY_CNT	7
#define	ECUF_DMA_ENTRY_CNT	4
#define	ECUF_IO_ENTRY_CNT	20

/*
 * EISA configuration space, as set up by the ECU, may be sparse.
 */
bus_size_t eisa_config_stride;
paddr_t eisa_config_addr;		/* defaults to 0 */
paddr_t eisa_config_header_addr;

struct ecu_mem {
	SIMPLEQ_ENTRY(ecu_mem) ecum_list;
	struct eisa_cfg_mem ecum_mem;
};

struct ecu_irq {
	SIMPLEQ_ENTRY(ecu_irq) ecui_list;
	struct eisa_cfg_irq ecui_irq;
};

struct ecu_dma {
	SIMPLEQ_ENTRY(ecu_dma) ecud_list;
	struct eisa_cfg_dma ecud_dma;
};

struct ecu_io {
	SIMPLEQ_ENTRY(ecu_io) ecuio_list;
	struct eisa_cfg_io ecuio_io;
};

struct ecu_func {
	SIMPLEQ_ENTRY(ecu_func) ecuf_list;
	int ecuf_funcno;
	u_int32_t ecuf_id;
	u_int16_t ecuf_slot_info;
	u_int16_t ecuf_cfg_ext;
	u_int8_t ecuf_selections[ECUF_SELECTIONS_SIZE];
	u_int8_t ecuf_func_info;
	u_int8_t ecuf_type_string[ECUF_TYPE_STRING_SIZE];
	u_int8_t ecuf_init[ECUF_INIT_ENTRY_SIZE];
	SIMPLEQ_HEAD(, ecu_mem) ecuf_mem;
	SIMPLEQ_HEAD(, ecu_irq) ecuf_irq;
	SIMPLEQ_HEAD(, ecu_dma) ecuf_dma;
	SIMPLEQ_HEAD(, ecu_io) ecuf_io;
};

struct ecu_data {
	SIMPLEQ_ENTRY(ecu_data) ecud_list;
	int ecud_slot;
	u_int8_t ecud_eisaid[EISA_IDSTRINGLEN];
	u_int32_t ecud_offset;

	/* General slot info. */
	u_int8_t ecud_slot_info;
	u_int16_t ecud_ecu_major_rev;
	u_int16_t ecud_ecu_minor_rev;
	u_int16_t ecud_cksum;
	u_int16_t ecud_ndevfuncs;
	u_int8_t ecud_funcinfo;
	u_int32_t ecud_comp_id;

	/* The functions */
	SIMPLEQ_HEAD(, ecu_func) ecud_funcs;
};

SIMPLEQ_HEAD(, ecu_data) ecu_data_list =
    SIMPLEQ_HEAD_INITIALIZER(ecu_data_list);

static void
ecuf_init(struct ecu_func *ecuf)
{

	memset(ecuf, 0, sizeof(*ecuf));
	SIMPLEQ_INIT(&ecuf->ecuf_mem);
	SIMPLEQ_INIT(&ecuf->ecuf_irq);
	SIMPLEQ_INIT(&ecuf->ecuf_dma);
	SIMPLEQ_INIT(&ecuf->ecuf_io);
}

static void
eisa_parse_mem(struct ecu_func *ecuf, u_int8_t *dp)
{
	struct ecu_mem *ecum;
	int i;

	for (i = 0; i < ECUF_MEM_ENTRY_CNT; i++) {
		ecum = malloc(sizeof(*ecum), M_DEVBUF, M_WAITOK);

		ecum->ecum_mem.ecm_isram = dp[0] & 0x1;
		ecum->ecum_mem.ecm_unitsize = dp[1] & 0x3;
		ecum->ecum_mem.ecm_decode = (dp[1] >> 2) & 0x3;
		ecum->ecum_mem.ecm_addr =
		    (dp[2] | (dp[3] << 8) | (dp[4] << 16)) << 8;
		ecum->ecum_mem.ecm_size = (dp[5] | (dp[6] << 8)) << 10;
		if (ecum->ecum_mem.ecm_size == 0)
			ecum->ecum_mem.ecm_size = (1 << 26);
		SIMPLEQ_INSERT_TAIL(&ecuf->ecuf_mem, ecum, ecum_list);

#if 0
		printf("MEM 0x%lx 0x%lx %d %d %d\n",
		    ecum->ecum_mem.ecm_addr, ecum->ecum_mem.ecm_size,
		    ecum->ecum_mem.ecm_isram, ecum->ecum_mem.ecm_unitsize,
		    ecum->ecum_mem.ecm_decode);
#endif

		if ((dp[0] & 0x80) == 0)
			break;
		dp += ECUF_MEM_ENTRY_SIZE;
	}
}

static void
eisa_parse_irq(struct ecu_func *ecuf, u_int8_t *dp)
{
	struct ecu_irq *ecui;
	int i;

	for (i = 0; i < ECUF_IRQ_ENTRY_CNT; i++) {
		ecui = malloc(sizeof(*ecui), M_DEVBUF, M_WAITOK);

		ecui->ecui_irq.eci_irq = dp[0] & 0xf;
		ecui->ecui_irq.eci_ist = (dp[0] & 0x20) ? IST_LEVEL : IST_EDGE;
		ecui->ecui_irq.eci_shared = (dp[0] & 0x40) ? 1 : 0;
		SIMPLEQ_INSERT_TAIL(&ecuf->ecuf_irq, ecui, ecui_list);

#if 0
		printf("IRQ %d %s%s\n", ecui->eci_irq.ecui_irq,
		    ecui->eci_irq.ecui_ist == IST_LEVEL ? "level" : "edge",
		    ecui->eci_irq.ecui_shared ? " shared" : "");
#endif

		if ((dp[0] & 0x80) == 0)
			break;
		dp += ECUF_IRQ_ENTRY_SIZE;
	}
}

static void
eisa_parse_dma(struct ecu_func *ecuf, u_int8_t *dp)
{
	struct ecu_dma *ecud;
	int i;

	for (i = 0; i < ECUF_DMA_ENTRY_CNT; i++) {
		ecud = malloc(sizeof(*ecud), M_DEVBUF, M_WAITOK);

		ecud->ecud_dma.ecd_drq = dp[0] & 0x7;
		ecud->ecud_dma.ecd_shared = dp[0] & 0x40;
		ecud->ecud_dma.ecd_size = (dp[1] >> 2) & 0x3;
		ecud->ecud_dma.ecd_timing = (dp[1] >> 4) & 0x3;
		SIMPLEQ_INSERT_TAIL(&ecuf->ecuf_dma, ecud, ecud_list);

#if 0
		printf("DRQ %d%s %d %d\n", ecud->ecud_dma.ecd_drq,
		    ecud->ecud_dma.ecd_shared ? " shared" : "",
		    ecud->ecud_dma.ecd_size, ecud->ecud_dma.ecd_timing);
#endif

		if ((dp[0] & 0x80) == 0)
			break;
		dp += ECUF_DMA_ENTRY_SIZE;
	}
}

static void
eisa_parse_io(struct ecu_func *ecuf, u_int8_t *dp)
{
	struct ecu_io *ecuio;
	int i;

	for (i = 0; i < ECUF_IO_ENTRY_CNT; i++) {
		ecuio = malloc(sizeof(*ecuio), M_DEVBUF, M_WAITOK);

		ecuio->ecuio_io.ecio_addr = dp[1] | (dp[2] << 8);
		ecuio->ecuio_io.ecio_size = (dp[0] & 0x1f) + 1;
		ecuio->ecuio_io.ecio_shared = (dp[0] & 0x40) ? 1 : 0;

#if 0
		printf("IO 0x%lx 0x%lx%s\n", ecuio->ecuio_io.ecio_addr,
		    ecuio->ecuio_io.ecio_size,
		    ecuio->ecuio_io.ecio_shared ? " shared" : "");
#endif

		if ((dp[0] & 0x80) == 0)
			break;
		dp += ECUF_IO_ENTRY_SIZE;
	}
}

static void
eisa_read_config_bytes(paddr_t addr, void *buf, size_t count)
{
	const u_int8_t *src = (const u_int8_t *)ALPHA_PHYS_TO_K0SEG(addr);
	u_int8_t *dst = buf;

	for (; count != 0; count--) {
		*dst++ = *src;
		src += eisa_config_stride;
	}
}

static void
eisa_read_config_word(paddr_t addr, u_int32_t *valp)
{
	const u_int8_t *src = (const u_int8_t *)ALPHA_PHYS_TO_K0SEG(addr);
	u_int32_t val = 0;
	int i;

	for (i = 0; i < sizeof(val); i++) {
		val |= (u_int)(*src << (i * 8));
		src += eisa_config_stride;
	}

	*valp = val;
}

static size_t
eisa_uncompress(void *cbufp, void *ucbufp, size_t count)
{
	const u_int8_t *cbuf = cbufp;
	u_int8_t *ucbuf = ucbufp;
	u_int zeros = 0;

	while (count--) {
		if (zeros) {
			zeros--;
			*ucbuf++ = '\0';
		} else if (*cbuf == '\0') {
			*ucbuf++ = *cbuf++;
			zeros = *cbuf++ - 1;
		} else
			*ucbuf++ = *cbuf++;
	}

	return ((size_t)cbuf - (size_t)cbufp);
}

void
eisa_init()
{
	struct ecu_data *ecud;
	paddr_t cfgaddr;
	u_int32_t offset;
	u_int8_t eisaid[EISA_IDSTRINGLEN];
	u_int8_t *cdata, *data;
	u_int8_t *cdp, *dp;
	struct ecu_func *ecuf;
	int i, func;

	/*
	 * Locate EISA configuration space.
	 */
	if (hwrpb->rpb_condat_off == 0UL ||
	    (hwrpb->rpb_condat_off >> 63) != 0) {
		printf(": WARNING: no EISA configuration space");
		return;
	}

	if (eisa_config_header_addr) {
		printf("\n");
		panic("eisa_init: EISA config space already initialized");
	}

	eisa_config_header_addr = hwrpb->rpb_condat_off;
#if 0
	printf("\nEISA config header at 0x%lx\n", eisa_config_header_addr);
#endif
	if (eisa_config_stride == 0)
		eisa_config_stride = 1;

	/*
	 * Read the slot headers, and allocate config structures for
	 * valid slots.
	 */
	for (cfgaddr = eisa_config_header_addr, i = 0; i < 16 /* XXX */; i++) {
		eisa_read_config_bytes(cfgaddr, eisaid, sizeof(eisaid));
		eisaid[EISA_IDSTRINGLEN - 1] = '\0';	/* sanity */
		cfgaddr += sizeof(eisaid) * eisa_config_stride;
		eisa_read_config_word(cfgaddr, &offset);
		cfgaddr += sizeof(offset) * eisa_config_stride;

		if (offset != 0) {
#if 0
			printf("SLOT %d: offset 0x%08x eisaid %s\n",
			    i, offset, eisaid);
#endif
			ecud = malloc(sizeof(*ecud), M_DEVBUF, M_WAITOK);
			memset(ecud, 0, sizeof(*ecud));

			SIMPLEQ_INIT(&ecud->ecud_funcs);

			ecud->ecud_slot = i;
			memcpy(ecud->ecud_eisaid, eisaid, sizeof(eisaid));
			ecud->ecud_offset = offset;
			SIMPLEQ_INSERT_TAIL(&ecu_data_list, ecud, ecud_list);
		}
	}

	/*
	 * Now traverse the valid slots and read the info.
	 */

	cdata = malloc(512, M_TEMP, M_WAITOK);
	data = malloc(512, M_TEMP, M_WAITOK);

	for (ecud = SIMPLEQ_FIRST(&ecu_data_list); ecud != NULL;
	     ecud = SIMPLEQ_NEXT(ecud, ecud_list)) {
		cfgaddr = eisa_config_addr + ecud->ecud_offset;
		eisa_read_config_bytes(cfgaddr, &cdata[0], 1);
		cfgaddr += eisa_config_stride;

		for (i = 1; ; cfgaddr += eisa_config_stride, i++) {
			eisa_read_config_bytes(cfgaddr, &cdata[i], 1);
			if (cdata[i - 1] == 0 && cdata[i] == 0)
				break;
		}
		i++;	/* index -> length */

#if 0
		printf("SLOT %d compressed data length %d:",
		    ecud->ecud_slot, i);
		{
			int j;

			for (j = 0; j < i; j++) {
				if ((j % 16) == 0)
					printf("\n");
				printf("0x%02x ", cdata[j]);
			}
			printf("\n");
		}
#endif

		cdp = cdata;
		dp = data;

		/* Uncompress the slot header. */
		cdp += eisa_uncompress(cdp, dp, EISA_SLOT_HEADER_SIZE);
#if 0
		printf("SLOT %d uncompressed header data:",
		    ecud->ecud_slot);
		{
			int j;

			for (j = 0; j < EISA_SLOT_HEADER_SIZE; j++) {
				if ((j % 16) == 0)
					printf("\n");
				printf("0x%02x ", dp[j]);
			}
			printf("\n");
		}
#endif

		dp = &data[EISA_SLOT_INFO_OFFSET];
		ecud->ecud_slot_info = *dp++;
		ecud->ecud_ecu_major_rev = *dp++;
		ecud->ecud_ecu_minor_rev = *dp++;
		memcpy(&ecud->ecud_cksum, dp, sizeof(ecud->ecud_cksum));
		dp += sizeof(ecud->ecud_cksum);
		ecud->ecud_ndevfuncs = *dp++;
		ecud->ecud_funcinfo = *dp++;
		memcpy(&ecud->ecud_comp_id, dp, sizeof(ecud->ecud_comp_id));
		dp += sizeof(ecud->ecud_comp_id);

#if 0
		printf("SLOT %d: ndevfuncs %d\n", ecud->ecud_slot,
		    ecud->ecud_ndevfuncs);
#endif

		for (func = 0; func < ecud->ecud_ndevfuncs; func++) {
			dp = data;
			cdp += eisa_uncompress(cdp, dp, EISA_CONFIG_BLOCK_SIZE);
#if 0
			printf("SLOT %d:%d uncompressed data:",
			    ecud->ecud_slot, func);
			{
				int j;

				for (j = 0; i < EISA_CONFIG_BLOCK_SIZE; j++) {
					if ((j % 16) == 0)
						printf("\n");
					printf("0x%02x ", dp[j]);
				}
				printf("\n");
			}
#endif

			/* Skip disabled functions. */
			if (dp[EISA_FUNC_INFO_OFFSET] & ECUF_DISABLED) {
				printf("SLOT %d:%d disabled\n",
				    ecud->ecud_slot, func);
				continue;
			}

			ecuf = malloc(sizeof(*ecuf), M_DEVBUF, M_WAITOK);
			ecuf_init(ecuf);
			ecuf->ecuf_funcno = func;
			SIMPLEQ_INSERT_TAIL(&ecud->ecud_funcs, ecuf,
			    ecuf_list);

			memcpy(&ecuf->ecuf_id, dp, sizeof(ecuf->ecuf_id));
			dp += sizeof(ecuf->ecuf_id);

			memcpy(&ecuf->ecuf_slot_info, dp,
			    sizeof(ecuf->ecuf_slot_info));
			dp += sizeof(ecuf->ecuf_slot_info);

			memcpy(&ecuf->ecuf_cfg_ext, dp,
			    sizeof(ecuf->ecuf_cfg_ext));
			dp += sizeof(ecuf->ecuf_cfg_ext);

			memcpy(&ecuf->ecuf_selections, dp,
			    sizeof(ecuf->ecuf_selections));
			dp += sizeof(ecuf->ecuf_selections);

			memcpy(&ecuf->ecuf_func_info, dp,
			    sizeof(ecuf->ecuf_func_info));
			dp += sizeof(ecuf->ecuf_func_info);

			if (ecuf->ecuf_func_info & ECUF_TYPE_STRING)
				memcpy(ecuf->ecuf_type_string, dp,
				    sizeof(ecuf->ecuf_type_string));
			dp += sizeof(ecuf->ecuf_type_string);

			if (ecuf->ecuf_func_info & ECUF_MEM_ENTRY)
				eisa_parse_mem(ecuf, dp);
			dp += ECUF_MEM_ENTRY_SIZE * ECUF_MEM_ENTRY_CNT;

			if (ecuf->ecuf_func_info & ECUF_IRQ_ENTRY)
				eisa_parse_irq(ecuf, dp);
			dp += ECUF_IRQ_ENTRY_SIZE * ECUF_IRQ_ENTRY_CNT;

			if (ecuf->ecuf_func_info & ECUF_DMA_ENTRY)
				eisa_parse_dma(ecuf, dp);
			dp += ECUF_DMA_ENTRY_SIZE * ECUF_DMA_ENTRY_CNT;

			if (ecuf->ecuf_func_info & ECUF_IO_ENTRY)
				eisa_parse_io(ecuf, dp);
			dp += ECUF_IO_ENTRY_SIZE * ECUF_IO_ENTRY_CNT;

			if (ecuf->ecuf_func_info & ECUF_INIT_ENTRY)
				memcpy(ecuf->ecuf_init, dp,
				    sizeof(ecuf->ecuf_init));
			dp += sizeof(ecuf->ecuf_init);
		}
	}

	free(cdata, M_TEMP);
	free(data, M_TEMP);
}

static struct ecu_data *
eisa_lookup_data(int slot)
{
	struct ecu_data *ecud;

	for (ecud = SIMPLEQ_FIRST(&ecu_data_list); ecud != NULL;
	     ecud = SIMPLEQ_NEXT(ecud, ecud_list)) {
		if (ecud->ecud_slot == slot)
			return (ecud);
	}
	return (NULL);
}

static struct ecu_func *
eisa_lookup_func(int slot, int func)
{
	struct ecu_data *ecud;
	struct ecu_func *ecuf;

	ecud = eisa_lookup_data(slot);
	if (ecud == NULL)
		return (NULL);

	for (ecuf = SIMPLEQ_FIRST(&ecud->ecud_funcs); ecuf != NULL;
	     ecuf = SIMPLEQ_NEXT(ecuf, ecuf_list)) {
		if (ecuf->ecuf_funcno == func)
			return (ecuf);
	}
	return (NULL);
}

int
eisa_conf_read_mem(eisa_chipset_tag_t ec, int slot, int func, int entry,
    struct eisa_cfg_mem *dp)
{
	struct ecu_func *ecuf;
	struct ecu_mem *ecum;

	ecuf = eisa_lookup_func(slot, func);
	if (ecuf == NULL)
		return (ENOENT);

	for (ecum = SIMPLEQ_FIRST(&ecuf->ecuf_mem); ecum != NULL;
	     ecum = SIMPLEQ_NEXT(ecum, ecum_list)) {
		if (entry-- == 0)
			break;
	}
	if (ecum == NULL)
		return (ENOENT);

	*dp = ecum->ecum_mem;
	return (0);
}

int
eisa_conf_read_irq(eisa_chipset_tag_t ec, int slot, int func, int entry,
    struct eisa_cfg_irq *dp)
{
	struct ecu_func *ecuf;
	struct ecu_irq *ecui;

	ecuf = eisa_lookup_func(slot, func);
	if (ecuf == NULL)
		return (ENOENT);

	for (ecui = SIMPLEQ_FIRST(&ecuf->ecuf_irq); ecui != NULL;
	     ecui = SIMPLEQ_NEXT(ecui, ecui_list)) {
		if (entry-- == 0)
			break;
	}
	if (ecui == NULL)
		return (ENOENT);

	*dp = ecui->ecui_irq;
	return (0);
}

int
eisa_conf_read_dma(eisa_chipset_tag_t ec, int slot, int func, int entry,
    struct eisa_cfg_dma *dp)
{
	struct ecu_func *ecuf;
	struct ecu_dma *ecud;

	ecuf = eisa_lookup_func(slot, func);
	if (ecuf == NULL)
		return (ENOENT);

	for (ecud = SIMPLEQ_FIRST(&ecuf->ecuf_dma); ecud != NULL;
	     ecud = SIMPLEQ_NEXT(ecud, ecud_list)) {
		if (entry-- == 0)
			break;
	}
	if (ecud == NULL)
		return (ENOENT);

	*dp = ecud->ecud_dma;
	return (0);
}

int
eisa_conf_read_io(eisa_chipset_tag_t ec, int slot, int func, int entry,
    struct eisa_cfg_io *dp)
{
	struct ecu_func *ecuf;
	struct ecu_io *ecuio;

	ecuf = eisa_lookup_func(slot, func);
	if (ecuf == NULL)
		return (ENOENT);

	for (ecuio = SIMPLEQ_FIRST(&ecuf->ecuf_io); ecuio != NULL;
	     ecuio = SIMPLEQ_NEXT(ecuio, ecuio_list)) {
		if (entry-- == 0)
			break;
	}
	if (ecuio == NULL)
		return (ENOENT);

	*dp = ecuio->ecuio_io;
	return (0);
}
