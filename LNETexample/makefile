DIRS=\
INFO\
ICONV\
TXT\
MATCH\
SDB\
COL\
CXML\
EXP\
SCK\
HELP\
WEBI\
PACK\
CACHE\
REX\
PCRE\
FIL\
EDI\
SFI\
NOD\
NOB\
LND\
GZIP\
DIS\
DIR\
LUA\
LUAC\
LUACX\
LUAX\
LUAL\
LHLP\
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
INI\
POP\
SEC\
BRO\

# LUACOM can only be added on windows
include ../make/makefile.platform
ifeq (${PLATFORM},Windows)
   DIRS += LUACOM
endif

include ../make/makefile.core
include ../libcurl/makefile.libcurl

# EDI cache shouldn't be in LND
# Does HELP need to be part of the core?
# Why does HELP depend on CFG?
# LHLP ?  Why is this a dependency of TRN
# bugger - help indexing even brings in CURL...
