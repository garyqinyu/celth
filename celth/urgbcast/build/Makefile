# Makefile
.EXPORT_ALL_VARIABLES:


APPROOT ?= $(shell cd ../../..; pwd)
CELTHAPIDIR = $(APPROOT)/celth/celthapi
NFSDIR =/opt/nfsroot
CELTHLIBDIR=$(CELTHAPIDIR)/lib
EMERGEBCASTDIR =$(APPROOT)/celth/urgbcast
EMERGEBCASTLIBDIR = $(EMERGEBCASTDIR)/lib
DEBUG ?= y

.PHONY: check_environment static shared copy_libfile clean
all: check_environment static shared copy_libfile copy_libfiletonfs

include $(EMERGEBCASTDIR)/build/bcm_defs.inc
include $(EMERGEBCASTDIR)/build/tools.mak
include $(EMERGEBCASTDIR)/build/system.mak

LIB = urgbcastapi

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
vpath %.c $(EMERGEBCASTDIR)/src

SRCS = \
	ts_egbt.c \
	urgbcast.c \
	urgbcastcommand.c \
	urgbcastcontrol.c \
	urgbcastdata.c \
	urgbcastpmtmonitor.c \
	urgbcastpsisearch.c \
	urgbcastprocess.c 
	 

include $(EMERGEBCASTDIR)/build/rules.mak
copy_libfile:
	@echo [copy...........urgbcast lib file to the right directory]	
	$(CP) -d ./obj/lib$(LIB).* $(EMERGEBCASTLIBDIR)

copy_libfiletonfs:
	@echo [copy..........urgbcast lib file to nfs directory]
	$(CP) -d ./obj/lib$(LIB).* $(NFSDIR)

clean:
	@echo [clean.........urgbcast lib file ]
	$(RM) -rf ./obj/*.*
	$(RM) -rf $(EMERGEBCASTLIBDIR)/lib$(LIB).*

