/*	$NetBSD: platid_name.c,v 1.12 2001/01/28 02:52:35 uch Exp $	*/

/*-
 * Copyright (c) 1999-2001
 *         Shin Takemura and PocketBSD Project. All rights reserved.
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
 *	This product includes software developed by the PocketBSD project
 *	and its contributors.
 * 4. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/*
 *  Do not edit.
 *  This file is automatically generated by platid.awk.
 */
#include <machine/platid.h>
#include <machine/platid_mask.h>
struct platid_name platid_name_table[] = {
	{ &platid_mask_CPU_MIPS,
	 TEXT("MIPS") },
	{ &platid_mask_CPU_MIPS_VR,
	 TEXT("MIPS VR") },
	{ &platid_mask_CPU_MIPS_VR_41XX,
	 TEXT("MIPS VR 41XX") },
	{ &platid_mask_CPU_MIPS_VR_4102,
	 TEXT("MIPS VR 4102") },
	{ &platid_mask_CPU_MIPS_VR_4111,
	 TEXT("MIPS VR 4111") },
	{ &platid_mask_CPU_MIPS_VR_4121,
	 TEXT("MIPS VR 4121") },
	{ &platid_mask_CPU_MIPS_TX,
	 TEXT("MIPS TX") },
	{ &platid_mask_CPU_MIPS_TX_3900,
	 TEXT("MIPS TX 3900") },
	{ &platid_mask_CPU_MIPS_TX_3911,
	 TEXT("MIPS TX 3911") },
	{ &platid_mask_CPU_MIPS_TX_3912,
	 TEXT("MIPS TX 3912") },
	{ &platid_mask_CPU_MIPS_TX_3920,
	 TEXT("MIPS TX 3920") },
	{ &platid_mask_CPU_MIPS_TX_3922,
	 TEXT("MIPS TX 3922") },
	{ &platid_mask_MACH_NEC,
	 TEXT("NEC") },
	{ &platid_mask_MACH_NEC_MCCS,
	 TEXT("NEC MC-CS") },
	{ &platid_mask_MACH_NEC_MCCS_1X,
	 TEXT("NEC MC-CS series") },
	{ &platid_mask_MACH_NEC_MCCS_11,
	 TEXT("NEC MC-CS11") },
	{ &platid_mask_MACH_NEC_MCCS_12,
	 TEXT("NEC MC-CS12") },
	{ &platid_mask_MACH_NEC_MCCS_13,
	 TEXT("NEC MC-CS13") },
	{ &platid_mask_MACH_NEC_MCR,
	 TEXT("NEC MC-R") },
	{ &platid_mask_MACH_NEC_MCR_3XX,
	 TEXT("NEC MC-R300 series") },
	{ &platid_mask_MACH_NEC_MCR_300,
	 TEXT("NEC MC-R300") },
	{ &platid_mask_MACH_NEC_MCR_320,
	 TEXT("NEC MC-R320") },
	{ &platid_mask_MACH_NEC_MCR_FORDOCOMO,
	 TEXT("NEC MobileGearII for DoCoMo") },
	{ &platid_mask_MACH_NEC_MCR_MPRO700,
	 TEXT("NEC MobilePro 700") },
	{ &platid_mask_MACH_NEC_MCR_330,
	 TEXT("NEC MC-R330") },
	{ &platid_mask_MACH_NEC_MCR_5XX,
	 TEXT("NEC MC-R500 series") },
	{ &platid_mask_MACH_NEC_MCR_500,
	 TEXT("NEC MC-R500") },
	{ &platid_mask_MACH_NEC_MCR_510,
	 TEXT("NEC MC-R510") },
	{ &platid_mask_MACH_NEC_MCR_520,
	 TEXT("NEC MC-R520") },
	{ &platid_mask_MACH_NEC_MCR_520A,
	 TEXT("NEC MobilePro 770") },
	{ &platid_mask_MACH_NEC_MCR_500A,
	 TEXT("NEC MobilePro 750c") },
	{ &platid_mask_MACH_NEC_MCR_530,
	 TEXT("NEC MC-R530") },
	{ &platid_mask_MACH_NEC_MCR_430,
	 TEXT("NEC MC-R430") },
	{ &platid_mask_MACH_NEC_MCR_530A,
	 TEXT("NEC MobilePro 780") },
	{ &platid_mask_MACH_NEC_MCR_SIGMARION,
	 TEXT("DoCoMo sigmarion") },
	{ &platid_mask_MACH_NEC_MCR_7XX,
	 TEXT("NEC MC-R700 series") },
	{ &platid_mask_MACH_NEC_MCR_700,
	 TEXT("NEC MC-R700") },
	{ &platid_mask_MACH_NEC_MCR_700A,
	 TEXT("NEC MobilePro 800") },
	{ &platid_mask_MACH_NEC_MCR_730,
	 TEXT("NEC MC-R730") },
	{ &platid_mask_MACH_NEC_MCR_730A,
	 TEXT("NEC MobilePro 880") },
	{ &platid_mask_MACH_EVEREX,
	 TEXT("Everex") },
	{ &platid_mask_MACH_EVEREX_FREESTYLE,
	 TEXT("Everex Freestyle") },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_AXX,
	 TEXT("Everex Freestyle AXX") },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A10,
	 TEXT("Everex Freestyle A10") },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A15,
	 TEXT("Everex Freestyle A15") },
	{ &platid_mask_MACH_EVEREX_FREESTYLE_A20,
	 TEXT("Everex Freestyle A20") },
	{ &platid_mask_MACH_CASIO,
	 TEXT("CASIO") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE,
	 TEXT("CASIO Cassiopeia") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_EXX,
	 TEXT("CASIO Cassiopeia EXX") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E10,
	 TEXT("CASIO Cassiopeia E10") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E11,
	 TEXT("CASIO Cassiopeia E11") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E15,
	 TEXT("CASIO Cassiopeia E15") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E55,
	 TEXT("CASIO Cassiopeia E-55") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_FORDOCOMO,
	 TEXT("CASIO Cassiopeia for DoCoMo") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E65,
	 TEXT("CASIO Cassiopeia E-65") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_EXXX,
	 TEXT("CASIO Cassiopeia EXXX") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E100,
	 TEXT("CASIO Cassiopeia E100") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E105,
	 TEXT("CASIO Cassiopeia E105") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E500,
	 TEXT("CASIO Cassiopeia E500") },
	{ &platid_mask_MACH_CASIO_CASSIOPEIAE_E507,
	 TEXT("CASIO Cassiopeia E507") },
	{ &platid_mask_MACH_SHARP,
	 TEXT("Sharp") },
	{ &platid_mask_MACH_SHARP_TRIPAD,
	 TEXT("Sharp Tripad") },
	{ &platid_mask_MACH_SHARP_TRIPAD_PV,
	 TEXT("Sharp Tripad PV") },
	{ &platid_mask_MACH_SHARP_TRIPAD_PV6000,
	 TEXT("Sharp Tripad PV6000") },
	{ &platid_mask_MACH_SHARP_TELIOS,
	 TEXT("Sharp Telios") },
	{ &platid_mask_MACH_SHARP_TELIOS_HC,
	 TEXT("Sharp Telios HC") },
	{ &platid_mask_MACH_SHARP_TELIOS_HCAJ1,
	 TEXT("Sharp HC-AJ1/AJ2") },
	{ &platid_mask_MACH_SHARP_TELIOS_HCVJ1C_JP,
	 TEXT("Sharp HC-VJ1C (Japanese)") },
	{ &platid_mask_MACH_SHARP_MOBILON,
	 TEXT("Sharp Mobilon") },
	{ &platid_mask_MACH_SHARP_MOBILON_HC,
	 TEXT("Sharp Mobilon HC") },
	{ &platid_mask_MACH_SHARP_MOBILON_HC4100,
	 TEXT("Sharp Mobilon HC4100") },
	{ &platid_mask_MACH_SHARP_MOBILON_HC4500,
	 TEXT("Sharp Mobilon HC4500") },
	{ &platid_mask_MACH_SHARP_MOBILON_HC1200,
	 TEXT("Sharp Mobilon HC1200") },
	{ &platid_mask_MACH_FUJITSU,
	 TEXT("Fujitsu") },
	{ &platid_mask_MACH_FUJITSU_INTERTOP,
	 TEXT("Fujitsu INTERTOP") },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_ITXXX,
	 TEXT("Fujitsu INTERTOP ITXXX") },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_IT300,
	 TEXT("Fujitsu INTERTOP IT300") },
	{ &platid_mask_MACH_FUJITSU_INTERTOP_IT310,
	 TEXT("Fujitsu INTERTOP IT310") },
	{ &platid_mask_MACH_PHILIPS,
	 TEXT("Philips") },
	{ &platid_mask_MACH_PHILIPS_NINO,
	 TEXT("Philips Nino") },
	{ &platid_mask_MACH_PHILIPS_NINO_3XX,
	 TEXT("Philips Nino 3XX") },
	{ &platid_mask_MACH_PHILIPS_NINO_312,
	 TEXT("Philips Nino 312") },
	{ &platid_mask_MACH_COMPAQ,
	 TEXT("Compaq") },
	{ &platid_mask_MACH_COMPAQ_C,
	 TEXT("Compaq C") },
	{ &platid_mask_MACH_COMPAQ_C_8XX,
	 TEXT("Compaq C 8XX") },
	{ &platid_mask_MACH_COMPAQ_C_810,
	 TEXT("Compaq C 810") },
	{ &platid_mask_MACH_COMPAQ_C_201X,
	 TEXT("Compaq C 201X") },
	{ &platid_mask_MACH_COMPAQ_C_2010,
	 TEXT("Compaq C 2010") },
	{ &platid_mask_MACH_COMPAQ_C_2015,
	 TEXT("Compaq C 2015") },
	{ &platid_mask_MACH_COMPAQ_AERO,
	 TEXT("Compaq AERO") },
	{ &platid_mask_MACH_COMPAQ_AERO_15XX,
	 TEXT("Compaq AERO 15XX") },
	{ &platid_mask_MACH_COMPAQ_AERO_1530,
	 TEXT("Compaq AERO 1530") },
	{ &platid_mask_MACH_COMPAQ_AERO_21XX,
	 TEXT("Compaq AERO 21XX") },
	{ &platid_mask_MACH_COMPAQ_AERO_2110,
	 TEXT("Compaq AERO 2110") },
	{ &platid_mask_MACH_COMPAQ_AERO_2130,
	 TEXT("Compaq AERO 2130") },
	{ &platid_mask_MACH_COMPAQ_AERO_2140,
	 TEXT("Compaq AERO 2140") },
	{ &platid_mask_MACH_COMPAQ_PRESARIO,
	 TEXT("Compaq PRESARIO") },
	{ &platid_mask_MACH_COMPAQ_PRESARIO_21X,
	 TEXT("Compaq PRESARIO 21X") },
	{ &platid_mask_MACH_COMPAQ_PRESARIO_213,
	 TEXT("Compaq PRESARIO 213") },
	{ &platid_mask_MACH_VICTOR,
	 TEXT("Victor") },
	{ &platid_mask_MACH_VICTOR_INTERLINK,
	 TEXT("Victor InterLink") },
	{ &platid_mask_MACH_VICTOR_INTERLINK_MP,
	 TEXT("Victor InterLink MP") },
	{ &platid_mask_MACH_VICTOR_INTERLINK_MPC101,
	 TEXT("Victor InterLink MPC101") },
	{ &platid_mask_MACH_IBM,
	 TEXT("IBM") },
	{ &platid_mask_MACH_IBM_WORKPAD,
	 TEXT("IBM WorkPad") },
	{ &platid_mask_MACH_IBM_WORKPAD_Z50,
	 TEXT("IBM WorkPad z50") },
	{ &platid_mask_MACH_IBM_WORKPAD_26011AU,
	 TEXT("IBM WorkPad z50 2601 1AU") },
	{ &platid_mask_MACH_VADEM,
	 TEXT("VADEM") },
	{ &platid_mask_MACH_VADEM_CLIO,
	 TEXT("VADEM CLIO") },
	{ &platid_mask_MACH_VADEM_CLIO_C,
	 TEXT("VADEM CLIO C") },
	{ &platid_mask_MACH_VADEM_CLIO_C1000,
	 TEXT("VADEM CLIO C-1000") },
	{ &platid_mask_MACH_VADEM_CLIO_C1050,
	 TEXT("VADEM CLIO C-1050") },
};
int platid_name_table_size = 112;
