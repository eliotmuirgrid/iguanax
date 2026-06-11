LIBRARY=crypto2

IFW_MAKE_SHARED_LIBRARY = 1
IFW_MAKE_LUA_EXTENSION = 1
# IFW_MAKE_USE_EXPORTS_LIST = 1

SRC=\
crypto.cpp\

MODULES=\
LUAL\
LUA\
OSSL\
FIL\
COL\
lua514\

DEPS += $(IFW_OPENSSL_MODULE)

# Prevents non static function from being exported
CPPEXTRA += -fvisibility=hidden

# For crypto2.so to work in vanilla Lua, we must statically link openssl.
# However, openssl is already in Iguana, statically link it again in crypto2
# will result in the following require error in translator:
#    error loading module 'crypto2' from file './crypto2.so':
#    initializer function 0x104517970 not in mapped image for ./crypto2.so
# AREXTRA += $(IFW_OPENSSL_LIB)

include ../makefiles/library.makefile

WIN_AREXTRA += Bcrypt.lib \
$(base_path)/ThirdParty$(arch)/installed/openssl/lib/libcrypto.lib \
$(base_path)/ThirdParty$(arch)/installed/openssl/lib/libssl.lib
