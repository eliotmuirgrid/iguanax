# Internal iNTERFACEWARE makefile for ext_hello_world native Lua module.
# We don't want this file in the customer-facing ext_hello_world directory.
# Supports win32, win64, linux and mac.

MODULE = hello
LIBRARY = hello

IFW_MAKE_SHARED_LIBRARY = 1
IFW_MAKE_USE_EXPORTS_LIST = 1

SRC = ../ext_hello_world/hello.cpp

include ../makefiles/library.makefile

ifeq ($(IFW_WIN), 1)
#####################################################################
#
#  Windows
$(LIBNAME): iguana_lua_export.lib

# Create the lua symbol import library from the def file.
iguana_lua_export.lib: ../ext_hello_world/iguana_lua_export.def
	$(call MAKEIMPORTLIB, $<, $@)

endif

clean::
	rm -f iguana_lua_export.lib iguana_lua_export.exp

EXT_INTERMEDIATE_EXTS = o obj dll map lib d
EXT_INTERMEDIATE_FILES = \
$(foreach ext, $(EXT_INTERMEDIATE_EXTS), \
               $(wildcard ../ext_hello_world/*.$(ext)))
	rm -f $(EXT_INTERMEDIATE_FILES)

