.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)

all: ${EXTLIBDIR}/include/zlib.h

${EXTLIBDIR}/include/zlib.h:
ifdef OS # Windows
	if exist ..\zlib\zlib\build rmdir /S /Q ..\zlib\zlib\build
	mkdir ..\zlib\zlib\build && \
	cd ..\zlib\zlib\build && \
	cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" && \
	cmake --build . --config Release --target install
else # POSIX
	cd ../zlib/zlib && \
	./configure --static --prefix="${EXTLIBDIR}" && \
	make -j && \
	make install
endif

clean::
	cd ../zlib/zlib && git clean -dfx && git reset --hard
