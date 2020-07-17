/* $NetBSD$ */

/*-
 * Copyright (c) 2020 Danny Lin <danny@kdrag0n.dev>
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

#include "opt_soc.h"
#include "opt_multiprocessor.h"
#include "opt_console.h"

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/device.h>
#include <sys/termios.h>

#include <dev/fdt/fdtvar.h>
#include <arm/fdt/arm_fdtvar.h>

#include <uvm/uvm_extern.h>

#include <machine/bootconfig.h>
#include <arm/cpufunc.h>
#include <arm/locore.h>

#include <evbarm/dev/plcomreg.h>
#include <evbarm/dev/plcomvar.h>

#include <dev/ic/ns16550reg.h>
#include <dev/ic/comreg.h>

#include <arm/cortex/gtmr_var.h>

#include <arm/arm/psci.h>
#include <arm/fdt/psci_fdtvar.h>

#include <arm/easel/easel_platform.h>

#define UART_LSR	5	/* In:  Line Status Register */
#define UART_LSR_TEMT		0x40 /* Transmitter empty */
#define UART_LSR_THRE		0x20 /* Transmit-hold-register empty */

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

void easel_platform_early_putchar(char);

void __noasan
easel_platform_early_putchar(char c)
{
#ifdef CONSADDR
	volatile uint32_t *uartaddr = cpu_earlydevice_va_p() ?
		(volatile uint32_t *)EASEL_PERIPHERAL_PTOV(CONSADDR) :
		(volatile uint32_t *)CONSADDR;

	uartaddr[0] = htole32(c);

	while ((le32toh(uartaddr[(UART_LSR << 2) / 4]) & BOTH_EMPTY) == 0)
		;
#endif
}

static const struct pmap_devmap *
easel_platform_devmap(void)
{
	static const struct pmap_devmap devmap[] = {
		DEVMAP_ENTRY(EASEL_PERIPHERAL_VBASE,
			     EASEL_PERIPHERAL_PBASE,
			     EASEL_PERIPHERAL_SIZE),
		DEVMAP_ENTRY_END
	};

	return devmap;
}

static void
easel_platform_init_attach_args(struct fdt_attach_args *faa)
{
	extern struct arm32_bus_dma_tag arm_generic_dma_tag;
	extern struct bus_space arm_generic_bs_tag;
	extern struct bus_space arm_generic_a4x_bs_tag;

	faa->faa_bst = &arm_generic_bs_tag;
	faa->faa_a4x_bst = &arm_generic_a4x_bs_tag;
	faa->faa_dmat = &arm_generic_dma_tag;
}

static void
easel_platform_device_register(device_t self, void *aux)
{
}

static u_int
easel_platform_uart_freq(void)
{
	// Reference 10 MHz clock
	return 100000000;
}

static const struct arm_platform easel_platform = {
	.ap_devmap = easel_platform_devmap,
	.ap_bootstrap = arm_fdt_cpu_bootstrap,
	.ap_init_attach_args = easel_platform_init_attach_args,
	.ap_device_register = easel_platform_device_register,
	.ap_reset = psci_fdt_reset,
	.ap_delay = gtmr_delay,
	.ap_uart_freq = easel_platform_uart_freq,
	.ap_mpstart = arm_fdt_cpu_mpstart,
};

ARM_PLATFORM(easel, "intel,mnh", &easel_platform);
