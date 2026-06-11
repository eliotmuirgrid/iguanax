include ../make/makefile.platform

TARGET_PDB := $(shell git rev-parse HEAD).pdb

# Define the EXE_EXTENSION variable based on the system type
ifeq ($(OS),Windows_NT)
	IGUANA_EXECUTABLE := ../Iguana/app/bin/iguana.exe
	CERT_FINGERPRINT := 4772277ecaa91dff886203ab9aae4e7cfb0e919c
	PDB_CLEAN := del ..\Iguana\app\pdb\*.pdb
	PDB_COPY := copy ..\Iguana\$(TARGET_PDB) ..\Iguana\app\pdb
else
	IGUANA_EXECUTABLE := ../Iguana/app/bin/iguana
ifeq ($(PLATFORM),macOS)
	DEVKEY = "Developer ID Application: Interfaceware Inc (6YMVXG636K)"
endif
endif

iguana:	../webpacker/webpacker
	echo "Run the tool on iguana etc and make the package"
	../webpacker/webpacker --binary ${IGUANA_EXECUTABLE} ../Iguana/app/web/ ../Iguana/app/help/ ../Iguana/app/pdb/ ../Iguana/app/cabundle/ ../Iguana/app/vmd/
ifneq ($(IFW_RELEASE),)
	echo "Sign the iguana binary"
ifeq ($(PLATFORM),Windows)
	signtool.exe sign /fd sha256 /tr http://ts.ssl.com /td sha256 /sha1 $(CERT_FINGERPRINT) iguana.exe
else ifeq ($(PLATFORM),macOS)
	codesign -s $(DEVKEY) -v iguana
endif
endif

clean:
	cd ../ && git clean -dfx

#  -j4 otherwise we kill windoze.
../webpacker/webpacker: ../Iguana/app/bin/iguana
	cd ../webpacker && make -j4

../Iguana/app/bin/iguana:
	cd ../Iguana && make -j4
	$(PDB_CLEAN)
	$(PDB_COPY)
