pdir := macPostgres
ifeq ($(IFW_MAKEFILE), linux.makefile)
pdir := linuxPostgres
endif
ifeq ($(IFW_MAKEFILE), windows.makefile)
pdir := winPostgres
endif

app/bin:
	mkdir -p app/bin

../postgres/$(pdir)/bin/postgres:
	cd ../postgres && make

app/bin/postgres:	app/bin	../postgres/$(pdir)/bin/postgres
	cp -Rf ../postgres/$(pdir)/bin/* app/bin/
	cp -Rf ../postgres/$(pdir)/lib/ app/lib/
	cp -Rf ../postgres/$(pdir)/share/ app/share/
