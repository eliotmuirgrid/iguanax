MODULE = ifware_license
LIBRARY = ifware_license

IFW_MAKE_SHARED_LIBRARY = 1
IFW_MAKE_LUA_EXTENSION = 1
IFW_MAKE_USE_EXPORTS_LIST = 1

SRC=\
ifware_license.cpp

MODULES=\
COL\
DRG\
RGN\
SFI\
REG\
FIL\

#
# Note for WIN_MODULES:
#
# This is a temporary hack that allows ifware_license.dll to
# work with tests from NODLexample and TRANexample on Windows.
# Having a Lua extension DLL statically link against a Lua library
# is technically WRONG.
# Also note that the Lua library linked here must be the EXACT
# SAME Lua library linked into in the host process.
# You can not mix and match lua514 and LuaJIT202, for example.
# Static linking lua into this DLL amounts to a second Lua library
# in the host process' memory.
# At some time in the future this makefile will be converted to use
# a proper import library of Lua symbols. The trick is how to make
# this work without using an external lua DLL, and not being tied
# to a particular exe. There's got to be a way to do this.
#
WIN_MODULES=$(IFW_LUA_MODULE)

include ../makefiles/library.makefile
