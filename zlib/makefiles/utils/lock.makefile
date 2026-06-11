###############################################################################
# Lock utility for makefiles
# Defines a utility for creating a lock in a module to allow exclusive
# access for building within that directory
# The utility takes the path to the module and, optionally, a make target
###############################################################################

###############################################################################
# Configuration/Utilities

# NOTE : Override this variable to specify the relative path of the 'build' dir
BUILDDIR ?= ./

# Utility to join strings properly - callers must handle quoting
JOINSTRINGS ?= $(strip $(1))$(strip $(2))

LOCKNAME = make.lock
MODULEPATH = $(call JOINSTRINGS, $(BUILDDIR), $(1))
MODULELOCKNAME = "$(strip $(call MODULEPATH, $(1)/$(LOCKNAME)))"
REMOVEMODULELOCK = rm -rf $(MODULELOCKNAME)

SUBMAKEOPTS = --no-print-directory
ifneq ($(IFW_VERBOSE), 1)
   SUBMAKEOPTS += -s
endif

SUBMAKEPREFIX ?=
SUBMAKEPOSTFIX ?=

SUBMAKEPATH = "$(MODULEPATH)"
SUBMAKEPATH_ESCAPED = \"$(MODULEPATH)\"

SUBMAKETARGET = $(if $(strip $(2)), "$(strip $(2))")
SUBMAKETARGET_ESCAPED = $(if $(strip $(2)), \"$(strip $(2))\")

SUBMAKECMD = $(SUBMAKEPREFIX) $(MAKE) $(SUBMAKEOPTS) -C $(SUBMAKEPATH) \
             $(SUBMAKETARGET) $(SUBMAKEPOSTFIX)
SUBMAKECMD_ESCAPED = $(SUBMAKEPREFIX) $(MAKE) $(SUBMAKEOPTS) -C \
                     $(SUBMAKEPATH_ESCAPED) $(SUBMAKETARGET_ESCAPED) \
                     $(SUBMAKEPOSTFIX)

# MKDIR implementation - Very complicated, and buggy
# If this gets interrupted, the lock file persists, and that module can't be
# built until it is removed...
CREATEMODULEMKDIRLOCK = mkdir $(MODULELOCKNAME)
MAKEMODULEMKDIR = \
if $(CREATEMODULEMKDIRLOCK) ; then \
   $(SUBMAKECMD) ; \
   makestatus=$$? ; \
   $(REMOVEMODULELOCK) ; \
else \
   while [ -e $(MODULELOCKNAME) ] ; do \
      sleep 0.1 ; \
   done ; \
   makestatus=0 ; \
fi ; \
[ "$$makestatus" -eq 0 ]

# Some systems have flock, some systems have shlock
# This can use both effectively, and recovers from interruption perfectly

# FLOCK implementation - easiest to use, but not available on mac
MAKEMODULEFLOCK = \
   flock -x $(MODULELOCKNAME) -c \
   "$(strip $(SUBMAKECMD_ESCAPED))"

# SHLOCK implementation - works reasonably well
# When interrupted, the lock file persists, but shlock knows how to recover
# from that, deleting the lock if the process that created it is dead
MAKEMODULESHLOCK = \
   while ! shlock -p $$$$ -f $(MODULELOCKNAME) ; do \
      sleep 0.1 ; \
   done ; \
   $(SUBMAKECMD) ; \
   makestatus=$$? ; \
   $(call REMOVEMODULELOCK, $(1)) ; \
   [ "$$makestatus" -eq 0 ]

ifneq ($(shell which flock),)
   MAKEMODULEVERBOSE = $(MAKEMODULEFLOCK)
else
   ifneq ($(shell which shlock),)
      MAKEMODULEVERBOSE = $(MAKEMODULESHLOCK)
   else
      $(warning No 'flock' or 'shlock' utility present - \
        using 'mkdir'... buggy)
      MAKEMODULEVERBOSE = $(MAKEMODULEMKDIR)
   endif
endif

# External makefiles can opt to add functionality to MAKEMODULE by
# using MAKEMODULEVERBOSE to do the actual building
# e.g. MAKEMODULE = @echo module $(1) ; $(MAKEMODULEVERBOSE)
MAKEMODULE ?= $(MAKEMODULEVERBOSE)
