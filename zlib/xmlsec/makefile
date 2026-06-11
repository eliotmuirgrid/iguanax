.NOTPARALLEL:
.ONESHELL:

EXTLIBDIR := $(abspath ../.git/extLibs/)
EXTLIBDIR_WIN := $(subst /,\,$(EXTLIBDIR))

.PHONY: all build-windows clean clean-deps rebuild help

all: ${EXTLIBDIR}/include/xmlsec1/xmlsec/xmlsec.h

${EXTLIBDIR}/include/xmlsec1/xmlsec/xmlsec.h: ${EXTLIBDIR}/include/libxml2/libxml/parser.h ${EXTLIBDIR}/include/libxslt/xslt.h ${EXTLIBDIR}/include/openssl/opensslconf.h
ifdef OS # Windows
	@if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\xmlsec.h" ( \
		$(MAKE) -f $(MAKEFILE_LIST) build-windows \
	) else ( \
		echo xmlsec already built. Use 'make rebuild' to force rebuild. \
	)
else ifeq ($(shell uname -s),Darwin) # macOS
	@if [ ! -f "${EXTLIBDIR}/include/xmlsec1/xmlsec/xmlsec.h" ]; then \
		echo "Building xmlsec for macOS..."; \
		cd ../xmlsec/xmlsec && \
		./autogen.sh --enable-shared=no --prefix="${EXTLIBDIR}" --enable-static-linking --enable-openssl --disable-nss --disable-gnutls --disable-apps --with-libxml="${EXTLIBDIR}" --with-libxslt="${EXTLIBDIR}" --with-openssl="${EXTLIBDIR}" && \
		make -j && \
		make install && \
		echo "xmlsec build completed successfully."; \
	else \
		echo "xmlsec already built. Use 'make rebuild' to force rebuild."; \
	fi
else # Linux
	@if [ ! -f "${EXTLIBDIR}/include/xmlsec1/xmlsec/xmlsec.h" ]; then \
		echo "Building xmlsec for Linux..."; \
		cd ../xmlsec/xmlsec && \
		./autogen.sh --enable-shared=no --prefix="${EXTLIBDIR}" --enable-static-linking --enable-openssl --disable-nss --disable-gnutls --disable-apps --with-libxml="${EXTLIBDIR}" --with-libxslt="${EXTLIBDIR}" --with-openssl="${EXTLIBDIR}" LDFLAGS="-L${EXTLIBDIR}/lib" CPPFLAGS="-I${EXTLIBDIR}/include" && \
		make -j && \
		make install && \
		echo "xmlsec build completed successfully."; \
	else \
		echo "xmlsec already built. Use 'make rebuild' to force rebuild."; \
	fi
endif

# Windows-specific build target
build-windows:
	@echo Building xmlsec for Windows...
	@echo Checking dependencies...
	@if not exist "$(EXTLIBDIR_WIN)\include\libxml2\libxml\parser.h" ( \
		echo ERROR: libxml2 not found. Run 'make' in ../libxml2 first. && \
		exit 1 \
	)
	@if not exist "$(EXTLIBDIR_WIN)\include\libxslt\xslt.h" ( \
		echo ERROR: libxslt not found. Run 'make' in ../libxslt first. && \
		exit 1 \
	)
	@if not exist "$(EXTLIBDIR_WIN)\include\openssl\opensslconf.h" ( \
		echo ERROR: OpenSSL not found. Run 'make' in ../OpenSSL first. && \
		exit 1 \
	)
	@echo Configuring xmlsec...
	cd ../xmlsec/xmlsec/win32
	cscript configure.js crypto=openssl,mscng,mscrypto xslt=yes unicode=yes static=yes pedantic=no cruntime=/MT prefix="$(EXTLIBDIR_WIN)" include="$(EXTLIBDIR_WIN)\include;$(EXTLIBDIR_WIN)\include\libxml2;$(EXTLIBDIR_WIN)\include\openssl" lib="$(EXTLIBDIR_WIN)\lib"
	@if errorlevel 1 ( \
		echo ERROR: xmlsec configuration failed. && \
		exit 1 \
	)
	@echo Building xmlsec libraries...
	nmake /f Makefile.msvc binaries\libxmlsec-openssl_a.lib binaries\libxmlsec-mscng_a.lib binaries\libxmlsec-mscrypto_a.lib
	@if errorlevel 1 ( \
		echo ERROR: xmlsec build failed. && \
		exit 1 \
	)
	@echo Installing xmlsec headers and libraries...
	cd ..
	if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec" mkdir "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec"
	if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\openssl" mkdir "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\openssl"
	if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscng" mkdir "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscng"
	if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscrypto" mkdir "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscrypto"
	xcopy /Y /I include\xmlsec\*.h "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec"
	xcopy /Y /I include\xmlsec\openssl\*.h "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\openssl"
	xcopy /Y /I include\xmlsec\mscng\*.h "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscng"
	xcopy /Y /I include\xmlsec\mscrypto\*.h "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\mscrypto"
	@if not exist "$(EXTLIBDIR_WIN)\lib" mkdir "$(EXTLIBDIR_WIN)\lib"
	copy /Y win32\binaries\libxmlsec_a.lib "$(EXTLIBDIR_WIN)\lib"
	copy /Y win32\binaries\libxmlsec-openssl_a.lib "$(EXTLIBDIR_WIN)\lib"
	copy /Y win32\binaries\libxmlsec-mscng_a.lib "$(EXTLIBDIR_WIN)\lib"
	copy /Y win32\binaries\libxmlsec-mscrypto_a.lib "$(EXTLIBDIR_WIN)\lib"
	@echo Creating library aliases...
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxmlsec_a.lib" "$(EXTLIBDIR_WIN)\lib\libxmlsec1.lib"
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxmlsec-openssl_a.lib" "$(EXTLIBDIR_WIN)\lib\libxmlsec1-openssl.lib"
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxmlsec-mscng_a.lib" "$(EXTLIBDIR_WIN)\lib\libxmlsec1-mscng.lib"
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxmlsec-mscrypto_a.lib" "$(EXTLIBDIR_WIN)\lib\libxmlsec1-mscrypto.lib"
	@if not exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\xmlsec.h" ( \
		echo ERROR: xmlsec.h not found after installation. && \
		exit 1 \
	)
	@echo xmlsec build and installation completed successfully.

# Dependency targets
${EXTLIBDIR}/include/libxml2/libxml/parser.h:
	@echo Building libxml2 dependency...
	cd ../libxml2 && $(MAKE)

${EXTLIBDIR}/include/libxslt/xslt.h:
	@echo Building libxslt dependency...
	cd ../libxslt && $(MAKE)

${EXTLIBDIR}/include/openssl/opensslconf.h:
	@echo Building OpenSSL dependency...
	cd ../OpenSSL && $(MAKE)

# Clean targets
clean:: clean-deps
	@echo Cleaning xmlsec...
	cd ../xmlsec/xmlsec && git clean -dfx && git reset --hard
	@echo xmlsec cleaned.

clean-deps:
	@echo Cleaning dependency libraries...
	cd ../libxml2 && $(MAKE) clean
	cd ../libxslt && $(MAKE) clean
	@echo Dependencies cleaned.

# Force rebuild
rebuild:
ifdef OS # Windows
	@echo Force rebuilding xmlsec for Windows...
	@if exist "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\xmlsec.h" del /F /Q "$(EXTLIBDIR_WIN)\include\xmlsec1\xmlsec\xmlsec.h"
	$(MAKE) -f $(MAKEFILE_LIST) build-windows
else
	@echo Force rebuilding xmlsec...
	@rm -f "${EXTLIBDIR}/include/xmlsec1/xmlsec/xmlsec.h"
	$(MAKE) -f $(MAKEFILE_LIST) all
endif

