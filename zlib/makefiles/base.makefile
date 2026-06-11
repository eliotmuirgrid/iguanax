###############################################################################
# Base makefile
# Just includes the platform definitions so that a package can be built
# without using the predefined binary/library makefile templates
###############################################################################

ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(detected_OS),Windows)
	IFW_MAKEFILE := windows.makefile
endif
ifeq ($(detected_OS),Darwin)
	IFW_MAKEFILE := mac.makefile
endif
ifeq ($(detected_OS),Linux)
	IFW_MAKEFILE := linux.makefile
endif

MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILEDIR)$(IFW_MAKEFILE)
