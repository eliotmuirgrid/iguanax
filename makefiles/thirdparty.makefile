# Variables for including/linking against third party libraries

ifeq ($(IFW_WIN), 1)
   base_path := "C:/ProgramData"
endif
ifeq ($(IFW_POSIX), 1)
   base_path := $(HOME)
   script_path := ../ThirdPartyScripts/posix
endif

ifneq (,$(IFW_WIN))
zlib_path := $(shell cygpath -w $(base_path)/ThirdParty/installed/zlib/lib)
openssl_path := $(shell cygpath -w $(base_path)/ThirdParty/installed/openssl/lib)
pcre_path := $(shell cygpath -w $(base_path)/ThirdParty/installed/pcre/lib)
iconv_path := $(shell cygpath -w $(base_path)/ThirdParty/installed/iconv/lib)
git_path := $(shell cygpath -w $(base_path)/ThirdParty/installed/git/bin)
else
zlib_path := $(base_path)/ThirdParty/installed/zlib/lib
openssl_path := $(base_path)/ThirdParty/installed/openssl/lib
pcre_path := $(base_path)/ThirdParty/installed/pcre/lib
iconv_path :=$(base_path)/ThirdParty/installed/iconv/lib
git_path :=$(base_path)/ThirdParty/installed/git/bin
endif

# These scripts must be run before any of our libraries are built if thirdparty libraries are required
# This can be done by calling make thirdparty before the usual make -j

check_tools:
ifneq (,$(IFW_WIN))
	../ThirdPartyScripts/win/CheckForTools.bat
else
	../ThirdPartyScripts/posix/CheckForTools.sh
endif

thirdparty: check_tools $(pcre_path) $(iconv_path) $(git_path) $(openssl_path)
ifneq (,$(THIRDPARTY))
	@echo Built any thirdparty libraries that needed to be built
endif

$(zlib_path):
ifneq (,$(THIRDPARTY))
ifneq (,$(IFW_WIN))
	../ThirdPartyScripts/win/zlibInstall.bat
else
	../ThirdPartyScripts/posix/zlibInstall.sh
endif
endif

$(openssl_path): $(zlib_path)
ifneq (,$(THIRDPARTY))
ifneq (,$(IFW_WIN))
	../ThirdPartyScripts/win/opensslInstall.bat
else
	../ThirdPartyScripts/posix/opensslInstall.sh
endif
endif

$(pcre_path):
ifneq (,$(THIRDPARTY))
ifneq (,$(IFW_WIN))
	../ThirdPartyScripts/win/pcreInstall.bat
else
	../ThirdPartyScripts/posix/pcreInstall.sh
endif
endif

$(iconv_path):
ifneq (,$(THIRDPARTY))
ifneq (,$(IFW_WIN))
	../ThirdPartyScripts/win/iconvInstall.bat
else
	../ThirdPartyScripts/posix/iconvInstall.sh
endif
endif

$(git_path):	$(pcre_path)
ifneq (,$(THIRDPARTY))
ifneq (,$(IFW_WIN))
	#Does nothing currently, copy git_win into git utilities where necessary
else
	../ThirdPartyScripts/posix/gitInstall.sh
endif
endif

zlib: $(zlib)
iconv: $(iconv_path)
pcre: $(pcre_path)
openssl: zlib $(openssl)

.PHONY: zlib iconv pcre openssl check_tools

# We will only add to CPPEXTRA and LDEXTRA when specifically requested by a library's/executable's makefile
# So We'll check the variable THIRDPARTY for the existance of a library name and if it's their then we'll add its flags
# On windows we often need many additional system libraries linked so we'll add all those windows libraries, this can be cleaned up at a later time if it becomes confusing
# ORDER BELOW MATTERS it MUST be: zlib pcre openssl to link successfully on linux

ifeq (zlib, $(findstring zlib,$(THIRDPARTY)))
CPPEXTRA += $(INCLUDEFLAG)$(base_path)/ThirdParty/installed/zlib/include
LDEXTRA += $(LINKFLAG)$(base_path)/ThirdParty/installed/zlib/lib
LDEXTRA += -lz
endif

ifeq (pcre, $(findstring pcre,$(THIRDPARTY)))
CPPEXTRA += $(DEFINEFLAG)PCRE_STATIC
CPPEXTRA += $(INCLUDEFLAG)$(base_path)/ThirdParty/installed/pcre/include
LDEXTRA += $(LINKFLAG)$(base_path)/ThirdParty/installed/pcre/lib
LDEXTRA += -lpcre -lpcreposix
endif

ifeq (openssl, $(findstring openssl,$(THIRDPARTY)))
CPPEXTRA += $(INCLUDEFLAG)$(base_path)/ThirdParty/installed/openssl/include
LDEXTRA += $(LINKFLAG)$(base_path)/ThirdParty/installed/openssl/lib
POSIX_LDEXTRA += -lssl -lcrypto
WIN_LDEXTRA += -lssl -lcrypto -lcrypt32 -lgdi32 -lws2_32
endif

ifeq (iconv, $(findstring iconv,$(THIRDPARTY)))
CPPEXTRA += $(INCLUDEFLAG)$(base_path)/ThirdParty/installed/iconv/include
LDEXTRA += $(LINKFLAG)$(base_path)/ThirdParty/installed/iconv/lib
endif

ifeq (git, $(findstring git,$(THIRDPARTY)))
ifneq (,$(IFW_WIN))
GIT_BIN_PATH = ../git_win
else
GIT_BIN_PATH = $(base_path)/ThirdParty/installed/git/bin
endif
endif

# Only delete the thirdparty directory for the architecture called to not incur more wait time than necessary
cleanall: clean
	rm -rf $(base_path)/ThirdParty