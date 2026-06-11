DMG = iguana_macos_x64.dmg
NAME = IguanaX.app
DEVKEY = "Developer ID Application: Interfaceware Inc (6YMVXG636K)"

all: $(DMG)

../Iguana/app/bin/iguana:
	cd ../Iguana && make

../IguanaController/build/Release/$(NAME):
	cd ../IguanaController && make

# Important to use -a to copy Iguana/app/ or bin/git-utils/ is gonna explode
$(DMG): ../IguanaController/build/Release/$(NAME) ../Iguana/app/bin/iguana
	pip3 install dmgbuild
	rm -rf tmp
	mkdir tmp
	cp -R ../IguanaController/build/Release/$(NAME) tmp/
	cp -a ../Iguana/app/* tmp/$(NAME)/Contents/Resources
ifeq ($(IFW_RELEASE), 1)
	codesign -s $(DEVKEY) --deep -v tmp/$(NAME)
endif
	dmgbuild -s dmg_options.py "IguanaX" $(DMG)
ifeq ($(IFW_RELEASE), 1)
	codesign -s $(DEVKEY) -v $(DMG)
endif
	$(eval VERSION=$(shell ../Iguana/app/bin/iguana --version | grep -oE "\d+\.\d+\.\d+" | tr . _))
	mv $(DMG) IguanaX_$(VERSION)_macOS_x64.dmg

clean::
	rm -rf tmp *.dmg
