.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)
EXTLIBDIR_WIN := $(subst /,\,$(EXTLIBDIR))

all: ${EXTLIBDIR}/include/libxml2/libxml/parser.h

${EXTLIBDIR}/include/libxml2/libxml/parser.h:
ifdef OS # Windows
	if exist ..\libxml2\libxml2\build rmdir /S /Q ..\libxml2\libxml2\build
	mkdir ..\libxml2\libxml2\build && \
	cd ..\libxml2\libxml2\build && \
	cmake .. -G "NMake Makefiles" -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded" -DLIBXML2_WITH_PYTHON=OFF -DLIBXML2_WITH_PROGRAMS=OFF -DLIBXML2_WITH_ICONV=OFF -DLIBXML2_WITH_TESTS=OFF -DLIBXML2_WITH_ZLIB=OFF -DZLIB_ROOT="${EXTLIBDIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" && \
	nmake && \
	nmake install && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxml2s.lib" "$(EXTLIBDIR_WIN)\lib\libxml2.lib" && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxml2s.lib" "$(EXTLIBDIR_WIN)\lib\libxml2_a.lib"
else # POSIX
	cd ../libxml2/libxml2 && \
	./autogen.sh --enable-shared=no --prefix="${EXTLIBDIR}" --without-python --without-icu && \
	make -j && \
	make install
endif

clean::
	cd ../libxml2/libxml2 && git clean -dfx && git reset --hard

