/*      $NetBSD: vm_machdep.c,v 1.36 1997/11/04 20:52:27 ragge Exp $       */

/*
 * Copyright (c) 1994 Ludd, University of Lule}, Sweden.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed at Ludd, University of Lule}.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/exec.h>
#include <sys/vnode.h>
#include <sys/core.h>
#include <sys/mount.h>
#include <sys/device.h>

#include <vm/vm.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>

#include <machine/vmparam.h>
#include <machine/mtpr.h>
#include <machine/pmap.h>
#include <machine/pte.h>
#include <machine/macros.h>
#include <machine/trap.h>
#include <machine/pcb.h>
#include <machine/frame.h>
#include <machine/cpu.h>
#include <machine/sid.h>

#include <sys/syscallargs.h>

volatile int whichqs;

/*
 * pagemove - moves pages at virtual address from to virtual address to,
 * block moved of size size. Using fast insn bcopy for pte move.
 */
void
pagemove(from, to, size)
	caddr_t from, to;
	size_t size;
{
	pt_entry_t *fpte, *tpte;
	int	stor;

	fpte = kvtopte(from);
	tpte = kvtopte(to);

	stor = (size >> PGSHIFT) * sizeof(struct pte);
	bcopy(fpte, tpte, stor);
	bzero(fpte, stor);
	mtpr(0, PR_TBIA);
}

/*
 * cpu_fork() copies parent process trapframe directly into child PCB
 * so that when we swtch() to the child process it will go directly
 * back to user mode without any need to jump back through kernel.
 * We also take away mapping for the second page after pcb, so that
 * we get something like a "red zone".
 * No need for either double-map kernel stack or relocate it when
 * forking.
 */
void
cpu_fork(p1, p2)
	struct proc *p1, *p2;
{
	struct pcb *nyproc;
	struct trapframe *tf;
	struct pmap *pmap, *opmap;
	extern vm_map_t pte_map;
	int bytesiz;

	nyproc = &p2->p_addr->u_pcb;
	tf = p1->p_addr->u_pcb.framep;
	opmap = p1->p_vmspace->vm_map.pmap;
	pmap = p2->p_vmspace->vm_map.pmap;
	pmap->pm_pcb = nyproc;

#ifdef notyet
	/* Mark page invalid */
	p2pte = kvtopte((u_int *)p2->p_addr + 2 * NBPG);
	*p2pte = 0; 
#endif

	/* Set up internal defs in PCB, and alloc PTEs. */
	bytesiz = btoc(MAXTSIZ + MAXDSIZ + MMAPSPACE + MAXSSIZ) *
	    sizeof(struct pte);
	nyproc->P0BR = (void *)kmem_alloc_wait(pte_map, bytesiz);
	nyproc->P0LR = btoc(MAXTSIZ + MAXDSIZ + MMAPSPACE) | AST_PCB;
	nyproc->P1BR = (void *)nyproc->P0BR + bytesiz - 0x800000;
	nyproc->P1LR = (0x200000 - btoc(MAXSSIZ));

	nyproc->iftrap = NULL;
	nyproc->KSP = (u_int)p2->p_addr + USPACE;

	/* General registers as taken from userspace */
	/* trapframe should be synced with pcb */
	bcopy(&tf->r2,&nyproc->R[2],10*sizeof(int));
	nyproc->AP = tf->ap;
	nyproc->FP = tf->fp;
	nyproc->USP = tf->sp;
	nyproc->PC = tf->pc;
	nyproc->PSL = tf->psl & ~PSL_C;
	nyproc->R[0] = p1->p_pid; /* parent pid. (shouldn't be needed) */
	nyproc->R[1] = 1;

	return; /* Child is ready. Parent, return! */
}

/*
 * cpu_set_kpc() sets up pcb for the new kernel process so that it will
 * start at the procedure pointed to by pc next time swtch() is called.
 * When that procedure returns, it will pop off everything from the
 * faked calls frame on the kernel stack, do an REI and go down to
 * user mode.
 */
void
cpu_set_kpc(p, pc)
	struct proc *p;
	void (*pc) __P((struct proc *));
{
	struct pcb *nyproc;
	struct {
		struct	callsframe cf;
		struct	trapframe tf;
	} *kc;
	extern int sret, boothowto;

	nyproc = &p->p_addr->u_pcb;
	(unsigned)kc = nyproc->FP = nyproc->KSP =
	    (unsigned)p->p_addr + USPACE - sizeof(*kc);
	kc->cf.ca_cond = 0;
	kc->cf.ca_maskpsw = 0x20000000;
	kc->cf.ca_pc = (unsigned)&sret;
	kc->cf.ca_argno = 1;
	kc->cf.ca_arg1 = (unsigned)p;
	kc->tf.r11 = boothowto;	/* If we have old init */
	kc->tf.psl = 0x3c00000;

	nyproc->framep = (void *)&kc->tf;
	nyproc->AP = (unsigned)&kc->cf.ca_argno;
	nyproc->FP = nyproc->KSP = (unsigned)kc;
	nyproc->PC = (unsigned)pc + 2;
}

int	reno_zmagic __P((struct proc *, struct exec_package *));

int
cpu_exec_aout_makecmds(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	int error;
	struct exec *ep;
	/*
	 * Compatibility with reno programs.
	 */
	ep=epp->ep_hdr;
	switch (ep->a_midmag) {
	case 0x10b: /* ZMAGIC in 4.3BSD Reno programs */
		error = reno_zmagic(p, epp);
		break;
	case 0x108:
printf("Warning: reno_nmagic\n");
		error = exec_aout_prep_nmagic(p, epp);
		break;
	case 0x107:
printf("Warning: reno_omagic\n");
		error = exec_aout_prep_omagic(p, epp);
		break;
	default:
		error = ENOEXEC;
	}
	return(error);
}

int
sys_sysarch(p, v, retval)
	struct proc *p;
	void *v;
	register_t *retval;
{

	return (ENOSYS);
};

#ifdef COMPAT_ULTRIX
extern struct emul emul_ultrix;
#endif
/*
 * 4.3BSD Reno programs have an 1K header first in the executable
 * file, containing a.out header. Otherwise programs are identical.
 *
 *      from: exec_aout.c,v 1.9 1994/01/28 23:46:59 jtc Exp $
 */

int
reno_zmagic(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	struct exec *execp = epp->ep_hdr;

	epp->ep_taddr = 0;
	epp->ep_tsize = execp->a_text;
	epp->ep_daddr = epp->ep_taddr + execp->a_text;
	epp->ep_dsize = execp->a_data + execp->a_bss;
	epp->ep_entry = execp->a_entry;

#ifdef COMPAT_ULTRIX
	epp->ep_emul = &emul_ultrix;
#endif

	/*
	 * check if vnode is in open for writing, because we want to
	 * demand-page out of it.  if it is, don't do it, for various
	 * reasons
	 */
	if ((execp->a_text != 0 || execp->a_data != 0) &&
	    epp->ep_vp->v_writecount != 0) {
		return ETXTBSY;
	}
	epp->ep_vp->v_flag |= VTEXT;

	/* set up command for text segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_pagedvn, execp->a_text,
	    epp->ep_taddr, epp->ep_vp, 0x400, VM_PROT_READ|VM_PROT_EXECUTE);

	/* set up command for data segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_pagedvn, execp->a_data,
	    epp->ep_daddr, epp->ep_vp, execp->a_text+0x400,
	    VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	/* set up command for bss segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_zero, execp->a_bss,
	    epp->ep_daddr + execp->a_data, NULLVP, 0,
	    VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	return exec_aout_setup_stack(p, epp);
}

/*
 * Dump the machine specific header information at the start of a core dump.
 * First put all regs in PCB for debugging purposes. This is not an good
 * way to do this, but good for my purposes so far.
 */
int
cpu_coredump(p, vp, cred, chdr)
	struct proc *p;
	struct vnode *vp;
	struct ucred *cred;
	struct core *chdr;
{
	struct trapframe *tf;
	struct md_coredump state;
	struct coreseg cseg;
	int error;

	tf = p->p_addr->u_pcb.framep;
	CORE_SETMAGIC(*chdr, COREMAGIC, MID_VAX, 0);
	chdr->c_hdrsize = sizeof(struct core);
	chdr->c_seghdrsize = sizeof(struct coreseg);
	chdr->c_cpusize = sizeof(struct md_coredump);

	bcopy(tf, &state, sizeof(struct md_coredump));

	CORE_SETMAGIC(cseg, CORESEGMAGIC, MID_VAX, CORE_CPU);
	cseg.c_addr = 0;
	cseg.c_size = chdr->c_cpusize;

	error = vn_rdwr(UIO_WRITE, vp, (caddr_t)&cseg, chdr->c_seghdrsize,
	    (off_t)chdr->c_hdrsize, UIO_SYSSPACE,
	    IO_NODELOCKED|IO_UNIT, cred, (int *)NULL, p);
	if (error)
		return error;

        error = vn_rdwr(UIO_WRITE, vp, (caddr_t)&state, sizeof(state),
            (off_t)(chdr->c_hdrsize + chdr->c_seghdrsize), UIO_SYSSPACE,
            IO_NODELOCKED|IO_UNIT, cred, (int *)NULL, p);

        if (!error)
                chdr->c_nseg++;

        return error;
}

/*
 * cpu_swapin() is called just before a process shall be swapped in.
 * Kernel stack and pcb must be mapped when we swtch() to this new
 * process, to guarantee that we frob all pages here to ensure that
 * they actually are in-core. Kernel stack red zone is also updated
 * here.
 */
void
cpu_swapin(p)
	struct proc *p;
{
	u_int uarea, i, *j, rv;

	uarea = (u_int)p->p_addr;

	for (i = uarea;i < uarea + USPACE;i += PAGE_SIZE) {
		j = (u_int *)kvtopte(i);
		if ((*j & PG_V) == 0) {
			rv = vm_fault(kernel_map, i,
			    VM_PROT_WRITE|VM_PROT_READ, FALSE);
			if (rv != KERN_SUCCESS)
				panic("cpu_swapin: rv %d",rv);
		}
	}
#ifdef notyet
	j = (u_int *)kvtopte(uarea + 2 * NBPG);
	*j = 0; /* Set kernel stack red zone */
#endif
}

#if VAX410 || VAX43
/*
 * vmapbuf()/vunmapbuf() only used on some vaxstations without
 * any busadapter with MMU.
 * XXX - This must be reworked to be effective.
 */
void
vmapbuf(bp, len)
        struct buf *bp;
        vm_size_t len;
{
        vm_offset_t faddr, taddr, off, pa;
        pmap_t fmap, tmap;

	if ((vax_boardtype != VAX_BTYP_43) && (vax_boardtype != VAX_BTYP_410))
		return;
        faddr = trunc_page(bp->b_saveaddr = bp->b_data);
        off = (vm_offset_t)bp->b_data - faddr;
        len = round_page(off + len);
        taddr = kmem_alloc_wait(phys_map, len);
        bp->b_data = (caddr_t)(taddr + off);
        fmap = vm_map_pmap(&bp->b_proc->p_vmspace->vm_map);
        tmap = vm_map_pmap(phys_map);
        len = len >> PGSHIFT;
        while (len--) {
                pa = pmap_extract(fmap, faddr);
                if (pa == 0)
                       	panic("vmapbuf: null page frame for %x", (u_int)faddr);

                pmap_enter(tmap, taddr, pa & ~(NBPG - 1),
                           VM_PROT_READ|VM_PROT_WRITE, TRUE);
                faddr += NBPG;
                taddr += NBPG;
        }
}

/*
 * Free the io map PTEs associated with this IO operation.
 * We also invalidate the TLB entries and restore the original b_addr.
 */
void
vunmapbuf(bp, len)
        struct buf *bp;
        vm_size_t len;
{
        vm_offset_t addr, off;

	if ((vax_boardtype != VAX_BTYP_43) && (vax_boardtype != VAX_BTYP_410))
		return;
        addr = trunc_page(bp->b_data);
        off = (vm_offset_t)bp->b_data - addr;
        len = round_page(off + len);
        kmem_free_wakeup(phys_map, addr, len);
        bp->b_data = bp->b_saveaddr;
        bp->b_saveaddr = 0;
}
#endif
