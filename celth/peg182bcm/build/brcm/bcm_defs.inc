############################################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm_defs.inc $
# $brcm_Revision: China_RefSW_Devel/6 $
# $brcm_Date: 6/25/08 1:15p $
#
# Module Description:
#
# Revision History:
#
# 
############################################################################

############################################
MAGNUM = $(CELTHAPIDIR)/../../magnum
ROCKFORD = $(CELTHAPIDIR)/../../rockford
NEXUS_TOP = $(CELTHAPIDIR)/../../nexus


# for rdb
PLATFORM_PATH = $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
BCM_BOARD_NO = $(PLATFORM)

CFLAGS += -DBCHP_CHIP=$(BCHP_CHIP)
CFLAGS += -DBCM_BOARD=$(PLATFORM)
CFLAGS += -DB_SYSTEM_$(SYSTEM)
CFLAGS += -DBCM_BOARD_STR=\"$(BCM_BOARD_NO)\"

