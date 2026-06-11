include ../make/makefile.platform

# Define the EXE_EXTENSION variable based on the system type
DEVKEY := "Developer ID Application: Interfaceware Inc (6YMVXG636K)"

# codesign --options=runtime hardens runtime for notarization
iguana:	../webpacker/webpacker
	../webpacker/webpacker --c ../CACHE/CACHEdata.c ../Iguana/app/web/ ../Iguana/app/help/ ../Iguana/app/cabundle/ ../Iguana/app/vmd/
	cd ../Iguana && make -j8
	cp ../Iguana/app/bin/iguana .
	# Leave the Iguana directory unpacked by default.
	cp CACHEdata.c.empty ../CACHE/CACHEdata.c
ifneq ($(IFW_RELEASE),)
	codesign --sign $(DEVKEY) --timestamp --options=runtime -v iguana
endif

clean:
	$(RM) ../webpacker/webpacker iguana
	cp CACHEdata.c.empty ../CACHE/CACHEdata.c

allclean:
	cd ../ && git clean -dfx

#  -j4 otherwise we kill windoze.
../webpacker/webpacker:
	cd ../webpacker && make -j8
