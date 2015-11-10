############################################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tools.mak $
# $brcm_Revision: 23 $
# $brcm_Date: 4/26/10 6:25p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/tools.mak $
# 
# 23   4/26/10 6:25p mward
# SW7125-253: Set CROSS_COMPILE for big-endian.
# 
# 22   4/16/10 10:07a erickson
# SW7400-2511: avoid circular reference if ARCH is defined
#
# 21   4/15/10 2:36p erickson
# SW7400-2511: support B_REFSW_ARCH
#
# 20   4/9/10 4:50p jrubio
# SW7125-253: fix for Big Endian
#
# 19   4/2/10 6:07p mward
# SW7125-253:  stbgcc-4.4.3-1.0 has mipsel-linux-uclibc-gcc not  mipsel-
# uclibc-gcc.
#
# 18   3/24/10 11:16a erickson
# SW7405-4104: remove comment about g++ GPL contamination
#
# 17   11/17/09 2:15p erickson
# SW7405-3408: use $(filter) instead of $(findstring)
#
# 16   3/26/09 10:56a vsilyaev
# PR 53611: Use simply expanded variables everywhere where  is used. It
# prevents evaluation of  every where variable is used
#
# 15   12/12/07 9:30a erickson
# PR32813: changed comment
#
# 14   7/25/07 11:28a vsilyaev
# PR 32813: Added Windows MINGW support
#
# 13   12/15/06 6:27p vsilyaev
# PR 26656: Added BPRofile support
#
# 12   7/19/06 5:39p jgarrett
# PR 19909: Fixing make -j errors
#
# 11   7/14/06 11:31a jgarrett
# PR 19909: Merging to latest baseline
#
# PR19909/2   7/7/06 1:30p jgarrett
# PR 19909: Changed Q_ from ?= to := for vxWorks.
#
# PR19909/1   7/6/06 10:51a jgarrett
# PR 19909: Reducing brutus build output
#
# 9   2/17/06 2:34p rjlewis
# PR19725: v6 and linux versions can use -p option.
#
# 8   1/18/06 11:51a rjlewis
# PR19044: added MV definition (for move).
#
# 7   12/7/05 4:10p hkuhnert
# PR15072: Change chdir to pwd when compiling from linux host.
#
# 6   8/4/05 4:41p erickson
# PR15139: collected generic build configuration into
# BSEAV/build/refsw_inc.mak
#
# 5   2/23/05 2:40p erickson
# PR14180: tools.mak must define SYSTEM too
#
# 4   2/22/05 1:53p erickson
# PR14180: default DEBUG to y, added better ARCH/PLATFORM support for
# vxworks
#
# 3   2/17/05 1:51p erickson
# PR9497: modified ARCH for vxworks to be more meaningful
#
# 2   2/16/05 4:45p erickson
# PR9497: allow dos mkdir to work
#
# 1   2/7/05 6:57p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
#
# Irvine_BSEAVSW_Devel/13   9/10/04 9:44a erickson
# PR12621: use which and dirname to determine TOOLCHAIN_DIR from bottom
# up
#
# Irvine_BSEAVSW_Devel/12   6/24/04 11:40a erickson
# PR11135: converted backticks to $(shell) command
#
# Irvine_BSEAVSW_Devel/11   6/24/04 10:27a erickson
# PR11135: dynamically determine toolchain path
#
# Irvine_BSEAVSW_Devel/10   6/1/04 11:38a erickson
# PR9447: setting default to uclibc
#
# Irvine_BSEAVSW_Devel/9   3/1/04 2:02p erickson
# PR9447: define TOOLCHAIN_DIR in the tool.mak file
#
# Irvine_BSEAVSW_Devel/8   1/30/04 4:39p erickson
# PR9107: added option for ARCH=i386-linux, this should default
# CROSS_COMPILE to nothing
#
# Irvine_BSEAVSW_Devel/7   1/29/04 6:42p vsilyaev
# PR 9447: Default arch is mipsel-linux until further notice.
#
# Irvine_BSEAVSW_Devel/6   1/27/04 5:56p vsilyaev
# PR 9497: Added tools for VxWorks
#
# Irvine_BSEAVSW_Devel/5   1/23/04 11:17a erickson
# PR9447: using common tools.mak so that conversion to uclibc is easier
#
# Irvine_BSEAVSW_Devel/4   1/22/04 4:32p erickson
# PR9447: default ARCH to mipsel-uclibc
#
# Irvine_BSEAVSW_Devel/3   10/28/03 1:13p erickson
# convert from g++ to c++
#
# Irvine_BSEAVSW_Devel/2   10/9/03 2:34p erickson
# fixed uclibc support
#
#
###########################################################################

# This make include defines macros for cross-development tools.
# It is used in a wide range of environments, so don't add anything
# that doesn't belong here.

# ARCH must indentify the CPU and operating system being compiled for.
# If you chose a value of ARCH which doesn't uniquely identify this, you
# will have an unstable or hacked-up build system elsewhere.

# Valid values include:
#  i386-linux = Intel Linux systems(e.g. unix host compiler)
#  i386-mingw = host compiler in Windows Mingw environment
#  mipsel-linux = glibc, little endian Linux on MIPS
#  mips-linux = glibc, big endian Linux on MIPS
#  mipsel-uclibc = uclibc (busybox), little endian Linux on MIPS
#  mips-uclibc = uclibc (busybox), big endian Linux on MIPS
#  mips-vxworks = vxworks, big endian on MIPS

ifdef B_REFSW_ARCH
ARCH ?= $(B_REFSW_ARCH)
endif

# Handle vxworks from either the SYSTEM or ARCH variable.
ifeq ($(ARCH),mips-vxworks)
SYSTEM=vxworks
endif

ifeq ($(SYSTEM),vxworks)

ARCH = mips-vxworks
CROSS_COMPILE = mips
CC      = cc${CROSS_COMPILE}
CXX     = c++${CROSS_COMPILE}
LD      = ld${CROSS_COMPILE}
AR      = ar${CROSS_COMPILE}
NM      = nm${CROSS_COMPILE}
STRIP   = strip${CROSS_COMPILE}
OBJCOPY = objcopy${CROSS_COMPILE}
OBJDUMP = objdump${CROSS_COMPILE}
RANLIB  = ranlib${CROSS_COMPILE}

# MKDIR must make recursive dirs, and not fail if already existing
ifeq ($(OSTYPE),linux)
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
else
ifeq ($(vxWorksVersion),6)
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv
else
# These are really DOS options:
MKDIR   = -mkdir
PWD     = chdir
MV      = move
endif
endif

else

#
# Default toolchain
#
ARCH ?= mipsel-uclibc
SYSTEM ?= linux

#
# Set variables based on the toolchain
#
ifeq ($(filter ${ARCH}, i386-linux i386-mingw), ${ARCH})
CROSS_COMPILE ?=
TOOLCHAIN_DIR=/usr/bin
else
ifeq ($(filter ${ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${ARCH})
TOOLCHAIN_DIR:=$(shell dirname $(shell dirname $(shell which mips-linux-uclibc-gcc 2>/dev/null) 2>/dev/null))
CROSS_COMPILE ?= mips-linux-uclibc-
else
# Default Little Endian
#
TOOLCHAIN_DIR:=$(shell dirname $(shell dirname $(shell which mipsel-linux-uclibc-gcc 2>/dev/null) 2>/dev/null))
CROSS_COMPILE ?= mipsel-linux-uclibc-
endif
endif

# Define make variables
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
CXX     = $(CROSS_COMPILE)c++
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB  = $(CROSS_COMPILE)ranlib
MKDIR   = mkdir -p
PWD     = pwd
MV      = mv

endif

# These are operations common to all environments.
CPP     = $(CC) -E
CP      = cp -f
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
AWK 	= awk

# Define options for quiet builds
export VERBOSE
ifneq ($(VERBOSE),)
Q_:=
else
# This was Q_?=@, but that caused vxWorks problems.  VERBOSE=y must be used now to get verbose msgs.
Q_:=@
MAKEFLAGS += --no-print-directory
endif

