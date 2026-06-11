LIBRARY=hello

IFW_MAKE_SHARED_LIBRARY = 1
IFW_MAKE_LUA_EXTENSION = 1
# IFW_MAKE_USE_EXPORTS_LIST = 1

SRC=\
hello.cpp\

MODULES=\
LUAL\
COL\
lua514\

# Dependencies
# DEPS += $(IFW_OPENSSL_MODULE)

# Prevents non static function from being exported
CPPEXTRA += -fvisibility=hidden

# Static link dependent libraries like openssl
# AREXTRA += $(IFW_OPENSSL_LIB)

include ../makefiles/library.makefile
