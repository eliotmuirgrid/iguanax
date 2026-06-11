###############################################################################
# Posix makefile
###############################################################################

###############################################################################
# Before including the common makefile, need to define a few utilities:
MAKESTLIBRARYNAME = lib$(strip $(1)).a
MAKESHLIBRARYNAME = lib$(strip $(1)).$(IFW_SHARED_LIB_EXT)
# Lua extension: always ends in .so
MAKELELIBRARYNAME = $(strip $(1)).so

ifneq ($(IFW_MAKE_LUA_EXTENSION), 1)
   ifneq ($(IFW_MAKE_SHARED_LIBRARY), 1)
      MAKELIBRARYNAME = $(MAKESTLIBRARYNAME)
   else
      MAKELIBRARYNAME = $(MAKESHLIBRARYNAME)
   endif
else
   MAKELIBRARYNAME = $(MAKELELIBRARYNAME)
endif

# Defines for other makefiles to perform conditionals/calls on these settings
IFW_POSIX = 1
IFW_PLATFORM_DEPS += $(POSIX_DEPS)
IFW_PLATFORM_MODULES += $(POSIX_MODULES)

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)settings.makefile
include $(MAKEFILEDIR)utils.makefile
include $(MAKEFILEDIR)common.makefile
include $(MAKEFILEDIR)thirdparty.makefile

###############################################################################
# Compiler/others flag setup
# CC and CXX should be overriden by mac.makefile and linux.makefile
IFW_CC ?= cc
IFW_CXX ?= c++
# IFW_DEBUG_SYMBOL_FLAGS may be overridden, but the default should suit most
# DEBUGFLAGS_EXTRA is used internally
IFW_DEBUG_SYMBOL_FLAGS ?= -g
ifneq ($(IFW_MAKE_NO_OPTIMIZE), 1)
   # Try to detect whether or not this is the default (empty) definition from
   # settings.makefile, and set it to the correct default if so
   ifeq ($(IFW_OPTIMIZE_FLAGS),)
      IFW_OPTIMIZE_FLAGS = -O2
   endif
endif
ifeq ($(IFW_MAKE_DEBUG), 1)
   DEBUGFLAGS_CALCULATED = \
      $(IFW_DEBUG_SYMBOL_FLAGS) $(IFW_DEBUG_FLAGS) $(DEBUGFLAGS_EXTRA)
endif

# 64-bit build
ifneq ($(IFW_MAKE_USE_CLANG), 1)
   # gcc prefers -m flag
   IFW_ARCH_FLAGS = -m64
else
   # clang prefers -arch flag
   IFW_ARCH_FLAGS = -arch x86_64
endif
IFW_PLATFORM = x86_64
IFW_64 = 1


ifeq ($(IFW_MAKE_USE_EXPORTS_LIST), 1)
   EXPORTSLIST ?= $(IFW_TARGET_NAME).exports_list
   EXPORTSFROMLIST = $(patsubst %.exports_list, %.exports, $(1))
   EXPORTSFILE ?= $(call EXPORTSFROMLIST, $(EXPORTSLIST))
endif

###############################################################################
# Compiler/others setup - these can be overridden/added to
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

# Defaults:
ifeq ($(call ISMAKEDEFAULT, CC), 1)
   CC = $(CCACHE) $(IFW_CC)
endif
# These flags apply to all Posix platform builds
#    -fexceptions is required on certain bizarro architecture-dependent
#       code generators. It allows exceptions thrown in C++ to propagate up
#       a call-stack, even through C code. This happens when, libgit2 runs an
#       SCMgit callback, which throws. We all blame Richard Stallman.
#    -DFD_SETSIZE is used to set the limit for the number of file descriptors
#       we can use in a call to 'select'. We set it higher than the default
#       of '1024' in certain environments.
#    -MMD generates 'make dependency' files for each compiled .cpp/.c file.
#       These dependency files can be used to correctly rebuild source files
#       when any of its included header files change.
#    -isystem dir is like the standard include flag, but it only uses the dir
#       after all standard (-I) includes. This ensures that the headers in
#       /usr/local/include are used only if the header is not part of our
#       build folder.
CFLAGS += $(IFW_ARCH_FLAGS) $(IFW_WARN_FLAGS) $(IFW_NOWARN_FLAGS) \
          $(IFW_OPTIMIZE_FLAGS) $(DEBUGFLAGS_CALCULATED) -fexceptions \
          $(IFW_CFLAGS) -isystem /usr/local/include -I../ -MMD
ifeq ($(call ISMAKEDEFAULT, CXX), 1)
   CXX = $(CCACHE) $(IFW_CXX)
endif
CXXFLAGS += $(CFLAGS) -std=c++11 
# Enforce expected 'ar rcs' pattern in Posix by default
# One thing to watch out for, though, is that make provides ARFLAGS with a
# default value of 'rv'
# When referencing ARFLAGS in ARSTFLAGS or ARSHFLAGS, that default portion
# should be removed
ifeq ($(call ISMAKEDEFAULT, AR), 1)
   AR = ar
endif
ifeq ($(call ISMAKEDEFAULT, ARFLAGS), 1)
   ARFLAGS = rcs
endif
# Precalculate the ARFLAGS minus the potential 'rv'/'rcs' in front:
ARFLAGS_ISRVFIRST = $(filter rv, $(firstword $(ARFLAGS)))
ARFLAGS_ISRCSFIRST = $(filter rcs, $(firstword $(ARFLAGS)))
ARFLAGS_ISRXFIRST = $(or $(ARFLAGS_ISRVFIRST), $(ARFLAGS_ISRCSFIRST))
ARFLAGS_CORRECTED = \
   $(strip $(if $(strip $(ARFLAGS_ISRXFIRST)), \
                $(wordlist 2, $(words $(ARFLAGS)), $(ARFLAGS)), \
                $(ARFLAGS)))
# NOTE : ARST is used internally for creating static libraries
#        It is defined as a variable, so that mac.makefile can override it
ARST ?= $(AR)
ARSTFLAGS += $(ARFLAGS)

# NOTE : Shared libraries are created with the compiler itself, for simplicity
#        It technically needs to be linked with 'ld', which needs an explicit
#        list of all objects/libraries, including the C-runtime
#        The actual flags that go into the call are different for each system,
#        so look in mac.makefile and linux.makefile for ARSHFLAGS
ARSH = $(CXX)
ARSHFLAGS += $(ARFLAGS_CORRECTED)
ifeq ($(call ISMAKEDEFAULT, LD), 1)
   LD = $(CXX)
   LDFLAGS += $(CXXFLAGS)
endif
STRIP = strip
# STRIPFLAGS is also used

# Internal build commands will use these precalculated flags:
CPPEXTRA_CALCULATED = $(CPPEXTRA) $(POSIX_CPPEXTRA)
CCEXTRA_CALCULATED = $(CCEXTRA) $(POSIX_CCEXTRA) $(CPPEXTRA_CALCULATED)
CXXEXTRA_CALCULATED = $(CXXEXTRA) $(POSIX_CXXEXTRA) $(CPPEXTRA_CALCULATED)
AREXTRA_CALCULATED = $(AREXTRA) $(POSIX_AREXTRA)
LDEXTRA_CALCULATED = $(LDEXTRA) $(POSIX_LDEXTRA) $(LDLIBS) $(POSIX_LDLIBS)
STRIPEXTRA_CALCULATED = $(STRIPEXTRA) $(POSIX_STRIPEXTRA)

###############################################################################
# Commands to generate targets
OBJSFROMSRCS=$(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(1)))
SRC_CALCULATED = $(SRC) $(POSIX_SRC)
OBJS = $(call OBJSFROMSRCS, $(SRC_CALCULATED))

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

MAKESTLIBVERBOSE = $(ARST) $(ARSTFLAGS) $(2) $(1) $(AREXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKESTLIB = $(call PRINTCMD, LIB, $(2)) && $(MAKESTLIBVERBOSE)
else
   MAKESTLIB = $(MAKESTLIBVERBOSE)
endif

MAKESHLIBVERBOSE = $(ARSH) $(ARSHFLAGS) -o $(2) $(1) $(AREXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKESHLIB = $(call PRINTCMD, SHLIB, $(2)) && $(MAKESHLIBVERBOSE)
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

MAKEBINARYVERBOSE = $(LD) $(LDFLAGS) -o $(2) $(1) $(LDEXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   MAKEBINARY = $(call PRINTCMD, LD, $(2)) && $(MAKEBINARYVERBOSE)
else
   MAKEBINARY = $(MAKEBINARYVERBOSE)
endif

STRIPBINARYOUTPUT = $(if $(strip $(2)), -o $(2))
STRIPBINARYVERBOSE = $(STRIP) $(STRIPFLAGS) $(STRIPBINARYOUTPUT) \
                     $(1) $(STRIPEXTRA_CALCULATED)
ifneq ($(IFW_VERBOSE), 1)
   STRIPBINARYDESC = $(if $(strip $(2)), $(2), $(1))
   STRIPBINARY = \
      $(call PRINTCMD, STRIP, $(STRIPBINARYDESC)) && $(STRIPBINARYVERBOSE)
else
   STRIPBINARY = $(STRIPBINARYVERBOSE)
endif

###############################################################################
# Posix-specific targets
all objs: $(OBJS)

info: infocompiler

infocompiler:
	$(call PRINTCMD, CPP:, $(call MAKEOBJVERBOSE, src.cpp, src.o))

ifneq ($(IFW_MAKE_SHARED_LIBRARY), 1)
   LIBDESC = lib.a
else
   LIBDESC = lib.$(IFW_SHARED_LIB_EXT)
endif

infoarchive:
	$(call PRINTCMD, AR:, $(call MAKELIBVERBOSE, objs.o, $(LIBDESC)))

infolink:
	$(call PRINTCMD, LD:, $(call MAKEBINARYVERBOSE, objs.o libs.a, binary))

infostrip:
	$(call PRINTCMD, STRIP:, '$(call STRIPBINARYVERBOSE, binary)')

ifneq ($(EXPORTSFILE),)
# Define these targets so long as the files exist - they are only used
# as pre-requisites when IFW_MAKE_USE_EXPORTS_LIST = 1
# NOTE : The MAKEEXPORTS command (for generating exports files from
#        exports_list) is defined in utils.makefile
#        it's a different format for Linux vs. Mac
$(EXPORTSFILE): $(EXPORTSLIST)
	$(call MAKEEXPORTS, $<, $@)

infoexports: EXPCMD = $(shell $(call GETCMD, $(EXPORTSFILE)))
infoexports:
	$(call PRINTCMD, CMD:, '$(EXPCMD)')
cleanexports:
	rm -f $(EXPORTSFILE)
endif
ifeq ($(IFW_MAKE_USE_EXPORTS_LIST), 1)
# Now that exports are requested, add the export pre-requisites
CPPEXPORTFLAG = $(call MAKESHLIBEXPORTS, $(EXPORTSFILE))
# List the export file as a dependency for objects, so both binaries and
# libraries are updated when the exports change
$(OBJS): $(EXPORTSFILE)
# Exports only apply to shared libraries and binaries
ARSHFLAGS += $(CPPEXPORTFLAG)
LDFLAGS += $(CPPEXPORTFLAG)

info: infoexports
clean:: cleanexports
# / End of exports section
endif

.PHONY: objs infocompiler infoarchive infolink infostrip
