.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)

all: ${EXTLIBDIR}/include/openssl/opensslconf.h
VCWIN := VC-WIN64A

ifeq ($(PROCESSOR_ARCHITEW6432),ARM64)
	VCWIN := VC-WIN64-ARM
endif

# Why do I use build.bat? (Eliot)
# The problem is that NMAKE (microsoft's poor implementation of make) uses the same environmental variables as GNU make
# When we do make -j in the Iguana directory this sets the environmental variable MAKEFLAGS which would then stop NMAKE
# from working.  I tried using SET and various things to clear the MAKEFLAGS - nothing seemed to work until I ran open OpenSSL
# nmake command from build.bat - that seemed to solve the problem.

${EXTLIBDIR}/include/openssl/opensslconf.h:
ifdef OS # Windows
	cd ..\OpenSSL\openssl && \
	perl Configure no-shared --prefix="${EXTLIBDIR}" ${VCWIN} && \
	..\build.bat
else ifeq ($(shell uname -s),Darwin) # macOS
	cd ../OpenSSL/openssl && \
	./Configure no-shared --prefix="${EXTLIBDIR}" darwin64-$(strip $(shell uname -m))-cc && \
	make LIBS="libcrypto.a libssl.a" build_libs -j && \
	make install_dev
else # Linux
	cd ../OpenSSL/openssl && \
	./Configure no-shared --prefix="${EXTLIBDIR}" linux-generic64 && \
	make LIBS="libcrypto.a libssl.a" build_libs -j && \
	make install_dev
endif

clean::
	cd ../OpenSSL/openssl && git clean -dfx && git reset --hard
