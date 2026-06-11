###############################################################################
# Common makefile
# Included by all platforms, to ensure that all builds behave the same
###############################################################################
.PHONY: all clean info .FORCE $(DEPS) $(POSIX_DEPS) $(WIN_DEPS) $(MODULES)
.PRECIOUS: %.o %.cpp %.c %.h
.FORCE:
MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))

###############################################################################
# Default sources for a directory: All C++ source files
SRC_CALCULATED ?= $(wildcard *.cpp)

###############################################################################
# Pretty print helper
# Use by performing $(call PRINTCMD, <commandname>, <command argument>)
# NOTE : This could be more intricate by using locks to prevent two commands
#        printing over each other, but that's not the end of the world
ifneq ($(shell which expr),)
   # Try to format things using the recursive depth of make as well
   EXTRAPADDING := $(shell expr $(MAKELEVEL) \* 2)
   FULLPADDING := $(shell expr $(EXTRAPADDING) + 7)
   PREPENDLEVEL = $(FULLPADDING)
else
   # Just use the base padding
   PREPENDLEVEL = 7
endif
PREPENDCMD = "% $(PREPENDLEVEL)s "
PRINTCMD = @printf $(PREPENDCMD) "$(1)" ; echo $(2)

# Default target
all: $(DEPS) $(IFW_PLATFORM_DEPS)

###############################################################################
# List of (unique) module names
MODULES_UNIQUE = $(sort $(MODULES) $(IFW_PLATFORM_MODULES))
DEPS_UNIQUE = $(sort $(DEPS) $(IFW_PLATFORM_DEPS))

###############################################################################
# Rules to work with subdirectories
ifneq ($(IFW_RECURSIVE_MODULENAMES), 1)
# Just assume that the modules are named with their folder names
# MAKESTLIBRARYNAME is defined in posix/windows.makefile separately
LIBS = $(foreach module, $(MODULES) $(IFW_PLATFORM_MODULES), \
         ../$(module)/$(call MAKESTLIBRARYNAME, $(module)))
LIBS_UNIQUE = $(foreach module, $(MODULES_UNIQUE), \
                ../$(module)/$(call MAKESTLIBRARYNAME, $(module)))
else
# Otherwise, recursively call make on each module to get their true names
LIBS = $(foreach module, $(MODULES) $(IFW_PLATFORM_MODULES), \
         ../$(module)/$(shell $(MAKE) -C ../$(module)/ libname))
LIBS_UNIQUE = $(foreach module, $(MODULES_UNIQUE), \
                ../$(module)/$(shell $(MAKE) -C ../$(module)/ libname))
endif
# Used by sub-directory make in the event that a module name is not the same
# as the library name
libname:
	@echo $(LIBNAME)
binname:
	@echo $(BINARY)$(EXE)
# Used by other makefiles for higher-level controls
listdep:
	@echo $(MODULES_UNIQUE) $(DEPS_UNIQUE)
listsrc:
	@echo $(SRC_CALCULATED)

# Having a target depend on $(LIBS) is preferred - this builds them if needed
$(call MAKEMAPPEDRECIPES_ORDERONLY, $(MODULES_UNIQUE), $(LIBS_UNIQUE))
# The above creates recipes of the form:
# ../COL/libCOL.a: | COL
# ../FIL/libFIL.a: | FIL
# etc.

# This forces make to always evaluate whether or not a library is up-to-date:
$(LIBS_UNIQUE): .FORCE
.PHONY: $(MODULES_UNIQUE)

# Making subdirectories is tricky - have to lock them first
# By default, assume we're in a subdirectory of 'build', so we just need to
# go one level up to access other modules
BUILDDIR ?= ../
include $(MAKEFILEDIR)utils/lock.makefile
# Add some extra utility:
#    The '+' allows make to communicate with parallel submake jobs
ifneq ($(IFW_VERBOSE), 1)
   # The '@' in PRINTCMD silences the commands, while printing the module name
   MAKEMODULE = +$(call PRINTCMD, MODULE, $(1)) ; $(MAKEMODULEVERBOSE)
else
   # The '@echo' here prints only the submake invocation portion, without the
   # locking/synchronization logic
   MAKEMODULE = +@echo $(SUBMAKECMD) ; $(MAKEMODULEVERBOSE)
endif

ifeq ($(IFW_NOSUBMAKE), 1)
   MAKEMODULE = 
endif

$(MODULES_UNIQUE) $(DEPS_UNIQUE):
	$(call MAKEMODULE, $@)

LOCKFILES = $(foreach module, $(MODULES_UNIQUE) $(DEPS_UNIQUE), \
              "$(call MODULELOCKNAME, $(module))") \
            ./$(LOCKNAME)

# This only works for non-MKDIR implementations
.DELETE_ON_ERROR: $(LOCKFILES)

# Having a clean target depend on $(CLEANLIBS) causes them to get cleaned too
CLEANLIBS = $(foreach module, $(MODULES_UNIQUE) $(DEPS_UNIQUE), \
              clean-$(module))
.PHONY: $(CLEANLIBS)

STRIPCLEANPREFIX = $(strip $(subst clean-, , $(1)))

$(CLEANLIBS): module = $(call STRIPCLEANPREFIX, $@)
$(CLEANLIBS):
	$(call MAKEMODULE, $(module), clean)
	@$(call REMOVEMODULELOCK, $(module))

clean:: $(CLEANLIBS)

###############################################################################
# Rules to compile object files from source files
# Enforce that dependencies are always built before any object
%.o %.obj: %.cpp | $(DEPS_UNIQUE)
	$(call MAKEOBJ, $<, $@)

%.o %.obj: %.c | $(DEPS_UNIQUE)
	$(call MAKEOBJCC, $<, $@)

###############################################################################
# Rule to generate information about make targets
info: infocommon

# To generate the command, we actually use make to print the commands that
# get executed - that way we don't have to worry about forgetting/omitting
# any actual flags
# As an optimization, several potential dependencies are marked up-to-date, so
# it only tries to remake the target we care about
# The command that gets printed is truly the command that make will run!
REMOVE_PRINTCMD = sed 's/^printf \(.*\); echo [^&]*&& //g'
LASTLINE = tail -n1

DYNAMICDEPS = $(OBJS) $(LIBS_UNIQUE) $(DEPS_UNIQUE) $(MODULES_UNIQUE)
ASSUMEOLD = $(foreach potentialdep, $(filter-out $(strip $(1)), \
                                         $(DYNAMICDEPS) $(STATICTARGETS)), \
                      -o $(potentialdep))
ESCAPE_QUOTES = tr '\`' '"' | tr "'" '"'
GETCMD = $(MAKE) -Bn "$(strip $(1))" $(ASSUMEOLD) 2>&1 | \
            $(ESCAPE_QUOTES) | $(LASTLINE) | $(REMOVE_PRINTCMD)

infocommon:
	$(if $(strip $(SRC_CALCULATED)), $(call PRINTCMD, SRCS:, $(SRC_CALCULATED)))
	$(if $(strip $(OBJS)), $(call PRINTCMD, OBJS:, $(OBJS)))

infolib: LIBCMD = $(shell $(call GETCMD, $(LIBNAME)))
infolib:
	$(call PRINTCMD, LIB:, $(LIBNAME))
	$(call PRINTCMD, CMD:, '$(LIBCMD)')

infoexe: EXECMD = $(shell $(call GETCMD, $(BINARY)))
infoexe:
	$(call PRINTCMD, BIN:, $(BINARY))
	$(call PRINTCMD, LIBS:, $(LIBS))
	$(call PRINTCMD, CMD:, '$(EXECMD)')

# Custom query of any command defined in the makefile
showcmd-%: ASSUMEOLD =
showcmd-%: REQCMD = $(shell $(call GETCMD, $*))
showcmd-%:
	$(call PRINTCMD, CMD:, '$(REQCMD)')

.PHONY: infocommon infolib infoexe
###############################################################################
# Rule for default clean action
clean::
	rm -f *.o *.lib *.a *.obj *.d *.ilk *.exe

########
# Includes all *.d dependency files with are generated by most compilers
-include *.d