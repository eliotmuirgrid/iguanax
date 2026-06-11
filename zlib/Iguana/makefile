TARGET = app/bin/iguana

# REG is a Windows dependency

DIRS=\
APP\
BRO\
LINK\
CACHE\
CSV\
CRH\
DOC\
EXPT\
TUN\
TWEB\
POP\
DBG\
DBD\
ACT\
SERV\
GITE\
GINI\
GINR\
GINS\
INI\
IP\
INST\
CFG\
CMPA\
CMPC\
CMPL\
DELC\
SDM\
REG\
SIG\
RGN\
DBDVER\
DBDUP\
IDB\
IWEB\
IWEBA\
CRT\
PRT\
FSB\
LOGG\
LOGR\
LOGV\
PRM\
USR\
ROL\
LOGE\
LOG\
DASH\
DIR\
SEC\
INFO\
TPJ\
CMP\
ENV\
PASS\
PACK\
NOTI\
HASH\
BLUA\
GRID\
GRIDA\
GRIDS\
GRIDI\
ICOL\
LQAPI\
SDB\
SDBA\
SDBS\
SDBU\
GUID\
CXML\
LHL7\
LX12\
EDIC\
EDI\
LUAL\
NOD\
NODL\
CURL\
LAPI\
LJSON\
LENV\
LSTR\
LUTIL\
LSCK\
LIG\
LLOG\
LTUN\
LCR\
LXML\
LCOM\
LTMR\
NOX\
DOM\
XML\
EXP\
LEG\
FMT\
LNET\
LTEST\
HTTP\
WEB\
USG\
WEBI\
SCKB\
LEDI\
LDB\
LND\
NTV\
NODB\
DBS\
DBSQL\
SQL\
SQLTE\
DBIM\
NTAB\
DB\
LOSE\
PRO\
LIV\
LUI\
TXT\
ICONV\
REX\
LSFI\
MZIP\
GZIP\
STMZIP\
BZP\
LUA\
LUAC\
TRNC\
THTM\
GITW\
GIT\
GITU\
GITC\
GITS\
GSV\
HA\
ENVC\
CHNK\
TRN\
DRX\
DIS\
LUACX\
SMPL\
LHLP\
NOB\
TBM\
TSM\
SFI\
SCKS\
SCK\
CMD\
FIL\
COL\
PCRE\
HTOOL\
SRCH\
HELP\
DEP\
DTL\
MATCH\
PEER\
LIB\
SSO\

include ../make/makefile.platform

ifeq (${PLATFORM},macOS)
   OBJECTS_EXTRA := APPmainCocoa.oo
endif
ifeq (${PLATFORM},Windows)
   DIRS += LUACOM
   OBJECTS_EXTRA := Iguana.res IguanaIcon.res
endif

include ../make/makefile.core
include ../libcurl/makefile.libcurl
include ../xmlsec/makefile.xmlsec

%.res: %.rc
	rc /r $<

# Compile message file
Iguana.rc: Iguana.mc
	mc Iguana.mc

ifeq (${PLATFORM},macOS)
   DEFAULT_LDFLAGS += -framework Cocoa
endif

ifdef OS
  target_bin := $(subst /,\,$(target_bin))
endif

# N.B. MAC_ARCH and PLATFORM are defined in makefile.core
#$(target_bin): $(TARGET) app/bin
#	$(CP) $(TARGET) $(target_bin)
#ifeq (${MAC_ARCH},arm64)
#	codesign -s - app/bin/$(TARGET)
#endif

ifeq (${PLATFORM},Linux)
all:	$(TARGET)
endif

cleanall:: clean allclean
allclean:: clean
	cd ../zlib/    && make clean
	cd ../OpenSSL/ && make clean
	cd ../libssh2/ && make clean
	cd ../libcurl/ && make clean
	cd ../libxml2/ && make clean
	cd ../libxslt/ && make clean
	cd ../xmlsec/  && make clean
ifdef OS # Windows
	if exist ..\.git\extLibs rmdir /S /Q ..\.git\extLibs
else  # POSIX
	$(RM) -r ../.git/extLibs
endif
