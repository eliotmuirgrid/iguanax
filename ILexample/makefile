# This example directory is just to ensure we do not break the builds
# of IguanaLicense.
include ../make/makefile.core
include ../libcurl/makefile.libcurl

$(TARGET): ../IguanaLicense/IguanaLicense

../IguanaLicense/IguanaLicense:
	cd ../IguanaLicense && make

clean:: 
	cd ../IguanaLicense && make clean