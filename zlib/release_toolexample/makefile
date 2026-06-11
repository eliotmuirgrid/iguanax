include ../make/makefile.core
include ../libcurl/makefile.libcurl

$(TARGET): ../release_tool/release_tool

../release_tool/release_tool:
	cd ../release_tool && make

clean:: 
	cd ../release_tool && make clean