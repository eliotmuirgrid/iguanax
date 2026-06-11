###############################################################################
# Library makefile
# Builds a library target
###############################################################################
LIBRARY ?= $(notdir $(shell pwd))
MAKE_TARGET = lib

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)base.makefile

# LIBNAME can be set in a module makefile to manually set the library name
LIBNAME ?= $(call MAKELIBRARYNAME, $(LIBRARY))

all: $(LIBNAME)

$(LIBNAME): $(OBJS) | $(DEPS_UNIQUE)
	$(call MAKELIB, $^, $@)

ifeq ($(IFW_MAKE_SHARED_LIBRARY), 1)
# Shared libraries should contain dependency code, or they may fail to load
$(LIBNAME): $(LIBS)
endif

info: infoarchive infolib
