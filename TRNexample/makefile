DIRS=\
COL\
WEBI\
PACK\
CACHE\
CXML\
EXP\
SCK\
HELP\
REX\
PCRE\
FIL\
LXML\
EDI\
SFI\
NOD\
NOB\
LND\
GZIP\
DIS\
DIR\
LDB\
NODL\
LUA\
LUAC\
LUACX\
LUAX\
LUAL\
LJSON\
LHLP\
LHL7\
LEDI\
LENV\
ENV\
ENVC\
FMT\
NODB\
DB\
DBSQL\
SQL\
SQLTE\
REG\
NOX\
XML\
DOM\
LEG\
NTAB\
PRO\
SMPL\
THTM\
CFG\
UNIT\
NTV\
CMD\
TSM\
TBM\
LSTR\
CHNK\
TRN\
DEP\
SRCH\
PASS\
GUID\
PRM\
ROL\
HASH\
GIT\
GITU\
INI\
POP\
GSV\
HTOOL\
SEC\
BRO\
SDB\
MATCH\
TXT\
ICONV\
INFO\

# LUACOM can only be added on windows
include ../make/makefile.platform
ifeq (${PLATFORM},Windows)
   DIRS += LUACOM
endif

include ../make/makefile.core
include ../OpenSSL/makefile.openssl
include ../zlib/makefile.zlib

# EDI cache shouldn't be in LND
# Does HELP need to be part of the core?
# Why does HELP depend on CFG?
# LHLP ?  Why is this a dependency of TRN
# bugger - help indexing even brings in CURL...
