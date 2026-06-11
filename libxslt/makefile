.NOTPARALLEL:

EXTLIBDIR := $(abspath ../.git/extLibs/)
EXTLIBDIR_WIN := $(subst /,\,$(EXTLIBDIR))

all: ${EXTLIBDIR}/include/libxslt/xslt.h

${EXTLIBDIR}/include/libxslt/xslt.h: ${EXTLIBDIR}/include/libxml2/libxml/parser.h
ifdef OS # Windows
	if exist ..\libxslt\libxslt\build rmdir /S /Q ..\libxslt\libxslt\build
	mkdir ..\libxslt\libxslt\build && \
	cd ..\libxslt\libxslt\build && \
	cmake .. -G "NMake Makefiles" -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS_RELEASE="/MT" -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded" -DLIBXSLT_WITH_PYTHON=OFF -DLIBXSLT_WITH_PROGRAMS=OFF -DLIBXSLT_WITH_TESTS=OFF -DLibXml2_ROOT="${EXTLIBDIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${EXTLIBDIR}" && \
	nmake && \
	nmake install && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxslts.lib" "$(EXTLIBDIR_WIN)\lib\libxslt.lib" && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libexslts.lib" "$(EXTLIBDIR_WIN)\lib\libexslt.lib" && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libxslts.lib" "$(EXTLIBDIR_WIN)\lib\libxslt_a.lib" && \
	copy /Y "$(EXTLIBDIR_WIN)\lib\libexslts.lib" "$(EXTLIBDIR_WIN)\lib\libexslt_a.lib"
else # POSIX
	cd ../libxslt/libxslt && \
	./autogen.sh --enable-shared=no --prefix="${EXTLIBDIR}" --without-python --with-libxml-prefix="${EXTLIBDIR}" && \
	make -j && \
	make install
endif

${EXTLIBDIR}/include/libxml2/libxml/parser.h:
	cd ../libxml2 && make

clean::
	cd ../libxslt/libxslt && git clean -dfx && git reset --hard

