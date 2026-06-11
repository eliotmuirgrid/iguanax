include ../make/makefile.platform

# The default target
.PHONY: all
all: $(PLATFORM)

Linux:
	@echo "Compiling for Linux..."
	cd ../IguanaPackage && IFW_RELEASE=1 make
	cp ../IguanaPackage/iguana .
	zip IguanaX.zip iguana

macOS:
	@echo "Compiling for Mac OS X..."
	cd ../Iguana_MacOS && IFW_RELEASE=1 make
	cp ../Iguana_MacOS/IguanaX.dmg .
	cp ../Iguana_MacOS/IguanaX.zip .

# Note 'set IFW_RELEASE=1 && make' actually sets IFW_RELEASE to '1 ' and then make.
# However, 'set IFW_RELEASE=1&& make' looks ugly so makefiles should check if it's
# set instead of if its value equals 1. i.e. 'ifneq ($(IFW_RELEASE),)'
Windows:
	@echo "Compiling for Windows..."
	cd ..\IguanaPackage && set IFW_RELEASE=1 && make
	copy ..\IguanaPackage\iguana.exe iguana_setup.exe
	copy ..\IguanaPackage\iguana.exe iguana.exe
	powershell -Command "Compress-Archive -Path .\iguana_setup.exe -DestinationPath .\iguana_setup.zip"
	powershell -Command "Compress-Archive -Path .\iguana.exe -DestinationPath .\iguana.zip"
clean:
	cd .. && git clean -dfx
