#=================================================================================
#================== MAKEFILE FOR GENERATION OF SV_PEG.LIB ========================
#=================================================================================

#========= Root directories
ST_ROOT = c:\stm\ctv100\startkit
PEG_ROOT = c:\peg

#========= PEG paths
PEG_INC = $(PEG_ROOT)\include
PEG_SRC = $(PEG_ROOT)\source
PEG_OBJ = $(PEG_ROOT)\build\st20\obj

#========= Path on Tools directory
TVD_TOOLS  = $(ST_ROOT)\maketool

#========= Include of common file
include $(TVD_TOOLS)\toolset.mak

#========= These are needed by 'Makelib' below.
TVD_SYSTEM = $(ST_ROOT)\..
TVD_DRIVER = $(ST_ROOT)\..
TVD_SERVICE = $(ST_ROOT)\..
TVD_INCLUDES = $(ST_ROOT)\Include
TVD_PLATFORM = STV3500
VIEW_DRIVE = $(ST_ROOT)

#========= Target name
PEG_LIB = sv_peg.lib

all: $(PEG_OBJ)\$(PEG_LIB)

#========= Module Identifier (it is the name of the directory)
MODULE_ID   = sv_peg

#========= Module Type (DRIVER, SERVICE)
MODULE_TYPE = PEG

#========= Modules list needed for the compilation
INCLUDES :=   $(INCLUDE_PREFIX)./                    \
	        $(INCLUDE_PREFIX)$(PEG_INC)		\
		$(INCLUDE_PREFIX)$(TVD_INCLUDES)   	\
                $(INCLUDE_PREFIX)$(TVD_SYSTEM)\OSAL     \
                $(INCLUDE_PREFIX)$(TVD_SYSTEM)\DBG      \
                $(INCLUDE_PREFIX)$(TVD_SYSTEM)\INCLUDE  \
                $(INCLUDE_PREFIX)$(TVD_SYSTEM)\HANDLE  \
                $(INCLUDE_PREFIX)$(TVD_DRIVER)\STOSD    \
                $(INCLUDE_PREFIX)$(TVD_DRIVER)\dv_bme    \
                $(INCLUDE_PREFIX)$(TVD_DRIVER)\sti2c    \
                $(INCLUDE_PREFIX)$(TVD_DRIVER)\stpio    \
                $(INCLUDE_PREFIX)$(TVD_DRIVER)\dv_adc    \
                $(INCLUDE_PREFIX)$(TVD_SERVICE)\sv_pow    \
       		$(INCLUDE_PREFIX)$(TVD_SYSTEM)\lib_header 

#========= Local objects which are used to build the library
#========= Local files which comprise this component
SRCS =  $(addprefix $(PEG_SRC)/, $(SRCS_NAME))
OBJS =  $(addprefix $(PEG_OBJ)/, $(patsubst %cpp,%tco,$(SRCS_NAME)))
#INCS =  $(addprefix $(PEG_INC)/, $(patsubst %cpp,%h,  $(SRCS_NAME) ))

SRCS_NAME = 3500scrn.cpp\
	3500peg.cpp \
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
	p2dpoly.cpp 
	


include $(TVD_TOOLS)/MakeLib


# Command Line for compilation of "C++" files
$(PEG_OBJ)/%.tco:  $(PEG_SRC)/%.cpp $(PEG_OBJ)             \
                        $(TVD_TOOLS)/defrules.mak          \
                        $(TVD_TOOLS)/toolset.mak           \
                        makefile
	@echo .
	@echo .
	@echo ---------------------------------------------------------------
	@echo --- Compiling $< 
	@echo ---------------------------------------------------------------
	$(CC) $< $(CFLAGS) $(INCLUDES) $(OPTIONS) -o $@


# Command Line .lib generation for TARGETS
$(PEG_OBJ)\$(PEG_LIB):  $(OBJS) $(IMPORTS) $(TVD_TOOLS)/MakeLib
	@echo .
	@echo .
	@echo ---------------------------------------------------------------
	@echo --- Generate $@ 
	@echo ---------------------------------------------------------------
	$(AR) $(OBJS) $(IMPORTS) -o $@	 
	@echo --- $(PEG_LIB) built
	@echo ---------------------------------------------------------------


$(PEG_OBJ):
	mkdir $@
