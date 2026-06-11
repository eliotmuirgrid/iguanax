###############################################################################
# Mac makefile
# Based on Posix makefile, with a few differences in compiler settings
###############################################################################

###############################################################################
# Defines for other makefiles to perform conditionals on these settings
IFW_MAKE_USE_CLANG = 1
IFW_MAC = 1

IFW_SHARED_LIB_EXT ?= dylib

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)posix.makefile

MAC_CFLAGS = -Qunused-arguments \
             -D_DARWIN_UNLIMITED_SELECT


ifeq ($(IFW_MAKE_SHARED_LIBRARY), 1)
   MAC_CFLAGS += -fPIC
endif

CFLAGS += $(MAC_CFLAGS)
# CXXFLAGS already includes CFLAGS
# NOTE : External libraries expect AR to remain as the default posix ar
#        command, so it will pass in flags like "rcs" as the first
#        argument to create the libraries
#        Therefore, AR is not overridden with libtool
ARST = libtool
ARSTFLAGS = $(ARFLAGS_CORRECTED) -no_warning_for_no_symbols -static -s

ARDYNFLAGS = $(CFLAGS) $(IFW_ARCH_FLAGS) -framework "Foundation" \
             -framework "IOKit" -Wl,-flat_namespace \
             -undefined suppress -Wl,-U,_environ

ARSHFLAGS += $(ARDYNFLAGS) -dynamiclib
ARBNFLAGS = $(ARDYNFLAGS) -bundle

# libtool uses -o to define the output, so define the command here:
MAKESTLIBVERBOSE = $(ARST) $(ARSTFLAGS) -o $(2) $(1) $(AREXTRA_CALCULATED)

# Logic for creating "bundle" libraries:
MAKEBNLIBVERBOSE = $(ARSH) $(ARBNFLAGS) -o $(2) $(1) $(AREXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKEBNLIB = $(call PRINTCMD, BNLIB, $(2)) && $(MAKEBNLIBVERBOSE)
else
   MAKEBNLIB = $(MAKEBNLIBVERBOSE)
endif

# Lua extensions must be built as a "bundle" on Mac
ifeq ($(IFW_MAKE_LUA_EXTENSION), 1)
   MAKELIBVERBOSE = $(MAKEBNLIBVERBOSE)
   MAKELIB = $(MAKEBNLIB)
endif

IFW_CC = llvm-gcc
IFW_CXX = llvm-g++
