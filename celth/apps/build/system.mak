SYSTEM ?= linux

ifeq (${SYSTEM},vxworks)
# TODO: These flags were causing a problem for i386-linux builds of mlibs. Are they
# really needed for Linux?
SYSTEM_FLAGS = -D_POSIX_C_SOURCE=199506 -D_ISOC9X_SOURCE

SYSTEM_FLAGS += -r -nostdlib
SYSTEM_FLAGS += -G 0 -mno-branch-likely -mips3 -EB -fno-builtin -DMIPSEB -DCPU=MIPS64 \
	-DTOOL_FAMILY=gnu -DTOOL=sfgnu -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
SYSTEM_FLAGS += -LC:\Tor21\host\x86-win32\mips-wrs-vxworks\lib\MIPS64gnu
endif

ifeq (${SYSTEM},linux)
SYSTEM_FLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
SYSTEM_FLAGS += -DLINUX
endif


#CFLAGS += -DBCHP_CHIP=$(BCHP_CHIP)
#CFLAGS += -DBCHP_VER=$(BCHP_VER)
#CXXFLAGS += -DBCHP_CHIP=$(BCHP_CHIP)
#CXXFLAGS += -DBCHP_VER=$(BCHP_VER)

CFLAGS += -DBCM_BOARD=$(PLATFORM)
CXXFLAGS += -DBCM_BOARD=$(PLATFORM)

