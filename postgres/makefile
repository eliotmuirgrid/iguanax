all:	src/bin/postgres

version := 12.2
tarball := postgresql-$(version).tar.gz
pdir := macPostgres
ifeq ($(IFW_MAKEFILE), linux.makefile)
pdir := linuxPostgres
endif
ifeq ($(IFW_MAKEFILE), windows.makefile)
pdir := winPostgres
endif

# See if this works on windows
mydir := $(shell pwd)

$(tarball):
	@echo "Fetching $(tarball)"
	curl https://ftp.postgresql.org/pub/source/v$(version)/$(tarball) --output $(tarball)

#TODO might be nice if we only configured once but it configures and makes in one step so it's okay.

postgres:	$(tarball)
	mkdir postgres
	tar xfz $(tarball) -C postgres --strip-components=1

src/bin/postgres: postgres
#MAKELEVEL is set to 0 to enable building postgres from this makefile, without this it will fail
ifeq ($(shell uname), Darwin)
	@echo MacOS Detected!
	MAKELEVEL=0 && cd postgres && ./configure --prefix=$(mydir)/$(pdir) && make && make install-world
	@find ./$(pdir)/bin -type f -name '*.o' | xargs -L 1 install_name_tool -change $(mydir)/$(pdir)/lib/libpq.5.dylib '@executable_path/../lib/libpq.5.dylib'
endif
ifeq ($(IFW_MAKEFILE), linux.makefile)
	export LD_RUN_PATH='$$ORIGIN/../lib';\
	MAKELEVEL=0;\
	cd postgres;\
	./configure --prefix=$(mydir)/$(pdir) --disable-rpath;\
	make;\
	make install-world
endif
ifeq ($(IFW_MAKEFILE), windows.makefile)
	cmd /C postgres.bat
endif

cleanme: clean
	@rm -rf src
	@rm -rf postgres
	@rm -rf postgresql*.tar.gz


rm:
	@rm -rf $(pdir)

rmall:
	@rm -rf macPostgres winPostgres linuxPostgres
