/*	$NetBSD: syscall.c,v 1.18 2003/06/29 22:28:08 fvdl Exp $	*/

/*-
 * Copyright (c) 2000, 2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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
 * Copyright (c) 1994-1998 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
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
 *	This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * syscall entry handling
 *
 * Created      : 09/11/94
 */

#include "opt_ktrace.h"
#include "opt_systrace.h"
#include "opt_syscall_debug.h"

#include <sys/param.h>

__KERNEL_RCSID(0, "$NetBSD: syscall.c,v 1.18 2003/06/29 22:28:08 fvdl Exp $");

#include <sys/device.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/reboot.h>
#include <sys/signalvar.h>
#include <sys/syscall.h>
#include <sys/systm.h>
#include <sys/user.h>
#ifdef KTRACE
#include <sys/ktrace.h>
#endif
#ifdef SYSTRACE
#include <sys/systrace.h>
#endif

#include <uvm/uvm_extern.h>

#include <machine/cpu.h>
#include <machine/frame.h>
#include <machine/pcb.h>
#include <arm/swi.h>

#ifdef acorn26
#include <machine/machdep.h>
#endif

void
swi_handler(trapframe_t *frame)
{
	struct lwp *l = curlwp;
	struct proc *p = l->l_proc;
	u_int32_t insn;

	/*
	 * Enable interrupts if they were enabled before the exception.
	 * Since all syscalls *should* come from user mode it will always
	 * be safe to enable them, but check anyway. 
	 */
#ifdef acorn26
	if ((frame->tf_r15 & R15_IRQ_DISABLE) == 0)
		int_on();
#else
	if (!(frame->tf_spsr & I32_bit))
		enable_interrupts(I32_bit);
#endif

#ifdef acorn26
	frame->tf_pc += INSN_SIZE;
#endif

	/* XXX fuword? */
#ifdef __PROG32
	insn = *(u_int32_t *)(frame->tf_pc - INSN_SIZE);
#else
	insn = *(u_int32_t *)((frame->tf_r15 & R15_PC) - INSN_SIZE);
#endif

	l->l_addr->u_pcb.pcb_tf = frame;

#ifdef CPU_ARM7
	/*
	 * This code is only needed if we are including support for the ARM7
	 * core. Other CPUs do not need it but it does not hurt.
	 */

	/*
	 * ARM700/ARM710 match sticks and sellotape job ...
	 *
	 * I know this affects GPS/VLSI ARM700/ARM710 + various ARM7500.
	 *
	 * On occasion data aborts are mishandled and end up calling
	 * the swi vector.
	 *
	 * If the instruction that caused the exception is not a SWI
	 * then we hit the bug.
	 */
	if ((insn & 0x0f000000) != 0x0f000000) {
		frame->tf_pc -= INSN_SIZE;
		curcpu()->ci_arm700bugcount.ev_count++;
		userret(l);
		return;
	}
#endif	/* CPU_ARM7 */

	uvmexp.syscalls++;

	(*(void(*)(struct trapframe *, struct lwp *, u_int32_t))
	    (p->p_md.md_syscall))(frame, l, insn);
}

#define MAXARGS 8

void syscall_intern(struct proc *);
void syscall_plain(struct trapframe *, struct lwp *, u_int32_t);
void syscall_fancy(struct trapframe *, struct lwp *, u_int32_t);

void
syscall_intern(struct proc *p)
{
#ifdef KTRACE
	if (p->p_traceflag & (KTRFAC_SYSCALL | KTRFAC_SYSRET)) {
		p->p_md.md_syscall = syscall_fancy;
		return;
	}
#endif
#ifdef SYSTRACE
	if (p->p_flag & P_SYSTRACE) {
		p->p_md.md_syscall = syscall_fancy;
		return;
	}
#endif
	p->p_md.md_syscall = syscall_plain;
}

void
syscall_plain(struct trapframe *frame, struct lwp *l, u_int32_t insn)
{
	struct proc *p = l->l_proc;
	const struct sysent *callp;
	int code, error;
	u_int nap, nargs;
	register_t *ap, *args, copyargs[MAXARGS], rval[2];

	KERNEL_PROC_LOCK(p);

	switch (insn & SWI_OS_MASK) { /* Which OS is the SWI from? */
	case SWI_OS_ARM: /* ARM-defined SWIs */
		code = insn & 0x00ffffff;
		switch (code) {
		case SWI_IMB:
		case SWI_IMBrange:
			/*
			 * Do nothing as there is no prefetch unit that needs
			 * flushing
			 */
			break;
		default:
			/* Undefined so illegal instruction */
			trapsignal(l, SIGILL, insn);
			break;
		}

		userret(l);
		return;
	case 0x000000: /* Old unofficial NetBSD range. */
	case SWI_OS_NETBSD: /* New official NetBSD range. */
		nap = 4;
		break;
	default:
		/* Undefined so illegal instruction */
		trapsignal(l, SIGILL, insn);
		userret(l);
		return;
	}

	code = insn & 0x000fffff;

	ap = &frame->tf_r0;
	callp = p->p_emul->e_sysent;

	switch (code) {	
	case SYS_syscall:
		code = *ap++;
		nap--;
		break;
        case SYS___syscall:
		code = ap[_QUAD_LOWWORD];
		ap += 2;
		nap -= 2;
		break;
	}

	code &= (SYS_NSYSENT - 1);
	callp += code;
	nargs = callp->sy_argsize / sizeof(register_t);
	if (nargs <= nap)
		args = ap;
	else {
		KASSERT(nargs <= MAXARGS);
		memcpy(copyargs, ap, nap * sizeof(register_t));
		error = copyin((void *)frame->tf_usr_sp, copyargs + nap,
		    (nargs - nap) * sizeof(register_t));
		if (error)
			goto bad;
		args = copyargs;
	}

	rval[0] = 0;
	rval[1] = 0;
	error = (*callp->sy_call)(l, args, rval);

	switch (error) {
	case 0:
		frame->tf_r0 = rval[0];
		frame->tf_r1 = rval[1];

#ifdef __PROG32
		frame->tf_spsr &= ~PSR_C_bit;	/* carry bit */
#else
		frame->tf_r15 &= ~R15_FLAG_C;	/* carry bit */
#endif
		break;

	case ERESTART:
		/*
		 * Reconstruct the pc to point at the swi.
		 */
		frame->tf_pc -= INSN_SIZE;
		break;

	case EJUSTRETURN:
		/* nothing to do */
		break;

	default:
	bad:
		frame->tf_r0 = error;
#ifdef __PROG32
		frame->tf_spsr |= PSR_C_bit;	/* carry bit */
#else
		frame->tf_r15 |= R15_FLAG_C;	/* carry bit */
#endif
		break;
	}

	KERNEL_PROC_UNLOCK(l);
	userret(l);
}

void
syscall_fancy(struct trapframe *frame, struct lwp *l, u_int32_t insn)
{
	struct proc *p = l->l_proc;
	const struct sysent *callp;
	int code, error;
	u_int nap, nargs;
	register_t *ap, *args, copyargs[MAXARGS], rval[2];

	KERNEL_PROC_LOCK(p);

	switch (insn & SWI_OS_MASK) { /* Which OS is the SWI from? */
	case SWI_OS_ARM: /* ARM-defined SWIs */
		code = insn & 0x00ffffff;
		switch (code) {
		case SWI_IMB:
		case SWI_IMBrange:
			/*
			 * Do nothing as there is no prefetch unit that needs
			 * flushing
			 */
			break;
		default:
			/* Undefined so illegal instruction */
			trapsignal(l, SIGILL, insn);
			break;
		}

		userret(l);
		return;
	case 0x000000: /* Old unofficial NetBSD range. */
	case SWI_OS_NETBSD: /* New official NetBSD range. */
		nap = 4;
		break;
	default:
		/* Undefined so illegal instruction */
		trapsignal(l, SIGILL, insn);
		userret(l);
		return;
	}

	code = insn & 0x000fffff;

	ap = &frame->tf_r0;
	callp = p->p_emul->e_sysent;

	switch (code) {	
	case SYS_syscall:
		code = *ap++;
		nap--;
		break;
        case SYS___syscall:
		code = ap[_QUAD_LOWWORD];
		ap += 2;
		nap -= 2;
		break;
	}

	code &= (SYS_NSYSENT - 1);
	callp += code;
	nargs = callp->sy_argsize / sizeof(register_t);
	if (nargs <= nap)
		args = ap;
	else {
		KASSERT(nargs <= MAXARGS);
		memcpy(copyargs, ap, nap * sizeof(register_t));
		error = copyin((void *)frame->tf_usr_sp, copyargs + nap,
		    (nargs - nap) * sizeof(register_t));
		if (error)
			goto bad;
		args = copyargs;
	}

	if ((error = trace_enter(l, code, code, NULL, args, rval)) != 0)
		goto bad;

	rval[0] = 0;
	rval[1] = 0;
	error = (*callp->sy_call)(l, args, rval);

	switch (error) {
	case 0:
		frame->tf_r0 = rval[0];
		frame->tf_r1 = rval[1];

#ifdef __PROG32
		frame->tf_spsr &= ~PSR_C_bit;	/* carry bit */
#else
		frame->tf_r15 &= ~R15_FLAG_C;	/* carry bit */
#endif
		break;

	case ERESTART:
		/*
		 * Reconstruct the pc to point at the swi.
		 */
		frame->tf_pc -= INSN_SIZE;
		break;

	case EJUSTRETURN:
		/* nothing to do */
		break;

	default:
	bad:
		frame->tf_r0 = error;
#ifdef __PROG32
		frame->tf_spsr |= PSR_C_bit;	/* carry bit */
#else
		frame->tf_r15 |= R15_FLAG_C;	/* carry bit */
#endif
		break;
	}

	trace_exit(l, code, args, rval, error);
	KERNEL_PROC_UNLOCK(l);
	userret(l);
}

void
child_return(arg)
	void *arg;
{
	struct lwp *l = arg;
	struct trapframe *frame = l->l_addr->u_pcb.pcb_tf;
#ifdef KTRACE
	struct proc *p = l->l_proc;
#endif

	frame->tf_r0 = 0;
#ifdef __PROG32
	frame->tf_spsr &= ~PSR_C_bit;	/* carry bit */
#else
	frame->tf_r15 &= ~R15_FLAG_C;	/* carry bit */
#endif

	KERNEL_PROC_UNLOCK(l);
	userret(l);
#ifdef KTRACE
	if (KTRPOINT(p, KTR_SYSRET)) {
		KERNEL_PROC_LOCK(p);
		ktrsysret(p, SYS_fork, 0, 0);
		KERNEL_PROC_UNLOCK(p);
	}
#endif
}
