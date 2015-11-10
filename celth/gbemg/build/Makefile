# Makefile
.EXPORT_ALL_VARIABLES:


APPROOT ?= $(shell cd ../../..; pwd)
CELTHAPIDIR = $(APPROOT)/celth/celthapi
NFSDIR =/opt/nfsroot
CELTHLIBDIR=$(CELTHAPIDIR)/lib
GBEMGDIR =$(APPROOT)/celth/gbemg
GBEMGLIBDIR = $(GBEMGDIR)/lib
DEBUG ?= y

.PHONY: check_environment static shared copy_libfile clean
all: check_environment static shared copy_libfile copy_libfiletonfs

include $(GBEMGDIR)/build/bcm_defs.inc
include $(GBEMGDIR)/build/tools.mak
include $(GBEMGDIR)/build/system.mak

LIB = gbemgapi

#CFLAGS_SHARED += -fpic -DPIC
CFLAGS_SHARED  += -fPIC -DPIC
# JJ 
# CFLAGS += -O2 -ansi -W -Wall -DBDBG_DEBUG_BUILD=1
ifeq (${DEBUG},y)
CFLAGS += -DCELTHAPI_DEBUG
CFLAGS += -DBDBG_DEBUG_BUILD=1
CFLAGS += $(CFLAGS_SHARED)
endif



# Move this sentence to line 59 to avoid a lot warnings when compiling IOI module.
#CFLAGS += -O2 -W -Wall -Wno-parentheses -Wno-unused -Wno-uninitialized -Wno-sign-compare -DLINUX
CFLAGS += -I../inc -I.



include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
CFLAGS += $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

CFLAGS += -I$(CELTHAPIDIR)/include

#add for thumbnail lib support
include $(BSEAV)/lib/thumbnail/bthumbnail.inc
include $(BSEAV)/lib/bfile/bfile.inc
CFLAGS += $(addprefix -I,$(BTHUMBNAIL_INCLUDES)) $(addprefix -I,$(BFILE_STDIO_INCLUDES)) 
CFLAGS += -I$(BSEAV)/lib/media
CFLAGS += -I$(BSEAV)/lib/utils

CFLAGS += -O2 -W -Wall -DLINUX
CFLAGS += -fstrict-aliasing -frename-registers 
vpath %.c $(GBEMGDIR)/src

SRCS = \
	gbemg_index.c \
	gbemg_indexpriv.c \
	gbemg_contentpriv.c \
	gbemg_emglist.c \
	gbemg_getcont.c \
	gbemg_configurefilter.c \
	gbemg_monitoremgsect.c \
	gbemg_playcontrol.c \
	gbemg_control.c \
	gbemg_timer.c \
	gbemg_timeprocess.c
	 

include $(GBEMGDIR)/build/rules.mak
copy_libfile:
	@echo [copy...........gbemg lib file to the right directory]	
	$(CP) -d ./obj/lib$(LIB).* $(GBEMGLIBDIR)

copy_libfiletonfs:
	@echo [copy..........gbemg lib file to nfs directory]
	$(CP) -d ./obj/lib$(LIB).* $(NFSDIR)

clean:
	@echo [clean.........gbemg lib file ]
	$(RM) -rf ./obj/*.*
	$(RM) -rf $(GBEMGLIBDIR)/lib$(LIB).*

