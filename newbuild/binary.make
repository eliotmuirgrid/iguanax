include ../newbuild/common.make

MODULES_LINK=$(foreach lib,$(MODULES),../$(lib)/library.lib)
MODULES_CLEAN=$(foreach lib,$(MODULES),../$(lib)/*.o)

BIN := test

all: $(BIN)

../%/library.lib:	../% ../%/*.h ../%/*.cpp 
	cd $< && make


$(BIN):	$(OBJS) $(MODULES_LINK)
	$(CC)  $^ -o $@ 
 
clean:
	-rm -f $(MODULES_LINK) $(MODULES_CLEAN) $(BIN) *.d *.o

# Include generate *.d files for header dependencies
-include *.d
