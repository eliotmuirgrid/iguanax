DIRS=\
CURL\
DIR\
FIL\
TST\
CMD\
COL\
SCM\

include ../make/makefile.core
include ../libcurl/makefile.libcurl

cacert.pem:
	curl "https://curl.se/ca/cacert.pem" -o cacert.pem

all: cacert.pem $(TARGET)
