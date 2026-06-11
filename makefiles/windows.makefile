###############################################################################
# Windows makefile
###############################################################################

###############################################################################
# Before including the common makefile, need to define a few utilities:
MAKESTLIBRARYNAME = $(strip $(1)).lib
MAKESHLIBRARYNAME = $(strip $(1)).dll

ifneq ($(IFW_MAKE_SHARED_LIBRARY), 1)
   MAKELIBRARYNAME = $(MAKESTLIBRARYNAME)
else
   MAKELIBRARYNAME = $(MAKESHLIBRARYNAME)
endif

# Defines for other makefiles to perform conditionals on these settings
IFW_WIN = 1
IFW_PLATFORM_DEPS += $(WIN_DEPS)
IFW_PLATFORM_MODULES += $(WIN_MODULES)

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)settings.makefile
include $(MAKEFILEDIR)utils.makefile
include $(MAKEFILEDIR)common.makefile
include $(MAKEFILEDIR)thirdparty.makefile

###############################################################################
# Compiler/others flag setup
IFW_CC = gcc
IFW_CXX = g++
# O2 
ifneq ($(IFW_MAKE_NO_OPTIMIZE), 1)
   IFW_OPTIMIZE_FLAGS ?= -O2
else
   IFW_OPTIMIZE_FLAGS ?= -Og
endif

# If the above isn't enough, just set IFW_DEBUG_SYMBOL_FLAGS to whatever
ifeq ($(IFW_MAKE_DEBUG), 1)
   IFW_DEBUG_SYMBOL_FLAGS ?= -g
endif

# These defines can be overriden, but the defaults should suit most
IFW_DEBUG_ON_DEFINES ?= -D_DEBUG
IFW_DEBUG_OFF_DEFINES ?= -DNDEBUG -DCOL_DEBUG_LEVEL=0

# DEBUG_DEFINE_FLAGS is used internally
ifeq ($(IFW_MAKE_DEBUG), 1)
   DEBUG_DEFINE_FLAGS += $(IFW_DEBUG_ON_DEFINES)
else
   DEBUG_DEFINE_FLAGS += $(IFW_DEBUG_OFF_DEFINES)
endif

# Now calculate the final flags
DEBUGFLAGS_CALCULATED += $(IFW_DEBUG_SYMBOL_FLAGS) $(DEBUG_DEFINE_FLAGS)
ifeq ($(IFW_MAKE_DEBUG), 1)
   # To be consistent with how Posix uses IFW_DEBUG_FLAGS, we only add these
   # flags when IFW_MAKE_DEBUG is enabled
   DEBUGFLAGS_CALCULATED += $(IFW_DEBUG_FLAGS) 
endif

# 64-bit build
IFW_ARCH_FLAGS +=
IFW_PLATFORM = x86_64
IFW_64 = 1

###############################################################################
# Compiler/others setup - these can be overridden/added to
# Removed clcache due to inconsistent performance
ifneq ($(IFW_MAKE_NO_CCACHE), 1)
   ifneq ($(shell which ccache),)
      CCACHE = ccache
   else
      ifneq ($(IFW_MAKE_NO_WARNINGS), 1)
         ifeq ($(IFW_VERBOSE), 1)
            @echo No ccache found
         endif
      endif
   endif
endif

ifeq ($(call ISMAKEDEFAULT, CC), 1)
   CC = $(CCACHE) $(IFW_CC)
endif
CFLAGS += $(IFW_WARN_FLAGS) $(IFW_NOWARN_FLAGS) $(IFW_OPTIMIZE_FLAGS) $(IFW_EXCEPTION_FLAGS) \
          $(DEBUGFLAGS_CALCULATED) $(IFW_CFLAGS) -MMD -I../ -DCHM_VC9 -DCOL_ASSERTS_ON -D_MBCS
ifeq ($(call ISMAKEDEFAULT, CXX), 1)
   CXX = $(CCACHE) $(IFW_CXX)
endif
CXXFLAGS += $(CFLAGS) -std=c++11
ifeq ($(call ISMAKEDEFAULT, AR), 1)
   AR = ar -ru
   # GNU make defaults ARFLAGS to "rv", which we don't want, so clear it out
   ARFLAGS = $(IFW_ARCH_FLAGS) $(IFW_NOWARN_NO_SYMBOLS)
   endif
ARSHFLAGS = $(ARFLAGS) $(LD_DEBUGFLAGS) /dll /OPT:REF /version:1.0 \
            /subsystem:windows /IGNORE:4089 /IGNORE:4221
ifeq ($(call ISMAKEDEFAULT, LD), 1)
   LD = $(CXX)
   LDFLAGS += -Bstatic -static-libgcc -static-libstdc++ $(CXXFLAGS)
endif

#TODO do we need to use rc still?
RES = windres
RESFLAGS = -r

#TODO remove mt usage?
MT = mt /nologo


# gcc doesn't seem to generate .pdb files, .map files can provide interesting information 
# but do we need them on windows if we don't use them on mac or linux?
# Add the .pdb and .map generation flags
#PDBFLAGS = $(call QUOTEFLAG, /PDB:, $(WIN_PDB_OUT))
#ifneq ($(WIN_PDB_STRIPPED_OUT),)
#   PDBSTRIPPEDFLAGS = \
#      $(call QUOTEFLAG, /PDBSTRIPPED:, $(WIN_PDB_STRIPPED_OUT))
#endif
#MAPFLAGS = $(call QUOTEFLAG, /MAP:, $(WIN_MAP_OUT))
#
#ARSHFLAGS += $(PDBFLAGS) $(PDBSTRIPPEDFLAGS) $(MAPFLAGS)
#LDFLAGS += $(PDBFLAGS) $(PDBSTRIPPEDFLAGS) $(MAPFLAGS) /MAPINFO:EXPORTS

#TODO is this necessary? Only used in shared libs which we have almost none of anymore
# If we opt to manually link with the libraries required, which we ought to do,
# then these are the libraries we'll want to use
WIN_CPPEXTRA += -I"C\:\\mingw-w64\\mingw32\\i686-w64-mingw32\\include" -D_WIN32_WINNT=0x0600
WIN_LDEXTRA += -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
WIN_LDEXTRA += -lshell32 -lbcrypt -ladvapi32 -lrpcrt4 -lole32 -lwinhttp -lnormaliz
WIN_LDEXTRA += -lcomdlg32 -lkernel32 -lwldap32 -lwinmm -luser32 -lmsvcrt
WIN_LDEXTRA += -ldbghelp -liphlpapi -lodbc32 -lodbccp32 -loleaut32
WIN_LDEXTRA += -luuid -lversion -lwinspool -lws2_32
WIN_LDEXTRA += -lpsapi

ifneq ($(IFW_MAKE_ALLOW_DEFAULT_LIB), 1)
   IFW_NODEFAULTLIB_FLAG = /NODEFAULTLIB
endif

CPPEXTRA_CALCULATED = $(CPPEXTRA) $(WIN_CPPEXTRA)
CCEXTRA_CALCULATED = $(CCEXTRA) $(WIN_CCEXTRA) $(CPPEXTRA_CALCULATED)
CXXEXTRA_CALCULATED = $(CXXEXTRA) $(WIN_CXXEXTRA) $(CPPEXTRA_CALCULATED)
AREXTRA_CALCULATED = $(AREXTRA) $(WIN_AREXTRA)
LDEXTRA_CALCULATED = $(LDEXTRA) $(WIN_LDEXTRA) $(LDLIBS) $(WIN_LDLIBS)

###############################################################################
# Commands to generate targets
OBJSFROMSRCS=$(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(patsubst %.rc, %.res, $(1))))
SRC_CALCULATED = $(SRC) $(WIN_SRC)
OBJS = $(call OBJSFROMSRCS, $(SRC_CALCULATED))

QUOTEFLAG = $(strip $(1))"$(strip $(2))"

MAKEOBJVERBOSE = $(CXX) $(CXXFLAGS) -c -o $(2) $(1) $(CXXEXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKEOBJ = $(call PRINTCMD, CPP, $(1)) && $(MAKEOBJVERBOSE)
else
   MAKEOBJ = $(MAKEOBJVERBOSE)
endif

MAKEOBJCCVERBOSE = $(CC) $(CFLAGS) -c -o $(2) $(1) $(CCEXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKEOBJCC = $(call PRINTCMD, CC, $(1)) && $(MAKEOBJCCVERBOSE)
else
   MAKEOBJCC = $(MAKEOBJCCVERBOSE)
endif

MAKERESVERBOSE = $(RES) $(RESFLAGS) -o $(2) $(1) $(RESEXTRA) $(WIN_RESEXTRA)
ifneq ($(IFW_VERBOSE), 1)
   MAKERES = $(call PRINTCMD, RES, $(1)) && $(MAKERESVERBOSE)
else
   MAKERES = $(MAKERESVERBOSE)
endif

MAKESTLIBVERBOSE = $(AR) $(ARFLAGS) $(2) $(1) $(AREXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKESTLIB = $(call PRINTCMD, LIB, $(2)) && $(MAKESTLIBVERBOSE)
else
   MAKESTLIB = $(MAKESTLIBVERBOSE)
endif

# Since dynamic libraries are created with the linker, we automatically
# include the Windows base libraries in the link
# UGH - link.exe will automatically generate import libraries and export files
# when linking DLLs that have explicit dllexport directives, so automatically
# trim that line if it appears
TRIM_SHLIB_IMPLIB_LINE = \
   | sed '/^\s*Creating library .*\.lib and object .*\.exp$$/d' ; \
     exit $${PIPESTATUS[0]}
MAKESHLIBVERBOSEBASE = $(LD) $(ARSHFLAGS) $(call QUOTEFLAG, /OUT:, $(2)) \
                       $(1) $(IFW_NODEFAULTLIB_FLAG) \
                       $(IFW_WIN_LIBS) $(AREXTRA_CALCULATED)
MAKESHLIBVERBOSE = ( $(MAKESHLIBVERBOSEBASE) $(TRIM_SHLIB_IMPLIB_LINE) )
ifneq ($(IFW_VERBOSE), 1)
   MAKESHLIB = $(call PRINTCMD, LIB, $(2)) && $(MAKESHLIBVERBOSE)
else
   MAKESHLIB = $(MAKESHLIBVERBOSE)
endif

ifneq ($(IFW_MAKE_SHARED_LIBRARY), 1)
   MAKELIBVERBOSE = $(MAKESTLIBVERBOSE)
   MAKELIB = $(MAKESTLIB)
else
   MAKELIBVERBOSE = $(MAKESHLIBVERBOSE)
   MAKELIB = $(MAKESHLIB)
endif

MAKEBINARYVERBOSE = $(LD) $(LDFLAGS) -o $(2).exe $(1) $(LDEXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKEBINARY = $(call PRINTCMD, LD, $(2)) && $(MAKEBINARYVERBOSE)
else
   MAKEBINARY = $(MAKEBINARYVERBOSE)
endif

MAKEMANIFESTVERBOSE = $(MT) $(call QUOTEFLAG, /OUTPUTRESOURCE:, $(2)) \
                      /MANIFEST $(1)
ifneq ($(IFW_VERBOSE), 1)
   MAKEMANIFEST = $(call PRINTCMD, MT, $(2)) && $(MAKEMANIFESTVERBOSE)
else
   MAKEMANIFEST = $(MAKEMANIFESTVERBOSE)
endif

MAKEIMPORTLIBVERBOSE = $(AR) $(IFW_ARCH_FLAGS) \
                       $(call QUOTEFLAG, /DEF:, $(1)) \
                       $(call QUOTEFLAG, /OUT:, $(2))
ifneq ($(IFW_VERBOSE), 1)
   MAKEIMPORTLIB = $(call PRINTCMD, IMPLIB, $(2)) && $(MAKEIMPORTLIBVERBOSE)
else
   MAKEIMPORTLIB = $(MAKEIMPORTLIBVERBOSE)
endif

###############################################################################
# Windows-specific targets
all objs: $(OBJS)

%.res: %.rc | $(DEPS)
	$(call MAKERES, $<, $@)

info: infocompiler

infocompiler:
	$(call PRINTCMD, CPP:, $(call MAKEOBJVERBOSEBASE, src.cpp, src.obj))

infoarchive:
	$(call PRINTCMD, AR:, $(call MAKELIBVERBOSE, objs.obj, lib.lib))

infolink:
	$(call PRINTCMD, LD:, \
	  $(call MAKEBINARYVERBOSE, objs.obj libs.lib, binary.exe))

# For binaries/shared libraries, clean up the linker cache files as well
LD_ILK_OUT = $(IFW_TARGET_NAME).ilk
LD_PDB_OUT = $(WIN_PDB_OUT)
LD_PDB_STRIPPED_OUT = $(WIN_PDB_STRIPPED_OUT)
LD_MAP_OUT = $(WIN_MAP_OUT)
LD_IMPLIB_OUT = $(IFW_TARGET_NAME).lib
LD_EXPORT_OUT = $(IFW_TARGET_NAME).exp
LD_FILES = $(LD_ILK_OUT) $(LD_MAP_OUT) $(LD_PDB_OUT) \
           $(LD_PDB_STRIPPED_OUT) $(LD_IMPLIB_OUT) $(LD_EXPORT_OUT)

.PHONY: objs
.PHONY: infocompiler infoarchive infolink infocsc infosn

# Make path to NSIS available to other makefiles
nsis_path := $(shell cygpath $(MAKEFILEDIR)../nsis/)
