include ../newbuild/common.make

LIB := library.lib

all: $(LIB)

$(LIB):	$(OBJS) 
	libtool $(LDGLAGS) $^ -o $@ $(LDLIBS)
 
clean:
	rm -rf *.o *.d library.lib

# Include generate *.d files for header dependencies
-include *.d
