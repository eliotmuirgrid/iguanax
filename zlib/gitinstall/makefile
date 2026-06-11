ifndef OS
all:	GITSRC/git

GITDIR  := $(abspath ../Iguana/app/bin/git-utils/)
ZLIBDIR := $(abspath ../.git/extLibs/)
CURLDIR := $(abspath ../.git/extLibs/)

./GITSRC/git:
	echo $(GITDIR)
	git clone https://github.com/git/git.git GITSRC
	cd GITSRC && git checkout v2.38.1
	cd GITSRC && make configure
	cd GITSRC && ./configure --prefix=$(GITDIR) --with-zlib=$(ZLIBDIR) --with-curl=$(CURLDIR)
	cd GITSRC && make -j

clean::
	rm -rf GITSRC

endif
