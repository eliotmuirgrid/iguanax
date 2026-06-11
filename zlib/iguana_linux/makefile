all: ../Iguana/app/bin/iguana
	$(eval VERSION=$(shell ../Iguana/app/bin/iguana --version_tag))
	mv ../Iguana/app ../Iguana/IguanaX_$(VERSION)
	tar zcf ../Iguana/IguanaX_$(VERSION)_Linux_x64.tar.gz ../Iguana/IguanaX_$(VERSION)
	mv ../Iguana/IguanaX_$(VERSION) ../Iguana/app
	mv ../Iguana/IguanaX_$(VERSION)_Linux_x64.tar.gz .

../Iguana/app/bin/iguana:
	cd ../Iguana && make

clean::
	rm -rf *.gz tarball