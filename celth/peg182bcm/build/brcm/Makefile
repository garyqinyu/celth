# Makefile
.EXPORT_ALL_VARIABLES:


APPROOT ?= $(shell cd ../../../..; pwd)
PEGAPIDIR = $(APPROOT)/celth/peg182bcm
PEGLIBDIR=$(PEGAPIDIR)/lib
NFSDIR  =/opt/nfsroot
CELTHAPIDIR = $(APPROOT)/celth/celthapi
DEBUG ?= y

.PHONY: check_environment static shared copy_libfile copy_libfile_to_nfsdirectory clean
all: check_environment static shared copy_libfile copy_libfile_to_nfsdirectory

include $(PEGAPIDIR)/build/brcm/bcm_defs.inc
include $(PEGAPIDIR)/build/brcm/tools.mak
include $(PEGAPIDIR)/build/brcm/system.mak

LIB = pegapi

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

#add for thumbnail lib support
include $(BSEAV)/lib/thumbnail/bthumbnail.inc
include $(BSEAV)/lib/bfile/bfile.inc
CFLAGS += $(addprefix -I,$(BTHUMBNAIL_INCLUDES)) $(addprefix -I,$(BFILE_STDIO_INCLUDES)) 
CFLAGS += -I$(BSEAV)/lib/media
CFLAGS += -I$(BSEAV)/lib/utils

CFLAGS += -O2 -W -Wall -DLINUX
CFLAGS += -fstrict-aliasing -frename-registers


 
vpath %.cpp $(PEGAPIDIR)/source
vpath %.c   $(PEGAPIDIR)/source
vpath %.hpp $(PEGAPIDIR)/include

CPPFLAGS += $(CFLAGS)


#VPATH = $(PEGAPIDIR)/source:$(PEGAPIDIR)/include:$(CELTHAPIDIR)/include

SRCS =	\
	ltoa.cpp \
	panimwin.cpp  \
	pbitmaps.cpp  \
	pblight.cpp  \
	pbmpconv.cpp  \
	pbutton.cpp  \
	pchart.cpp  \
	pclight.cpp  \
	pcombo.cpp  \
	pdecbtn.cpp \
	pdecwin.cpp  \
	pdial.cpp  \
	pdialog.cpp  \
	peditbox.cpp  \
	pfbdial.cpp  \
	pfdial.cpp  \
	pfonts.cpp  \
	pgifconv.cpp  \
	pgroup.cpp  \
	picon.cpp  \
	pimgconv.cpp  \
	pjpgconv.cpp  \
	plbscale.cpp  \
	plight.cpp  \
	plist.cpp  \
	pliteral.cpp  \
	plnchart.cpp  \
	plscale.cpp  \
	pmenfont.cpp  \
	pmenu.cpp  \
	pmesgwin.cpp  \
	pmessage.cpp  \
	pmlchart.cpp  \
	pmltbtn.cpp  \
	pnotebk.cpp  \
	ppngconv.cpp  \
	ppresent.cpp  \
	pprogbar.cpp  \
	pprogwin.cpp  \
	pprompt.cpp  \
	pquant.cpp  \
	prect.cpp  \
	pscale.cpp  \
	pscreen.cpp  \
	pscroll.cpp  \
	psincos.cpp  \
	pslider.cpp  \
	pspin.cpp  \
	pspread.cpp  \
	pstatbar.cpp  \
	pstchart.cpp  \
	pstring.cpp  \
	psysfont.cpp  \
	ptable.cpp  \
	ptermwin.cpp  \
	ptextbox.cpp  \
	pthing.cpp  \
	ptitle.cpp  \
	ptoolbar.cpp  \
	ptree.cpp  \
	pvecfont.cpp  \
	pvprompt.cpp  \
	pwindow.cpp  \
	pzip.cpp \
	p2dpoly.cpp \
	pronto10.cpp \
	pronto12.cpp \
	pronto26.cpp \
	pronto16.cpp \
	l8brcmscrn.cpp \
	l8brcmpeg.cpp \
	chinesefont.cpp
	 

include $(PEGAPIDIR)/build/brcm/rules.mak
copy_libfile:
	@echo [copy...........lib file to the right directory]	
	$(CP) -d ./obj/lib$(LIB).* $(PEGLIBDIR)

copy_libfile_to_nfsdirectory:
	@echo [copy............lib file to nfs directory]
	$(CP) -d ./obj/lib$(LIB).* $(NFSDIR)
clean:
	@echo [clean.........pegapi lib file ]
	$(RM) -rf ./obj/*.*
	$(RM) -rf $(PEGLIBDIR)/lib$(LIB).*


