# This makefile depends on SOURCES, OBJECTS and DEPENDS defined in makefile.core.
# so it must be included after these variable definitions.

# if DBDVER is used, need to add ../DBDVER/DBDVERgitInfo.cpp to build list
ifneq ($(findstring ../DBDVER/DBDVERgitInfo.cpp,$(SOURCES)),)
    # we want to regenerate DBDVERgitInfo.cpp every time
    # $(shell rm -f ../DBDVER/DBDVERgitInfo.cpp)
    # $(shell rm -f ../DBDVER/DBDVERgitInfo.o)
    # $(shell rm -f ../DBDVER/DBDVERgitInfo.d)
else ifneq ($(findstring ../DBDVER,$(SOURCES)),)
    SOURCES := $(SOURCES) ../DBDVER/DBDVERgitInfo.cpp
    OBJECTS := $(OBJECTS) ../DBDVER/DBDVERgitInfo$(OBJEXT)
    DEPENDS := $(DEPENDS) ../DBDVER/DBDVERgitInfo.d
endif

# In case this make file is included before the 'all' target in makefile.core,
# we use an alias target for 'all' to make sure default make still makes 'all'.
all_dbdver_alias: all

../DBDVER/DBDVERgitInfo.cpp:	compose


ifdef OS
GEXE := .exe
RM := del /f /s
GITINFO := ..\git_info\git_info.exe
else
GITINFO := ../git_info/git_info
endif

../git_info/git_info$(GEXE):	../git_info/main.cpp
	cd ../git_info/ && make

compose: ../git_info/git_info$(GEXE)
	@$(GITINFO)

.PHONY: all_dbdver_alias compose

all:

clean::
	$(RM) *.o *.obj *.d DBDVERgitInfo.cpp
