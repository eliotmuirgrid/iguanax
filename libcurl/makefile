.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)

all: ${EXTLIBDIR}/include/curl/curl.h

# libcurl also depends on zlib and openssl, but its dependency libssh2 depends on them
${EXTLIBDIR}/include/curl/curl.h: ${EXTLIBDIR}/include/libssh2.h ${EXTLIBDIR}/include/nghttp2/nghttp2.h
# TODO: add zlib for Windows
ifdef OS # Windows
	if exist ..\libcurl\curl\build rmdir /S /Q ..\libcurl\curl\build
	mkdir ..\libcurl\curl\build && \
	cd ..\libcurl\curl\build && \
	cmake .. -DCMAKE_C_FLAGS="/DNGHTTP2_STATICLIB" -DCURL_STATICLIB=ON -DCURL_STATIC_CRT=ON -DCURL_DISABLE_LDAP=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DENABLE_MANUAL=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" -DUSE_LIBIDN2=OFF -DZLIB_ROOT="${EXTLIBDIR}" -DCURL_USE_OPENSSL=ON -DOPENSSL_USE_STATIC_LIBS=ON -DOPENSSL_ROOT_DIR="${EXTLIBDIR}" -DLIBSSH2_INCLUDE_DIR="${EXTLIBDIR}/include" -DLIBSSH2_LIBRARY="${EXTLIBDIR}/lib/libssh2.lib" -DUSE_NGHTTP2=ON -DNGHTTP2_INCLUDE_DIR="${EXTLIBDIR}/include" && \
	cmake --build . --config Release --target install
else # macOS and Linux
	cd ../libcurl/curl && \
	autoreconf -fi && \
	./configure --enable-shared=no --disable-docs --disable-manual --without-libidn2 --without-libpsl --disable-ldap --without-brotli --without-zstd --without-nghttp2 --prefix="${EXTLIBDIR}" --with-zlib="${EXTLIBDIR}" --with-openssl="${EXTLIBDIR}" --with-libssh2="${EXTLIBDIR}" --with-nghttp2="${EXTLIBDIR}" && \
	make -j && \
	make install
endif

${EXTLIBDIR}/include/zlib.h:
	cd ../zlib && make

${EXTLIBDIR}/include/openssl/opensslconf.h:
	cd ../OpenSSL && make

${EXTLIBDIR}/include/libssh2.h: ${EXTLIBDIR}/include/zlib.h ${EXTLIBDIR}/include/openssl/opensslconf.h
	cd ../libssh2 && make

${EXTLIBDIR}/include/nghttp2/nghttp2.h:
	cd ../Nghttp2 && make

clean::
	cd ../libcurl/curl && git clean -dfx && git reset --hard
