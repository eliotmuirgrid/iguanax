.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)

all: ${EXTLIBDIR}/include/nghttp2/nghttp2.h

${EXTLIBDIR}/include/nghttp2/nghttp2.h:
ifdef OS # Windows
	if exist ..\Nghttp2\nghttp2\build rmdir /S /Q ..\Nghttp2\nghttp2\build
	mkdir ..\Nghttp2\nghttp2\build && \
	cd ..\Nghttp2\nghttp2\build && \
	cmake .. -DENABLE_FAILMALLOC=OFF -DENABLE_LIB_ONLY=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON -DENABLE_STATIC_CRT=OFF -DENABLE_DOC=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" && \
	cmake --build . --config Release --target install
else # POSIX, make sure autoconf automake libtool pkg-config are installed
	cd ../Nghttp2/nghttp2 && \
	autoreconf -i && \
	./configure --enable-shared=no --prefix="${EXTLIBDIR}" --enable-lib-only && \
	make -j && \
	make install
endif

clean::
	cd ../Nghttp2/nghttp2 && git clean -dfx && git reset --hard
