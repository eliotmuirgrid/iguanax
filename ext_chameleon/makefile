# Internal iNTERFACEWARE makefile for ext_hello_world native Lua module.
# We don't want this file in the customer-facing ext_hello_world directory.
# Supports win32, win64, linux and mac.

MODULE = chameleon
LIBRARY = chameleon

IFW_MAKE_SHARED_LIBRARY = 1
IFW_MAKE_USE_EXPORTS_LIST = 1

CPPEXTRA += $(IFW_PYTHON_INCLUDE)

SRC=\
ext_python_config.c\
main.cpp\

# TIME might be a dependency we can remove.

MODULES=\
NTB\
HTTP\
CURL\
NTV\
TIME\
NTAB\
NOPH\
NOTG\
NOH\
NOX\
DOM\
NODB\
NOD\
CHJ\
CHL\
ANT\
ARFX\
ARF\
ATT\
TTA\
TIC\
CARC\
CHX\
SGX\
MZIP\
GZIP\
SFI\
SGX\
SGC\
SGPY\
SGP\
CHP\
CHX\
SCC\
SGM\
DLL\
LAG\
LAN\
PC\
LUA\
DB\
TXT\
IPG\
IPA\
PCRE\
REG\
XML\
LEG\
REX\
FMT\
FIL\
PRO\
COL\
EXP\
BZP\

THIRDPARTY=\
curl \
libssh2 \
openssl

AREXTRA += $(IFW_IDN_LINK)
WIN_AREXTRA += Bcrypt.lib \
$(base_path)/ThirdParty$(arch)/installed/openssl/lib/libcrypto.lib \
$(base_path)/ThirdParty$(arch)/installed/openssl/lib/libssl.lib

include ../makefiles/library.makefile

ifeq ($(IFW_WIN), 1)
#####################################################################
#
#  Windows
$(LIBNAME): iguana_lua_export.lib

# Create the lua symbol import library from the def file.
iguana_lua_export.lib: iguana_lua_export.def
	$(call MAKEIMPORTLIB, $<, $@)

clean cleanme: clean-implib
clean-implib:
	rm iguana_lua_export.lib iguana_lua_export.exp
endif
