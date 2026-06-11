###############################################################################
# Linux makefile
# Based on Posix makefile, with a few differences in compiler settings
###############################################################################

###############################################################################
# Defines for other makefiles to perform conditionals on these settings
IFW_LINUX = 1

IFW_SHARED_LIB_EXT ?= so

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)posix.makefile

# On Linux, always compile explicitly with -fPIC
#    -fPIC is required for linking shared libraries correctly.
LINUX_CFLAGS = -fPIC
# Automatically link pthread and dl on Linux
# COL depends on them, and since it's such a central library, it's easier
# if it is just automatically linked in, instead of having to specify it
# for every binary makefile
LINUX_LDLIBS = -lpthread -ldl

# These flags help produce better backtraces on Linux
# -fno-omit-frame-pointer ensures that each function call gets a position
#    on the call stack so they appear on the backtrace separately
# -rdynamic leaves all symbols exposed, allowing function names and offsets
#    to appear properly in the backtrace
ifeq ($(IFW_MAKE_DEBUG), 1)
   DEBUGFLAGS_EXTRA += -fno-omit-frame-pointer
   LINUX_LDLIBS += -rdynamic
else
   ifeq ($(IFW_MAKE_USE_EXPORTS_LIST), 1)
      # Still need -rdynamic so that the exported symbols on binaries are
      # made available to any dynamically loaded libraries (e.g. via dlopen)
      LINUX_LDLIBS += -rdynamic
   endif
endif

CFLAGS += $(LINUX_CFLAGS)
POSIX_LDLIBS += $(LINUX_LDLIBS)

ARSHFLAGS += $(CFLAGS) -shared

#Do we need the clang option on?
ifeq ($(IFW_MAKE_USE_CLANG), 1)
   ifneq ($(shell which llvm-gcc),)
      IFW_CC = llvm-gcc
   else
      ifneq ($(shell which clang),)
         IFW_CC = clang
      else
         $(error Could not find clang binary)
      endif
   endif
   ifneq ($(shell which llvm-g++),)
      IFW_CXX = llvm-g++
   else
      ifneq ($(shell which clang++),)
         IFW_CXX = clang++
      else
         $(error Could not find clang++ binary)
      endif
   endif
else
   IFW_CC = gcc
   IFW_CXX = g++
endif
