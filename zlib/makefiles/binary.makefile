###############################################################################
# Binary makefile
# Builds a binary target
###############################################################################
BINARY ?= $(notdir $(shell pwd))
MAKE_TARGET = exe

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)base.makefile

# NOTE : For Windows, it's important to use the correct file name, including
#        the .exe suffix, when specifying the main target/prerequisite
#        Without it, make will treat the file as missing, and always consider
#        it out-of-date, i.e. requiring a rebuild
#        Internal targets do not need to worry about this distinction, and
#        should always use $(BINARY)
#        The .SECONDARY designation here allows $(BINARYEXE) and $(BINARY) to
#        be used interchangeably, so prerequisites are calculated correctly
#        BUT, there is an edge-case...
#        If a file with the same name exists (e.g. when re-using a build
#        directory from Posix to Windows, without cleaning), then the $(BINARY)
#        target will appear to be outdated
#        This is easily solved by also updating the existing $(BINARY) file
#        timestamp to match the newly built $(BINARYEXE)
ifeq ($(IFW_WIN), 1)
#####
# Use BINARYEXE helper
BINARYEXE = $(strip $(BINARY)).exe

ifneq ($(IFW_VERBOSE), 1)
$(BINARYEXE): quiet=@
endif

$(BINARYEXE): $(BINARY)
	$(quiet)[ -e "$(strip $@)" ] || \
	   ( rm -f "$(strip $<)" && \
	     echo "error: encountered invalid state..." \
	          "applied fix; please rebuild" >&2 && \
	     exit 1 )
	$(quiet)[ ! -e "$(strip $<)" ] || touch "$(strip $<)" -r "$(strip $@)"

.SECONDARY: $(BINARY)

else
#####
# BINARYEXE helper is not required
BINARYEXE = $(BINARY)
endif

all: $(BINARYEXE)

# Note the use if $+ instead of $^ in the recipe below
# $+ allows duplicate entries, which is important for linking on linux
$(BINARY): $(OBJS) $(LIBS) | $(MODULES) $(DEPS)
	$(call MAKEBINARY, $+, $@)

ifeq ($(IFW_POSIX), 1)
# Support for stripping binaries
strip: $(BINARY)
	$(call STRIPBINARY, $<)
# Define a helpful strip target in all binary modules for quick testing
$(BINARY).stripped: $(BINARY)
	$(call STRIPBINARY, $<, $@)

ifeq ($(IFW_MAKE_STRIP_BINARY), 1)
all: strip
info: infostrip
endif

.PHONY: strip
endif

info: infolink infoexe

clean::
	rm -f $(BINARYEXE)