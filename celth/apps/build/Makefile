############################################################
#     Copyright (c) 2003-2009, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: China_RefSW_Devel/126 $
# $brcm_Date: 3/31/09 10:51a $
#
# Module Description:
# 
############################################################

##############################
# define base directories    #
##############################

CELTHROOT ?= $(shell cd ../../..; pwd)
CELTHAPIDIR = $(CELTHROOT)/celth/celthapi
PEGAPIDIR   = $(CELTHROOT)/celth/peg182bcm
PEGMENUAPIDIR   = $(CELTHROOT)/celth/pegmenu
URGBCASTAPIDIR  = $(CELTHROOT)/celth/urgbcast
GBEMGAPIDIR     = $(CELTHROOT)/celth/gbemg
NEXUS_TOP = $(CELTHROOT)/nexus
MAGNUM = $(CELTHROOT)/magnum
BSEAV = $(CELTHROOT)/BSEAV
PLATFORM=$(NEXUS_PLATFORM)

##############################
# include files              #
##############################
include Makefile.cfg
include $(CELTHAPIDIR)/build/bcm_defs.inc
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc
# for magnum VDB lib support
#include $(MAGNUM)/commonutils/fmt/bfmt.inc
#include $(MAGNUM)/commonutils/pxl/bpxl.inc
include tools.mak
include system.mak

##############################
# linux related              #
##############################
LINUX ?= /opt/brcm/linux
LINUX_INC ?= ${LINUX}/include
OS=linuxuser

##############################
# define directories         #
##############################
ifeq ($(DEBUG),y)
OBJ_DIR := $(NEXUS_PLATFORM)-$(OS)-uclibc-debug
else
OBJ_DIR := $(NEXUS_PLATFORM)-$(OS)-uclibc-release
endif

LIB_DIR := $(OBJ_DIR)/lib

APP = $(OBJ_DIR)/celthapp
/*BINDIR ?= /public/$(USER)/bin*/
BINDIR =/opt/nfsroot
CELTHAPP_DIRS = $(CELTHROOT)/celth/apps/celthtestapp
INSTALL_DIR = ${BINDIR}/setup


##############################
# identify debug flag        #
##############################
ifeq ($(DEBUG),y)
CFLAGS += -DCELTHAPI_DEBUG
CFLAGS += -DBDBG_DEBUG_BUILD=1
LDFLAGS += -g
else
CFLAGS += -DBDBG_DEBUG_BUILD=0
LDFLAGS += -s
endif


##############################
# identify static flag       #
##############################

ifeq ($(SHAREABLE),n)
STATIC_CELTH := y
else
STATIC_CELTH := n
endif

ifndef SHAREABLE
STATIC_CELTH := n
endif

##############################
# kernel mode flag       #
##############################
ifeq ($(MODE),proxy)
KERNEL_MODE_CELTH := y
else
KERNEL_MODE_CELTH := n
endif


##############################
# variables                  #
##############################
BUILDDATE = `date +%Y%m%d`
CELTH_TAR_BALL = celthapp-$(BUILDDATE).$(PLATFORM)$(BCHP_VER)-$(SYSTEM)
ifeq ($(ARCH),arm-linux)
CELTH_TAR_BALL := $(CELTH_TAR_BALL)-armlinux
else
ifeq ($(ARCH),mipsel-linux)
CELTH_TAR_BALL := $(CELTH_TAR_BALL)-glibc
endif
endif

ifeq ($(SMP),y)
CELTH_TAR_BALL := $(CELTH_TAR_BALL)-smp
endif

CELTH_TAR_BALL := $(CELTH_TAR_BALL).bin.tgz

export CELTH := y


#The following are for CFLAGS
CELTHAPP_INCLUDE_DIRS = $(addprefix -I,$(CELTHAPP_DIRS))

OTHER_INCLUDES += -I$(BSEAV)/linux/driver/common


CELTHAPI_CFLAGS = \
	-I$(CELTHROOT)/celth/celthapi/include

#The following are for PEG

PEGAPI_CFLAGS = \
	-I$(PEGAPIDIR)/include 
	
PEGMENUAPI_CFLAGS = \
	-I$(PEGMENUAPIDIR)/include

URGBCASTAPI_CFLAGS = \
		-I$(URGBCASTAPIDIR)/inc

GBEMGAPI_CFLAGS = \
		-I$(GBEMGAPIDIR)/inc

#The following are for LDFLAGS

CELTHAPI_LDFLAGS = -L$(CELTHROOT)/celth/celthapi/lib \
	 -lcelthapi 
	 
NEXUS_LDFLAGS= -L$(CELTHROOT)/obj.$(PLATFORM)/nexus/bin \
	-lnexus

PEGAPI_LDFLAGS = -L$(PEGAPIDIR)/lib \
	-lpegapi
PEGMENUAPI_LDFLAGS = -L$(PEGMENUAPIDIR)/lib \
	-lpegmenuapi

URGBCASTAPI_LDFLAGS = -L$(URGBCASTAPIDIR)/lib \
	-lurgbcastapi

GBEMGAPI_LDFLAGS = -L$(GBEMGAPIDIR)/lib \
	-lgbemgapi
	

#APP_SRC =\
#	testmain.c

#APP_OBJECTS = $(foreach file, $(patsubst %.c,%.o,$(APP_SRC)),$(OBJ_DIR)/$(notdir $(file)))
	
APP_OBJECTS = \
	testmain.o

# for magnum VDB lib support
LIB_OBJECTS = $(foreach module, ${notdir $(BVDB_SOURCES)}, $(module:%.c=$(LIB_DIR)/%.o))

# for power standby passive
LIB_OBJECTS += $(foreach module, ${notdir $(BPOWER_STANDBY_SOURCES)}, $(module:%.c=$(LIB_DIR)/%.o))


##############################
# configure CFLAGS           #
##############################
CFLAGS += -DLINUX -MD -O2 -fno-exceptions
CFLAGS += -DBCM_BOARD=$(PLATFORM)
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
CFLAGS += $(CELTHAPP_INCLUDE_DIRS)
CFLAGS += $(B_REFSW_MAGNUM_INCLUDE_DIRS)
CFLAGS += $(SYSTEM_FLAGS)
CFLAGS += $(OTHER_INCLUDES)
CFLAGS += $(CELTHAPI_CFLAGS)
CFLAGS += $(PEGAPI_CFLAGS)
CFLAGS += $(PEGMENUAPI_CFLAGS)
CFLAGS += $(URGBCASTAPI_CFLAGS)
CFLAGS += $(GBEMGAPI_CFLAGS)

# for magnum VDB lib support
CFLAGS += $(addprefix -I,$(BVDB_INCLUDES)) $(addprefix -I,$(BFMT_INCLUDES)) $(addprefix -I, $(BPXL_INCLUDES))

# for power standby passive
CFLAGS += $(addprefix -I,$(BPOWER_STANDBY_INCLUDES))

# put this in the end of the CFLAGS configuration to make sure CXXFLAGS be exactly as same as CFLAGS
CXXFLAGS = $(CFLAGS)

##############################
# configure LDFLAGS          #
##############################
ifeq ($(STATIC_CELTH),y)
SHAREDLIB = no
CFLAGS += -DSTATIC_CELTH=1
LDFLAGS += -static
else
CFLAGS += -DSTATIC_CELTH=0
LDFLAGS += -rdynamic
endif

LDFLAGS += $(CELTHAPI_LDFLAGS) $(NEXUS_LDFLAGS) $(PEGAPI_LDFLAGS) $(PEGMENUAPI_LDFLAGS) $(URGBCASTAPI_LDFLAGS)

LDFLAGS += $(GBEMGAPI_LDFLAGS)

LDFLAGS += -lpthread


##############################
# vpath                      #
##############################
VPATH += $(CELTHAPP_DIRS)
vpath %.h $(VPATH)
vpath %.cpp $(VPATH)
vpath %.c $(VPATH)
vpath %.o $(OBJ_DIR)


##############################
# verbose flag               #
##############################
ifneq (${CELTH_VERBOSE},yes)
Q_:= @
else
Q_:=
endif


.PHONY: settop do_install all lib lib-nexuspi lib-celthapi lib-pegapi lib-pegmenuapi lib-urgbcastapi lib-gbemgapi lib-celapp bin_link bin_copy \
	install celthapp celthapi nexuspi clean-celthapi-lib clean-pegapi-lib clean-pegmenuapi-lib clean-urgbcastapi-lib clean-gbemgapi-lib

# 'all' is the default rule.
# 'all' builds everything but doesn't perform install.
# 'all' copies only binaries into BINDIR. It allows you to
#  run from that directory without having your configuration files overwritten.
# install builds an installation tarball in BINDIR.

all: check_environment ${BINDIR} settop lib bin_link bin_copy

install: check_environment ${BINDIR} settop lib bin_link do_install

clean: check_environment clean-lib

celapp: lib-celapp bin_link bin_copy

celapi: lib-celthapi bin_link bin_copy

nexuspi: lib-nexuspi bin_link bin_copy

lib: lib-nexuspi lib-celthapi lib-celapp lib-pegapi lib-pegmenuapi lib-urgbcastapi lib-gbemgapi

clean-lib: clean-celthapi clean-pegapi clean-pegmenuapi clean-urgbcastapi clean-gbemgapi clean-celapp clean-nexuspi 

lib-celapp: $(OBJ_DIR) $(LIB_DIR)  $(addprefix $(OBJ_DIR)/,$(APP_OBJECTS)) $(LIB_OBJECTS)

lib-celthapi:
	${Q_}echo [Build... celthapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(CELTHROOT)/celth/celthapi/build DEBUG=$(DEBUG)
lib-pegapi:
	${Q_}echo [Build... pegapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(PEGAPIDIR)/build/brcm DEBUG=$(DEBUG)
lib-pegmenuapi:
	${Q_}echo [Build... pegmenuapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(PEGMENUAPIDIR)/build/brcm DEBUG=$(DEBUG)

lib-urgbcastapi:
	${Q_}echo [Build... urgbcastapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(URGBCASTAPIDIR)/build DEBUG=$(DEBUG)

lib-gbemgapi:
	${Q_}echo [Build... gbemgapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(GBEMGAPIDIR)/build DEBUG=$(DEBUG)
	
	
lib-nexuspi:
	${Q_}echo [Build... nexus and magnum]
	${Q_}$(MAKE) -s --no-print-directory -C $(NEXUS_TOP)/build DEBUG=$(DEBUG)

clean-celapp: 
	$(RM) -rf $(OBJ_DIR)
	$(RM) -rf ../*.opt
	$(RM) -rf ../*.ncb
	$(RM) -rf ../Debug
	$(RM) -rf ../Release
	$(RM) -rf ../apps/Debug
	$(RM) -rf ../apps/Release

#clean-celthapi-lib:
#	${Q_}$(MAKE) -s --no-print-directory -C $(NEXUS_TOP)/lib/playback_ip clean
#	${Q_}$(MAKE) -s --no-print-directory -C $(BSEAV)/lib/livemedia/live clean &> /dev/null
#	${Q_}$(MAKE) -s --no-print-directory -C $(NEXUS_TOP)/lib/os clean

clean-celthapi: clean-celthapi-lib
	${Q_}echo [Clean... celthapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(CELTHROOT)/celth/celthapi/build DEBUG=$(DEBUG) clean

clean-pegapi: clean-pegapi-lib
	${Q_}echo [Clean... pegapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(PEGAPIDIR)/build/brcm DEBUG=$(DEBUG) clean

clean-pegmenuapi: clean-pegmenuapi-lib
	${Q_}echo [Clean... pegmenuapi]
	${Q_}$(MAKE) -s --no-print-directory -C $(PEGMENUAPIDIR)/build/brcm DEBUG=$(DEBUG) clean	

clean-urgbcastapi: clean-urgbcastapi-lib
	${Q_}echo [Clean... urgbcastlib]
	${Q_}$(MAKE) -s --no-print-directory -C $(URGBCASTAPIDIR)/build DEBUG=$(DEBUG) clean	

clean-gbemgapi: clean-gbemgapi-lib
	${Q_}echo [Clean... gbemglib]
	${Q_}$(MAKE) -s --no-print-directory -C $(GBEMGAPIDIR)/build DEBUG=$(DEBUG) clean	


	
clean-nexuspi:
	${Q_}echo [Clean... nexus and magnum]	
	${Q_}$(RM) -rf $(NEXUS_TOP)/bin/libnexus.*
	${Q_}$(MAKE) -s --no-print-directory -C $(NEXUS_TOP)/build DEBUG=$(DEBUG) clean

settop:
	${Q_}$(RM) -rf ${BINDIR}/$@
	${Q_}sed \
	-e "s/%%LIBS%%/$(notdir ${APP})/g" \
	-e "s/%%KYLIN%%/$(notdir ${APP})/g" \
	-e "s/%%KYLIN_TAR_BALL%%/${KYLIN_TAR_BALL}/g" \
	-e "s/%%PLATFORM%%/${PLATFORM}/g" \
	$@ >${BINDIR}/$@
	${Q_}chmod +x ${BINDIR}/$@
	${Q_}$(CP) settop.install $(BINDIR)
	${Q_}if [ -e settop.${PLATFORM} ]; then $(CP) settop.${PLATFORM} ${BINDIR}; fi
#${Q_}$(CP) ./create_dev_nodes $(BINDIR)

# Copy stuff to the INSTALL_DIR and tar it up
do_install:
	${Q_}echo 
	${Q_}echo "Creating installation tarball"
	${Q_}$(RM) -rf $(INSTALL_DIR)
	${Q_}mkdir -p $(INSTALL_DIR)
#
# For some environments, we need to grab libraries from the
# toolchain because they are not present in the rootfs by default.
#
ifeq ($(ARCH),mipsel-uclibc)
	${Q_}echo "Copying libraries from the toolchain"
	${Q_}$(CP) -a $(TOOLCHAIN_DIR)/mipsel-linux-uclibc/lib/libstdc++.so* $(INSTALL_DIR)
	${Q_}$(CP) -a $(TOOLCHAIN_DIR)/mipsel-linux-uclibc/lib/libgcc_s.so* $(INSTALL_DIR)
	${Q_}$(STRIP) --strip-all $(INSTALL_DIR)/libstdc++.so*
	${Q_}$(STRIP) --strip-all $(INSTALL_DIR)/libgcc_s.so*
endif
	${Q_}$(CP) $(APP) $(INSTALL_DIR)
	${Q_}$(CP) $(BINDIR)/settop $(INSTALL_DIR)
	${Q_}if [ -e $(BINDIR)/settop.${PLATFORM} ]; then ${CP} $(BINDIR)/settop.${PLATFORM} ${INSTALL_DIR}; fi
	${Q_}$(CP) settop.install $(INSTALL_DIR)

ifeq ($(STATIC_CELTH),n)
	${Q_}$(CP) $(CELTHROOT)/obj.$(PLATFORM)/nexus/bin/libnexus.so $(INSTALL_DIR)
#${Q_}$(CP) $(NEXUS_TOP)/bin/libb_os.so $(INSTALL_DIR)
#${Q_}$(CP) $(NEXUS_TOP)/bin/libb_playback_ip.so $(INSTALL_DIR)
endif
#	${Q_}$(CP) ./create_dev_nodes $(INSTALL_DIR)
ifeq ($(KERNEL_MODE_CELTH), y)
	${Q_}$(CP) $(CELTHROOT)/obj.$(PLATFORM)/nexus/bin/nexus.ko ${INSTALL_DIR}
	${Q_}$(RM) -rf $(BINDIR)/bcmdriver.ko
else
	${Q_}$(CP) $(NEXUS_TOP)/bin/bcmdriver.ko ${INSTALL_DIR}
	${Q_}$(RM) -rf $(BINDIR)/nexus.ko	
endif	

ifeq ($(SMP),y)
	${Q_}$(CP) $(BSEAV)/lib/netaccel/bin/$(BCHP_CHIP)/2618/smp/bcmnetaccel.ko ${INSTALL_DIR}
else
	${Q_}$(CP) $(BSEAV)/lib/netaccel/bin/$(BCHP_CHIP)/2618/bcmnetaccel.ko ${INSTALL_DIR}
endif
	${Q_}chmod 777 -R $(INSTALL_DIR)
	${Q_}(cd ${INSTALL_DIR};tar -pzck . -f -) >${BINDIR}/${DIGA_TAR_BALL}
	${Q_}(cd ${BINDIR};tar zxfp ${BINDIR}/${DIGA_TAR_BALL}) 
	${Q_}$(RM) -rf $(INSTALL_DIR)
	${Q_}echo ""
	${Q_}echo "**************************************************************************"
	${Q_}echo "* Installation tarball created in $(BINDIR)"
	${Q_}echo "**************************************************************************"
	${Q_}echo ""

bin_link:
	${Q_}echo "$(LDFLAGS)"
	${Q_}$(CXX) -o $(APP) $(addprefix $(OBJ_DIR)/,$(APP_OBJECTS)) $(LIB_OBJECTS) $(LDFLAGS)

bin_copy:
	${Q_}echo ""
	${Q_}echo "**************************************************************************"
	${Q_}echo "* Copying all binaries to ${BINDIR}"
	${Q_}echo "**************************************************************************"
	${Q_}echo ""
	${Q_}$(CP) $(APP) ${BINDIR}
ifeq ($(STATIC_CELTH),n)
	${Q_}$(CP) $(CELTHROOT)/obj.$(PLATFORM)/nexus/bin/libnexus.so ${BINDIR}
endif
ifeq ($(KERNEL_MODE_CELTH), y)
	${Q_}$(CP) $(CELTHROOT)/obj.$(PLATFORM)/nexus/bin/nexus.ko ${BINDIR}
endif

${BINDIR}:
	mkdir -p ${BINDIR}
	${Q_}chmod 777 -R ${BINDIR}

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Implicit C Compile Rule
$(OBJ_DIR)/%.o: %.cpp
	${Q_}echo "[Compile...  $(notdir $<) (celthapp)]"
	${Q_}$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	${Q_}echo "[Compile...  $(notdir $<) (celthapp)]"
	${Q_}$(CC) ${CDEP_FLAG} $(CFLAGS) -c $< -o $@

$(LIB_DIR)/%.o: %.c
	${Q_}echo "[Compile...  $(notdir $<) (celthapp/lib)]"
	${Q_}$(CC) ${CDEP_FLAG} $(CFLAGS) -c $< -o $@


#require check_environment
check_environment:
PLATFORMS = \
	97250
ifndef PLATFORM
	$(error PLATFORM is undefined. Valid platforms are $(PLATFORMS).)
endif
ifeq ($(findstring $(PLATFORM),$(PLATFORMS)),)
	$(error PLATFORM is defined to an unknown value. Valid platforms are $(PLATFORMS).)
endif
ifndef BSEAV
	$(error BSEAV is undefined. You must define this.)
endif
ifeq ($(findstring $(SYSTEM),linux linuxkernel),)
	$(error SYSTEM is defined to an unknown value $(SYSTEM).)
endif

echo:
	echo $(CELTH)

-include $(OBJ_DIR)/*.d


ifneq ($(SYSTEM),vxworks)
# Create output directories
$(OBJ_DIR):
ifdef OBJ_DIR
	$(MKDIR) $(OBJ_DIR)
	$(MKDIR) $(LIB_DIR)
endif
endif



