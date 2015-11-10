# Makefile
.EXPORT_ALL_VARIABLES:


APPROOT ?= $(shell cd ../../..; pwd)
CELTHAPIDIR = $(APPROOT)/celth/celthapi
NFSDIR =/opt/nfsroot
CELTHLIBDIR=$(CELTHAPIDIR)/lib
DEBUG ?= y

.PHONY: check_environment static shared copy_libfile clean
all: check_environment static shared copy_libfile copy_libfiletonfs

include $(CELTHAPIDIR)/build/bcm_defs.inc
include $(CELTHAPIDIR)/build/tools.mak
include $(CELTHAPIDIR)/build/system.mak

LIB = celthapi

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
CFLAGS += -I../include -I.



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
vpath %.c $(CELTHAPIDIR)/src

SRCS = \
	ts_pat.c \
	ts_pmt.c \
	ts_priv.c \
	ts_psi.c \
	tspsisearch.c \
	celth_debug.c \
	celth_tuner.c \
	celth_avplay.c \
	celth_rtos.c \
	celth_event.c \
	celth_platform.c \
	celth_graphic.c \
	celth_display.c \
	celth_mutex.c \
	celth_message.c \
	celth_ringbuffer.c \
	celth_demux.c \
	celth_symbol.c

#celth_timer.c
	 

include $(CELTHAPIDIR)/build/rules.mak
copy_libfile:
	@echo [copy...........lib file to the right directory]	
	$(CP) -d ./obj/lib$(LIB).* $(CELTHLIBDIR)

copy_libfiletonfs:
	@echo [copy..........lib file to nfs directory]
	$(CP) -d ./obj/lib$(LIB).* $(NFSDIR)

clean:
	@echo [clean.........celth lib file ]
	$(RM) -rf ./obj/*.*
	$(RM) -rf $(CELTHLIBDIR)/lib$(LIB).*

