############################################################################
#     Copyright (c) 2006-2007, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: system.mak $
# $brcm_Revision: China_RefSW_Devel/1 $
# $brcm_Date: 12/10/07 11:25a $
#
# Module Description:
#
# Revision History:
#
############################################################################

SYSTEM ?= linux

ifeq (${SYSTEM},vxworks)
# TODO: These flags were causing a problem for i386-linux builds of mlibs. Are they
# really needed for Linux?
SYSTEM_FLAGS = -D_POSIX_C_SOURCE=199506 -D_ISOC9X_SOURCE

SYSTEM_FLAGS += -r -nostdlib
SYSTEM_FLAGS += -G 0 -mno-branch-likely -mips3 -EB -fno-builtin -DMIPSEB -DCPU=MIPS64 \
	-DTOOL_FAMILY=gnu -DTOOL=sfgnu -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
SYSTEM_FLAGS += -LC:\Tor21\host\x86-win32\mips-wrs-vxworks\lib\MIPS64gnu
endif

CFLAGS += -DB_SYSTEM_${SYSTEM}=1
ifeq (${SYSTEM},linux)

CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
CFLAGS += -D_GNU_SOURCE=1
CFLAGS += -DLINUX
CFLAGS += -pipe
CFLAGS += -W
UCLIBC_VERSION = $(shell basename $(TOOLCHAIN_DIR)/mipsel-linux/lib/libuClibc*)
CFLAGS += -DUCLIBC_VERSION=\"$(UCLIBC_VERSION)\"
CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
endif

ifeq ($(SYSTEM),linuxkernel)
CFLAGS += -Wall
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
CFLAGS += -nostdinc -mips2 -fomit-frame-pointer -fno-strict-aliasing  -G 0 -mno-abicalls -fno-pic -Wa,--trap -DMODULE -mlong-calls  
# -O2 is required for kernel inline functions
CFLAGS += -O2
CFLAGS += -fno-common
CFLAGS += -msoft-float
CFLAGS += -I${LINUX_INC} -I${STD_INC} -I${GCC_INC} -DLINUX -D__KERNEL__
CFLAGS += -pipe
endif

