# Makefile
.EXPORT_ALL_VARIABLES:


APPROOT ?= $(shell cd ../../../..; pwd)
PEGMENUAPIDIR = $(APPROOT)/celth/pegmenu
PEGMENUAPILIBDIR=$(PEGMENUAPIDIR)/lib
NFSDIR  =/opt/nfsroot
CELTHAPIDIR = $(APPROOT)/celth/celthapi
PEGAPIDIR  = $(APPROOT)/celth/peg182bcm
DEBUG ?= y

.PHONY: check_environment static shared copy_libfile copy_libfile_to_nfsdirectory clean
all: check_environment static shared copy_libfile copy_libfile_to_nfsdirectory

include $(PEGAPIDIR)/build/brcm/bcm_defs.inc
include $(PEGAPIDIR)/build/brcm/tools.mak
include $(PEGAPIDIR)/build/brcm/system.mak

LIB = pegmenuapi

CFLAGS_SHARED += -fpic -DPIC
CPPFLAGS_SHARED += -fpic -DPIC

# JJ 
# CFLAGS += -O2 -ansi -W -Wall -DBDBG_DEBUG_BUILD=1
ifeq (${DEBUG},y)
CFLAGS += -DBDBG_DEBUG_BUILD=1
CFLAGS += $(CPPFLAGS_SHARED)
endif



# Move this sentence to line 59 to avoid a lot warnings when compiling IOI module.
#CFLAGS += -O2 -W -Wall -Wno-parentheses -Wno-unused -Wno-uninitialized -Wno-sign-compare -DLINUX
CFLAGS += -I../include -I.



include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
CFLAGS += $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

CFLAGS += -I$(PEGAPIDIR)/include
CFLAGS += -I$(CELTHAPIDIR)/include
CFLAGS += -I$(PEGMENUAPIDIR)/include

#add for thumbnail lib support
include $(BSEAV)/lib/thumbnail/bthumbnail.inc
include $(BSEAV)/lib/bfile/bfile.inc
CFLAGS += $(addprefix -I,$(BTHUMBNAIL_INCLUDES)) $(addprefix -I,$(BFILE_STDIO_INCLUDES)) 
CFLAGS += -I$(BSEAV)/lib/media
CFLAGS += -I$(BSEAV)/lib/utils

CFLAGS += -O2 -W -Wall -DLINUX
CFLAGS += -fstrict-aliasing -frename-registers


 
vpath %.cpp $(PEGMENUAPIDIR)/src
vpath %.c   $(PEGMENUAPIDIR)/src
vpath %.hpp $(PEGMENUAPIDIR)/include

CPPFLAGS += $(CFLAGS)


#VPATH = $(PEGAPIDIR)/source:$(PEGAPIDIR)/include:$(CELTHAPIDIR)/include

SRCS =	\
	arial12.cpp \
	arial14.cpp \
	factorymenu.cpp \
	helpwnd.cpp \
	image.cpp \
	picturemenu.cpp \
	soundmenu.cpp \
	superfunction.cpp \
	supsetmenu.cpp \
	tooltip.cpp \
	TVAnimateWindow.cpp \
	tvbitmapbutton.cpp \
	tvprobar.cpp \
	tvprogwi.cpp \
	tvprompt.cpp \
	tvselectwi.cpp \
	tvtextbutton.cpp \
	videomenu.cpp \
	wbstring.cpp \
	mainmenu.cpp

include $(PEGMENUAPIDIR)/build/brcm/rules.mak
copy_libfile:
	@echo [copy...........menu lib file to the right directory]	
	$(CP) -d ./obj/lib$(LIB).* $(PEGMENUAPILIBDIR)

copy_libfile_to_nfsdirectory:
	@echo [copy............menu lib file to nfs directory]
	$(CP) -d ./obj/lib$(LIB).* $(NFSDIR)

clean:
	@echo [clean.........pegmenu lib file ]
	$(RM) -rf ./obj/*.*
	$(RM) -rf $(PEGMENUAPILIBDIR)/lib$(LIB).*


