#	$NetBSD: std.cats,v 1.6 2005/09/17 09:44:06 yamt Exp $
#
# standard NetBSD/cats options

machine	cats arm
include		"conf/std"	# MI standard options

options 	EXEC_AOUT
options 	EXEC_SCRIPT
options 	EXEC_ELF32

# To support easy transit to ../arch/arm/arm32
options 	ARM32

#options 	ARM32_NEW_VM_LAYOUT	# Not yet supported

options 	ARM_INTR_IMPL="<arm/footbridge/footbridge_intr.h>"
