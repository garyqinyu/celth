############################################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: rules.mak $
# $brcm_Revision: China_RefSW_Devel/4 $
# $brcm_Date: 8/1/08 6:56p $
#
# Module Description:
#
# Revision History:
#
# 
###########################################################################


ifneq (${CELTH_VERBOSE},yes)
Q_ :=  
else
Q_ := @ 
endif

CDEP_FLAG = -MD

ODIR ?= ./obj
#CELTHLIBDIR= ../lib

# Implicit C Compile Rule
$(ODIR)/%.o : %.c
		@echo "[Compile... $(notdir $<) (celthapi)]"
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_STATIC} $(CFLAGS) -c $< -o $@


$(ODIR)/%.so : %.c
		@echo "[Compile... $(notdir $<) (celthapi)]"
ifeq ($(SYSTEM),vxworks)
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_SHARED} $(CFLAGS) -c $< -o $@
		-${Q_}move $(patsubst %.c,%.d,$(notdir $< )) ${ODIR}
else
		${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_SHARED} $(CFLAGS) -c $< -o $@
endif


# Implicit C++ Compile Rule
$(ODIR)/%.o : %.cpp
	@echo "[Compile... $(notdir $<) (celthapi)]"
	${Q_}$(CC) ${CDEP_FLAG} ${CPPFLAGS_STATIC} $(CPPFLAGS) -c $< -o $@

# Implicit C++ Compile Rule
$(ODIR)/%.so : %.cpp
	@echo "[Compile... $(notdir $<) (celthapi)]"
	${Q_}$(CC) ${CDEP_FLAG} ${CPPFLAGS_SHARED} $(CPPFLAGS) -c $< -o $@

ifdef APP

APP_IMAGE = ${ODIR}/${APP}

application:  ${ODIR} ${APP_IMAGE}

$(APP_IMAGE): ${LIBS} $(OBJS)
	@echo Linking $@
	${Q_}$(CXX) $(OBJS) $(LDFLAGS) -o $@

target-clean:
	${Q_}${RM} -v ${APP_IMAGE}

endif

ifdef LIB
ifeq (${DEBUG},y)

SH_LIB = ${ODIR}/lib$(LIB).so
SH_MAP = ${ODIR}/lib$(LIB).map
else

ST_LIB = ${ODIR}/lib$(LIB).a

endif

ifeq (${DEBUG},y)

shared: ${ODIR} ${SH_LIB}

else 

static: ${ODIR} ${ST_LIB} 

endif

ifeq (${DEBUG},y)
SH_OBJS = $(SRCS:%.c=${ODIR}/%.so)
else
ST_OBJS = $(SRCS:%.c=${ODIR}/%.o)
endif
#OBJS = $(SRCS:%.c =${ODIR}/%.o)

ifeq (${DEBUG},y)

${SH_LIB}: ${SH_OBJS} ${LIBS}
	@echo Linking shared ... $@ ${LDFLAGS}
#${Q_}$(CC) ${LDFLAGS}  -shared -fPIC -Wl,-map,${SH_MAP} -Wl,--cref  -o  $@ -Wl,-soname,lib${LIB}.so ${SH_OBJS} ${LIBS}
	${Q_}$(CC) ${LDFLAGS}  -shared   -Wl,--cref  -o  $@ -Wl,-soname,lib${LIB}.so ${SH_OBJS} ${LIBS}

else

${ST_LIB}: ${ST_OBJS} 
	@echo Linking static.... $@
	${Q_}$(RM) $@
	${Q_}$(AR) cq $@ $(ST_OBJS)
	${Q_}$(RANLIB) $@
endif

#copy_libfile:
#	@echo[copy....lib file]
#	$(Q_)$(CP) -d $(ODIR)/lib$(LIB).so $(CELTHLIBDIR)


target-clean:
ifeq (${DEBUG},y)
	${Q_}${RM} ${SH_LIB}
else
        ${Q_}${RM} ${ST_LIB}
endif  

endif
# Dependency file checking (created with gcc -M command)
-include $(ODIR)/*.d

# Clean Rules
.PHONY: clean veryclean 
clean: target-clean

ifdef OBJS
	${Q_}$(RM) $(OBJS:%.o=%.d)
	${Q_}$(RM) $(OBJS)
endif
ifdef SH_OBJS
	${Q_}$(RM) $(SH_OBJS:%.so=%.d)
	${Q_}$(RM) $(SH_OBJS)
endif
ifdef ST_OBJS
	${Q_}$(RM) $(ST_OBJS:%.o=%.d)
	${Q_}$(RM) $(ST_OBJS)
endif

veryclean:
ifdef ODIR
	${Q_}$(RM) $(ODIR)/*
	${Q_}$(RM) -r $(ODIR)
endif

ifneq ($(SYSTEM),vxworks)
# Create output directories
$(ODIR):
ifdef ODIR
	$(MKDIR) $(ODIR)
endif
endif



