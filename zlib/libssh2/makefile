.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)

all: ${EXTLIBDIR}/include/libssh2.h

${EXTLIBDIR}/include/libssh2.h: ${EXTLIBDIR}/include/zlib.h ${EXTLIBDIR}/include/openssl/opensslconf.h
# TODO: add zlib for Windows
ifdef OS # Windows
	if exist ..\libssh2\libssh2\build rmdir /S /Q ..\libssh2\libssh2\build
	mkdir ..\libssh2\libssh2\build && \
	cd ..\libssh2\libssh2\build && \
	cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DZLIB_ROOT="${EXTLIBDIR}" -DCRYPTO_BACKEND=OpenSSL -DOPENSSL_USE_STATIC_LIBS=ON -DOPENSSL_ROOT_DIR="${EXTLIBDIR}" && \
	cmake --build . --config Release --target install
else # POSIX, LIBS variable is needed for Linux
	cd ../libssh2/libssh2 && \
	autoreconf -fi && \
	./configure --enable-shared=no --enable-examples-build=no --prefix="${EXTLIBDIR}" --with-libz-prefix="${EXTLIBDIR}" --with-libssl-prefix="${EXTLIBDIR}" LIBS="-lpthread -ldl" && \
	make -j && \
	make install
endif

${EXTLIBDIR}/include/zlib.h:
	cd ../zlib && make

${EXTLIBDIR}/include/openssl/opensslconf.h:
	cd ../OpenSSL && make

clean::
	cd ../libssh2/libssh2 && git clean -dfx && git reset --hard
