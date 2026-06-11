all:	IguanaX.zip

IGUANA = IguanaX.app/Contents/MacOS/iguana

# DEVKEY points to the private key and code signing certificate stored in Keychain Access
# NOTARY points to the iNTERFACEWARE Apple Developer account username and app-specific password stored in Keychain Access
DEVKEY = "Developer ID Application: Interfaceware Inc (6YMVXG636K)"
NOTARY = "iguanaxNotaryCred"

# Code signing and notarization are used to prevent Gatekeeper warnings like “IguanaX.app can’t be opened because Apple cannot check it for malicious software.“
# We need to sign the following using our Developer ID Application certificate with runtime hardening
#  * iguana binary - happens in IguanaPackageMac
#  * IguanaX.app
#  * IguanaX.dmg
# We need to notarize and staple
#  * IguanaX.dmg
#  * the IguanaX.app in IguanaX.zip
# See https://interfaceware.atlassian.net/browse/IX-1961 for more info

$(IGUANA):	../IguanaPackageMac/iguana
	cp ../IguanaPackageMac/iguana $(IGUANA)
	vtool -set-build-version macos 11.0 11.0 $(IGUANA) -o $(IGUANA)       # IX-1909 modify the Mach-O file load command to add support for older versions of macOS (Monterey)
ifeq ($(IFW_RELEASE), 1)
	rm -f "IguanaX.app/Contents/MacOS/.dummy"
	codesign --sign $(DEVKEY) --timestamp --options=runtime --entitlements entitlements.plist -f -v "IguanaX.app"
endif

# Please run:
#    brew install create-dmg
# Groan.  So creating a DMG file with a background image is a proprietary black art by Apple that involves manipulating the binary .DS_Store file in the dmg file.
# AWFUL.  The least gross solution I could find to creating a pretty looking DMG image file was to use create-dmg which you can install from brew install create-dmg
# At least create-dmg is just a shell script so it's unlikely to be screwed over by messing around with your python settings.  GROSS!!!! Eliot
# create-dmg is basically a community effort reverse engineering the format of .DS_Store files.

IguanaX.dmg:	$(IGUANA)
	-rm -f *.dmg
	#hdiutil create -volname "IguanaX" -srcfolder "IguanaX.app" -ov -format UDZO "IguanaX.dmg"  # this creates a very bare bones DMG with no pretty background.
	@command -v create-dmg >/dev/null 2>&1 || { brew install create-dmg; }
	create-dmg --volicon iguana.icns --window-pos 400 400 --icon "IguanaX.app" 210 230 --app-drop-link 630 230 --icon-size 80 --volname "IguanaX" --background background.jpg --window-size 834 740  --text-size 12 IguanaX.dmg "IguanaX.app"
ifeq ($(IFW_RELEASE), 1)
	codesign --sign $(DEVKEY) --timestamp --options=runtime -v IguanaX.dmg
ifeq ($(IFW_SKIP_NOTARIZATION), )
	xcrun notarytool submit IguanaX.dmg --keychain-profile $(NOTARY) --wait
	xcrun stapler staple IguanaX.dmg
endif
endif

IguanaX.zip:	IguanaX.dmg
	zip -r IguanaX.zip IguanaX.app
ifeq ($(IFW_RELEASE), 1)
ifeq ($(IFW_SKIP_NOTARIZATION), )
	xcrun notarytool submit IguanaX.zip --keychain-profile $(NOTARY) --wait
	xcrun stapler staple IguanaX.app
	rm -f IguanaX.zip
	zip -r IguanaX.zip IguanaX.app
endif
endif

../IguanaPackageMac/iguana:
	cd ../IguanaPackageMac/ && make

clean::	additional_clean

additional_clean:
	-cd ../ && git clean -dfx
