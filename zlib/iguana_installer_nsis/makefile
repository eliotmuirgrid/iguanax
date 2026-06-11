NSIS = iguana_windows_x64_setup.exe
NSIS_SCRIPT = main.nsi
NSIS_EXE = ../nsis/makensis.exe
#SIGN_TOOL = /c/Program\ Files\ \(x86\)/Windows\ Kits/10/bin/10.0.22000.0/x64/signtool.exe

all: $(NSIS)

# TODO: use make -j<num_core> to speed up Iguana build. Server spec dependent
# Once Iguana is built, we clean up the files we don't need
# TODO: Move the pdb?
../Iguana/app/bin/iguana:
	cd ../Iguana && make -j
	cd ../Iguana/app/bin && del iguana.exp && del iguana.lib && del iguana.ilk && del iguana.pdb

$(NSIS): $(NSIS_SCRIPT) ../Iguana/app/bin/iguana
	del /s /f  "$@"
	$(eval VERSION=$(shell ../Iguana/app/bin/iguana --version_tag )) 
	$(NSIS_EXE) -V3 -DIFW_VERSION=$(VERSION) -DIFW_OUTFILE=$(NSIS) $<
ifneq ($(IFW_RELEASE),)
	signtool sign /t http://timestamp.digicert.com /fd SHA256 /v /sm "$@"
endif
	copy $@ IguanaX_$(VERSION)_Windows_x64.exe

# N.B. Iguana is not cleaned for you when you make clean
clean::
	@del /f /s *.exe 
