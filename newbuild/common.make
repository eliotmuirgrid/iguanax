# Common make definitions

# for debug
#CC      := ccache llvm-g++ -std=c++14 -MMD -g -fno-omit-frame-pointer -fsanitize=address
CC      := ccache llvm-g++ -std=c++14 -MMD -g 
SRCS    := $(wildcard *.cpp)
OBJS    := $(patsubst %.cpp,%.o,$(SRCS))
CFLAGS  := -I../


%.o:	%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:	all clean

