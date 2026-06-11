DIRS=\
TST\
CMD\
SQL\
SQLTE\
FIL\
PCRE\
COL\

# Need to add the following CFLAGS to build SQLTE/sqlite3.c
#   -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_LOCKING_STYLE=0 -DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 -D_HAVE_SQLITE_CONFIG_H -DSQLITE_TEMP_STORE=3 -DSQLITE_ENABLE_FTS3_PARENTHESIS=1 -DSQLITE_ENABLE_FTS3=1 -DSQLITE_HAS_CODEC=1
# Also need -Wno-deprecated-declarations or upgrade sqlite3 to v3.18.0+
# SQLTE/shell.c must be removed, or we have duplicate symbol '_main' issue

include ../make/makefile.core
