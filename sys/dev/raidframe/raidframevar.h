/*	$NetBSD: raidframevar.h,v 1.1 2001/10/04 15:43:58 oster Exp $ */
/*-
 * Copyright (c) 1996, 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Greg Oster
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
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Mark Holland
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

/*
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Jim Zelenka
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

/*****************************************************
 *
 * raidframevar.h
 *
 * main header file for using raidframe in the kernel.
 *
 *****************************************************/


#ifndef _RF_RAIDFRAMEVAR_H_
#define _RF_RAIDFRAMEVAR_H_


#include <sys/ioctl.h>

#include <sys/errno.h>
#include <sys/types.h>

#include <sys/uio.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/lock.h>

/*
 * First, define system-dependent types and constants.
 *
 * If the machine is big-endian, RF_BIG_ENDIAN should be 1.
 * Otherwise, it should be 0.
 *
 * The various integer types should be self-explanatory; we
 * use these elsewhere to avoid size confusion.
 *
 * LONGSHIFT is lg(sizeof(long)) (that is, log base two of sizeof(long)
 *
 */

#include <sys/types.h>
#include <machine/endian.h>
#include <machine/limits.h>

#if BYTE_ORDER == BIG_ENDIAN
#define RF_IS_BIG_ENDIAN    1
#elif BYTE_ORDER == LITTLE_ENDIAN
#define RF_IS_BIG_ENDIAN    0
#else
#error byte order not defined
#endif
typedef int8_t RF_int8;
typedef u_int8_t RF_uint8;
typedef int16_t RF_int16;
typedef u_int16_t RF_uint16;
typedef int32_t RF_int32;
typedef u_int32_t RF_uint32;
typedef int64_t RF_int64;
typedef u_int64_t RF_uint64;
#if LONG_BIT == 32
#define RF_LONGSHIFT        2
#elif LONG_BIT == 64
#define RF_LONGSHIFT        3
#else
#error word size not defined
#endif

/*
 * These are just zero and non-zero. We don't use "TRUE"
 * and "FALSE" because there's too much nonsense trying
 * to get them defined exactly once on every platform, given
 * the different places they may be defined in system header
 * files.
 */
#define RF_TRUE  1
#define RF_FALSE 0

/*
 * Now, some generic types
 */
typedef RF_uint64 RF_IoCount_t;
typedef RF_uint64 RF_Offset_t;
typedef RF_uint32 RF_PSSFlags_t;
typedef RF_uint64 RF_SectorCount_t;
typedef RF_uint64 RF_StripeCount_t;
typedef RF_int64 RF_SectorNum_t;/* these are unsigned so we can set them to
				 * (-1) for "uninitialized" */
typedef RF_int64 RF_StripeNum_t;
typedef RF_int64 RF_RaidAddr_t;
typedef int RF_RowCol_t;	/* unsigned so it can be (-1) */
typedef RF_int64 RF_HeadSepLimit_t;
typedef RF_int64 RF_ReconUnitCount_t;
typedef int RF_ReconUnitNum_t;

typedef char RF_ParityConfig_t;

typedef char RF_DiskQueueType_t[1024];
#define RF_DISK_QUEUE_TYPE_NONE ""

/* values for the 'type' field in a reconstruction buffer */
typedef int RF_RbufType_t;
#define RF_RBUF_TYPE_EXCLUSIVE   0	/* this buf assigned exclusively to
					 * one disk */
#define RF_RBUF_TYPE_FLOATING    1	/* this is a floating recon buf */
#define RF_RBUF_TYPE_FORCED      2	/* this rbuf was allocated to complete
					 * a forced recon */

typedef char RF_IoType_t;
#define RF_IO_TYPE_READ          'r'
#define RF_IO_TYPE_WRITE         'w'
#define RF_IO_TYPE_NOP           'n'
#define RF_IO_IS_R_OR_W(_type_) (((_type_) == RF_IO_TYPE_READ) \
                                || ((_type_) == RF_IO_TYPE_WRITE))

typedef void (*RF_VoidFuncPtr) (void *,...);

typedef RF_uint32 RF_AccessStripeMapFlags_t;
typedef RF_uint32 RF_DiskQueueDataFlags_t;
typedef RF_uint32 RF_DiskQueueFlags_t;
typedef RF_uint32 RF_RaidAccessFlags_t;

#define RF_DISKQUEUE_DATA_FLAGS_NONE ((RF_DiskQueueDataFlags_t)0)

typedef struct RF_AccessStripeMap_s RF_AccessStripeMap_t;
typedef struct RF_AccessStripeMapHeader_s RF_AccessStripeMapHeader_t;
typedef struct RF_AllocListElem_s RF_AllocListElem_t;
typedef struct RF_CallbackDesc_s RF_CallbackDesc_t;
typedef struct RF_ChunkDesc_s RF_ChunkDesc_t;
typedef struct RF_CommonLogData_s RF_CommonLogData_t;
typedef struct RF_Config_s RF_Config_t;
typedef struct RF_CumulativeStats_s RF_CumulativeStats_t;
typedef struct RF_DagHeader_s RF_DagHeader_t;
typedef struct RF_DagList_s RF_DagList_t;
typedef struct RF_DagNode_s RF_DagNode_t;
typedef struct RF_DeclusteredConfigInfo_s RF_DeclusteredConfigInfo_t;
typedef struct RF_DiskId_s RF_DiskId_t;
typedef struct RF_DiskMap_s RF_DiskMap_t;
typedef struct RF_DiskQueue_s RF_DiskQueue_t;
typedef struct RF_DiskQueueData_s RF_DiskQueueData_t;
typedef struct RF_DiskQueueSW_s RF_DiskQueueSW_t;
typedef struct RF_Etimer_s RF_Etimer_t;
typedef struct RF_EventCreate_s RF_EventCreate_t;
typedef struct RF_FreeList_s RF_FreeList_t;
typedef struct RF_LockReqDesc_s RF_LockReqDesc_t;
typedef struct RF_LockTableEntry_s RF_LockTableEntry_t;
typedef struct RF_MCPair_s RF_MCPair_t;
typedef struct RF_OwnerInfo_s RF_OwnerInfo_t;
typedef struct RF_ParityLog_s RF_ParityLog_t;
typedef struct RF_ParityLogAppendQueue_s RF_ParityLogAppendQueue_t;
typedef struct RF_ParityLogData_s RF_ParityLogData_t;
typedef struct RF_ParityLogDiskQueue_s RF_ParityLogDiskQueue_t;
typedef struct RF_ParityLogQueue_s RF_ParityLogQueue_t;
typedef struct RF_ParityLogRecord_s RF_ParityLogRecord_t;
typedef struct RF_PerDiskReconCtrl_s RF_PerDiskReconCtrl_t;
typedef struct RF_PSStatusHeader_s RF_PSStatusHeader_t;
typedef struct RF_PhysDiskAddr_s RF_PhysDiskAddr_t;
typedef struct RF_PropHeader_s RF_PropHeader_t;
typedef struct RF_Raid_s RF_Raid_t;
typedef struct RF_RaidAccessDesc_s RF_RaidAccessDesc_t;
typedef struct RF_RaidDisk_s RF_RaidDisk_t;
typedef struct RF_RaidLayout_s RF_RaidLayout_t;
typedef struct RF_RaidReconDesc_s RF_RaidReconDesc_t;
typedef struct RF_ReconBuffer_s RF_ReconBuffer_t;
typedef struct RF_ReconConfig_s RF_ReconConfig_t;
typedef struct RF_ReconCtrl_s RF_ReconCtrl_t;
typedef struct RF_ReconDoneProc_s RF_ReconDoneProc_t;
typedef struct RF_ReconEvent_s RF_ReconEvent_t;
typedef struct RF_ReconMap_s RF_ReconMap_t;
typedef struct RF_ReconMapListElem_s RF_ReconMapListElem_t;
typedef struct RF_ReconParityStripeStatus_s RF_ReconParityStripeStatus_t;
typedef struct RF_RedFuncs_s RF_RedFuncs_t;
typedef struct RF_RegionBufferQueue_s RF_RegionBufferQueue_t;
typedef struct RF_RegionInfo_s RF_RegionInfo_t;
typedef struct RF_ShutdownList_s RF_ShutdownList_t;
typedef struct RF_SpareTableEntry_s RF_SpareTableEntry_t;
typedef struct RF_SparetWait_s RF_SparetWait_t;
typedef struct RF_StripeLockDesc_s RF_StripeLockDesc_t;
typedef struct RF_ThreadGroup_s RF_ThreadGroup_t;
typedef struct RF_ThroughputStats_s RF_ThroughputStats_t;

/*
 * Important assumptions regarding ordering of the states in this list
 * have been made!!!  Before disturbing this ordering, look at code in 
 * sys/dev/raidframe/rf_states.c
 */
typedef enum RF_AccessState_e {
	/* original states */
	rf_QuiesceState,	    /* handles queisence for reconstruction */
	rf_IncrAccessesCountState,  /* count accesses in flight */
	rf_DecrAccessesCountState,
	rf_MapState,		    /* map access to disk addresses */
	rf_LockState,		    /* take stripe locks */
	rf_CreateDAGState,	    /* create DAGs */
	rf_ExecuteDAGState,	    /* execute DAGs */
	rf_ProcessDAGState,	    /* DAGs are completing- check if correct,
				     * or if we need to retry */
	rf_CleanupState,	    /* release stripe locks, clean up */
	rf_LastState		    /* must be the last state */
}       RF_AccessState_t;


/* Some constants related to RAIDframe.  These are arbitrary and 
   can be modified at will. */

#define RF_MAXROW    10
#define RF_MAXCOL    40
#define RF_MAXSPARE  10
#define RF_MAXDBGV   75		    /* max number of debug variables */
#define RF_MAX_DISKS 128	    /* max disks per array */
#define RF_SPAREMAP_NAME_LEN 128
#define RF_PROTECTED_SECTORS 64L    /* # of sectors at start of disk to
				       exclude from RAID address space */

struct RF_SpareTableEntry_s {
        u_int   spareDisk;          /* disk to which this block is spared */
        u_int   spareBlockOffsetInSUs;  /* offset into spare table for that
                                         * disk */
};

union RF_GenericParam_u {
	void   *p;
	RF_uint64 v;
};
typedef union RF_GenericParam_u RF_DagParam_t;
typedef union RF_GenericParam_u RF_CBParam_t;

/* the raidframe configuration, passed down through an ioctl.
 * the driver can be reconfigured (with total loss of data) at any time,
 * but it must be shut down first.
 */
struct RF_Config_s {
	RF_RowCol_t numRow, numCol, numSpare;	/* number of rows, columns,
						 * and spare disks */
	dev_t   devs[RF_MAXROW][RF_MAXCOL];	/* device numbers for disks
						 * comprising array */
	char    devnames[RF_MAXROW][RF_MAXCOL][50];	/* device names */
	dev_t   spare_devs[RF_MAXSPARE];	/* device numbers for spare
						 * disks */
	char    spare_names[RF_MAXSPARE][50];	/* device names */
	RF_SectorNum_t sectPerSU;	/* sectors per stripe unit */
	RF_StripeNum_t SUsPerPU;/* stripe units per parity unit */
	RF_StripeNum_t SUsPerRU;/* stripe units per reconstruction unit */
	RF_ParityConfig_t parityConfig;	/* identifies the RAID architecture to
					 * be used */
	RF_DiskQueueType_t diskQueueType;	/* 'f' = fifo, 'c' = cvscan,
						 * not used in kernel */
	char    maxOutstandingDiskReqs;	/* # concurrent reqs to be sent to a
					 * disk.  not used in kernel. */
	char    debugVars[RF_MAXDBGV][50];	/* space for specifying debug
						 * variables & their values */
	unsigned int layoutSpecificSize;	/* size in bytes of
						 * layout-specific info */
	void   *layoutSpecific;	/* a pointer to a layout-specific structure to
				 * be copied in */
	int     force;                          /* if !0, ignore many fatal
						   configuration conditions */
	/* 
	   "force" is used to override cases where the component labels would 
	   indicate that configuration should not proceed without user 
	   intervention
	 */
};

typedef RF_uint32 RF_ReconReqFlags_t;
/* flags that can be put in the rf_recon_req structure */
#define RF_FDFLAGS_NONE   0x0	/* just fail the disk */
#define RF_FDFLAGS_RECON  0x1	/* fail and initiate recon */

struct rf_recon_req {		/* used to tell the kernel to fail a disk */
	RF_RowCol_t row, col;
	RF_ReconReqFlags_t flags;
	void   *raidPtr;	/* used internally; need not be set at ioctl
				 * time */
	struct rf_recon_req *next;	/* used internally; need not be set at
					 * ioctl time */
};

struct RF_SparetWait_s {
	int     C, G, fcol;	/* C = # disks in row, G = # units in stripe,
				 * fcol = which disk has failed */

	RF_StripeCount_t SUsPerPU;	/* this stuff is the info required to
					 * create a spare table */
	int     TablesPerSpareRegion;
	int     BlocksPerTable;
	RF_StripeCount_t TableDepthInPUs;
	RF_StripeCount_t SpareSpaceDepthPerRegionInSUs;

	RF_SparetWait_t *next;	/* used internally; need not be set at ioctl
				 * time */
};
/*
 * A physical disk can be in one of several states:
 * IF YOU ADD A STATE, CHECK TO SEE IF YOU NEED TO MODIFY RF_DEAD_DISK().
 */
enum RF_DiskStatus_e {
        rf_ds_optimal,          /* no problems */
        rf_ds_failed,           /* reconstruction ongoing */
        rf_ds_reconstructing,   /* reconstruction complete to spare, dead disk
                                 * not yet replaced */
        rf_ds_dist_spared,      /* reconstruction complete to distributed
                                 * spare space, dead disk not yet replaced */
        rf_ds_spared,           /* reconstruction complete to distributed
                                 * spare space, dead disk not yet replaced */
        rf_ds_spare,            /* an available spare disk */
        rf_ds_used_spare        /* a spare which has been used, and hence is
                                 * not available */
};
typedef enum RF_DiskStatus_e RF_DiskStatus_t;

struct RF_RaidDisk_s {
        char    devname[56];    /* name of device file */
        RF_DiskStatus_t status; /* whether it is up or down */
        RF_RowCol_t spareRow;   /* if in status "spared", this identifies the
                                 * spare disk */
        RF_RowCol_t spareCol;   /* if in status "spared", this identifies the
                                 * spare disk */
        RF_SectorCount_t numBlocks;     /* number of blocks, obtained via READ
                                         * CAPACITY */
        int     blockSize;
        RF_SectorCount_t partitionSize; /* The *actual* and *full* size of 
                                           the partition, from the disklabel */
        int     auto_configured;/* 1 if this component was autoconfigured.
                                   0 otherwise. */
        dev_t   dev;
};
/* The per-component label information that the user can set */
typedef struct RF_ComponentInfo_s {
	int row;              /* the row number of this component */
	int column;           /* the column number of this component */
	int serial_number;    /* a user-specified serial number for this
				 RAID set */
} RF_ComponentInfo_t;

/* The per-component label information */
typedef struct RF_ComponentLabel_s {
	int version;          /* The version of this label. */
	int serial_number;    /* a user-specified serial number for this
				 RAID set */
	int mod_counter;      /* modification counter.  Changed (usually
				 by incrementing) every time the label 
				 is changed */
	int row;              /* the row number of this component */
	int column;           /* the column number of this component */
	int num_rows;         /* number of rows in this RAID set */
	int num_columns;      /* number of columns in this RAID set */
	int clean;            /* 1 when clean, 0 when dirty */
	int status;           /* rf_ds_optimal, rf_ds_dist_spared, whatever. */
	/* stuff that will be in version 2 of the label */
	int sectPerSU;        /* Sectors per Stripe Unit */
	int SUsPerPU;         /* Stripe Units per Parity Units */
	int SUsPerRU;         /* Stripe Units per Reconstruction Units */
	int parityConfig;     /* '0' == RAID0, '1' == RAID1, etc. */
	int maxOutstanding;   /* maxOutstanding disk requests */
	int blockSize;        /* size of component block. 
				 (disklabel->d_secsize) */
	int numBlocks;        /* number of blocks on this component.  May
			         be smaller than the partition size. */
	int partitionSize;    /* number of blocks on this *partition*. 
				 Must exactly match the partition size
				 from the disklabel. */
	int future_use[33];   /* Future expansion */
	int autoconfigure;    /* automatically configure this RAID set. 
				 0 == no, 1 == yes */
	int root_partition;   /* Use this set as /
				 0 == no, 1 == yes*/
	int last_unit;        /* last unit number (e.g. 0 for /dev/raid0) 
				 of this component.  Used for autoconfigure
				 only. */
	int config_order;     /* 0 .. n.  The order in which the component
				 should be auto-configured.  E.g. 0 is will 
				 done first, (and would become raid0).
				 This may be in conflict with last_unit!!?! */
	                      /* Not currently used. */
	int future_use2[44];  /* More future expansion */
} RF_ComponentLabel_t;

typedef struct RF_SingleComponent_s {
	int row;
	int column;
	char component_name[50]; /* name of the component */
} RF_SingleComponent_t; 

typedef struct RF_DeviceConfig_s {
	u_int   rows;
	u_int   cols;
	u_int   maxqdepth;
	int     ndevs;
	RF_RaidDisk_t devs[RF_MAX_DISKS];
	int     nspares;
	RF_RaidDisk_t spares[RF_MAX_DISKS];
}       RF_DeviceConfig_t;

typedef struct RF_ProgressInfo_s {
	RF_uint64 remaining;
	RF_uint64 completed;
	RF_uint64 total;
} RF_ProgressInfo_t;

typedef struct RF_LayoutSW_s {
	RF_ParityConfig_t parityConfig;
	const char *configName;

#ifndef _KERNEL
	/* layout-specific parsing */
	int     (*MakeLayoutSpecific) (FILE * fp, RF_Config_t * cfgPtr, 
				       void *arg);
	void   *makeLayoutSpecificArg;
#else				/* !KERNEL */

	/* initialization routine */
	int     (*Configure) (RF_ShutdownList_t ** shutdownListp, 
			      RF_Raid_t * raidPtr, RF_Config_t * cfgPtr);

	/* routine to map RAID sector address -> physical (row, col, offset) */
	void    (*MapSector) (RF_Raid_t * raidPtr, RF_RaidAddr_t raidSector,
			      RF_RowCol_t * row, RF_RowCol_t * col, 
			      RF_SectorNum_t * diskSector, int remap);

	/* routine to map RAID sector address -> physical (r,c,o) of parity
	 * unit */
	void    (*MapParity) (RF_Raid_t * raidPtr, RF_RaidAddr_t raidSector,
			      RF_RowCol_t * row, RF_RowCol_t * col, 
			      RF_SectorNum_t * diskSector, int remap);

	/* routine to map RAID sector address -> physical (r,c,o) of Q unit */
	void    (*MapQ) (RF_Raid_t * raidPtr, RF_RaidAddr_t raidSector, 
			 RF_RowCol_t * row, RF_RowCol_t * col, 
			 RF_SectorNum_t * diskSector, int remap);

	/* routine to identify the disks comprising a stripe */
	void    (*IdentifyStripe) (RF_Raid_t * raidPtr, RF_RaidAddr_t addr,
				   RF_RowCol_t ** diskids, 
				   RF_RowCol_t * outRow);

	/* routine to select a dag */
	void    (*SelectionFunc) (RF_Raid_t * raidPtr, RF_IoType_t type,
				  RF_AccessStripeMap_t * asmap,
				  RF_VoidFuncPtr *);

	/* map a stripe ID to a parity stripe ID.  This is typically the
	 * identity mapping */
	void    (*MapSIDToPSID) (RF_RaidLayout_t * layoutPtr, 
				 RF_StripeNum_t stripeID,
				 RF_StripeNum_t * psID, 
				 RF_ReconUnitNum_t * which_ru);

	/* get default head separation limit (may be NULL) */
	RF_HeadSepLimit_t(*GetDefaultHeadSepLimit) (RF_Raid_t * raidPtr);

	/* get default num recon buffers (may be NULL) */
	int     (*GetDefaultNumFloatingReconBuffers) (RF_Raid_t * raidPtr);

	/* get number of spare recon units (may be NULL) */
	RF_ReconUnitCount_t(*GetNumSpareRUs) (RF_Raid_t * raidPtr);

	/* spare table installation (may be NULL) */
	int     (*InstallSpareTable) (RF_Raid_t * raidPtr, RF_RowCol_t frow, 
				      RF_RowCol_t fcol);

	/* recon buffer submission function */
	int     (*SubmitReconBuffer) (RF_ReconBuffer_t * rbuf, int keep_it,
				      int use_committed);

	/*
         * verify that parity information for a stripe is correct
         * see rf_parityscan.h for return vals
         */
	int     (*VerifyParity) (RF_Raid_t * raidPtr, RF_RaidAddr_t raidAddr,
				 RF_PhysDiskAddr_t * parityPDA, 
				 int correct_it, RF_RaidAccessFlags_t flags);

	/* number of faults tolerated by this mapping */
	int     faultsTolerated;

	/* states to step through in an access. Must end with "LastState". The
	 * default is DefaultStates in rf_layout.c */
	RF_AccessState_t *states;

	RF_AccessStripeMapFlags_t flags;
#endif				/* !KERNEL */
}       RF_LayoutSW_t;

#endif				/* !_RF_RAIDFRAMEVAR_H_ */
