###############################################################################
# Make/build configuration and settings
# Included by platform makefiles and performs different builds based
# on the settings in this file
# NOTE : Conditionals are performed by checking for equal to 1
#        To enable an option, set it to 1, otherwise, set it to anything else
###############################################################################

# Disables warnings and warnings-as-errors - useful for third-party code
IFW_MAKE_THIRD_PARTY ?= 0

# Uses clang on linux systems
IFW_MAKE_USE_CLANG ?= 0

# Builds a debug version (includes debugging symbols/support)
IFW_MAKE_DEBUG ?= 1

# Strips binaries after building
IFW_MAKE_STRIP_BINARY ?= 0

# Disables caching
IFW_MAKE_NO_CCACHE ?= 0

# Disables optimization
IFW_MAKE_NO_OPTIMIZE ?= 0

# Builds a shared library instead of a static one
IFW_MAKE_SHARED_LIBRARY ?= 0

# Sets up some special link flags that helps Lua load up this shared
# library as an extension
IFW_MAKE_LUA_EXTENSION ?= 0

# Disables warnings altogether - useful for third-party code
IFW_MAKE_NO_WARNINGS ?= 0

# Disables warnings-as-errors (a warning will not cause the build to abort)
IFW_MAKE_DISABLE_WARNINGS_AS_ERRORS ?= 0

# Allows automatically linking required system libraries on Windows
IFW_MAKE_ALLOW_DEFAULT_LIB ?= 0

# Enables use of the .exports_list file when linking shared libraries or
# binaries in Posix (this defines exported symbols in the result)
#    EXPORTSLIST can be defined to customize the name of the exports list file
#       e.g. EXPORTSLIST = custom.exports_list
IFW_MAKE_USE_EXPORTS_LIST ?= 0

###############################################################################
# Settings for building - these are provided here with default settings
# and can be overriden by the environment
# e.g. $ export IFW_VERBOSE=1
#      Will enable verbose mode

# Enables verbose mode
# Prints the 'make' status line for modules
# Prints commands that get executed
IFW_VERBOSE ?= 0

# Flags used when compiling C/C++ source files to generate/disable warnings
# NOTE : IFW_MAKE_NO_WARNINGS and IFW_MAKE_DISABLE_WARNINGS_AS_ERRORS take
#        precedence over these flags

# The warn flags should be used to enable warnings (e.g. -Wall)
IFW_WARN_FLAGS ?=

# The no-warn flags should be used to disable warnings (e.g. -w)
# It can be used with -Wall to disable certain warnings, like -Wno-reorder
IFW_NOWARN_FLAGS ?= $(IFW_DISABLE_ALL_WARNINGS)

# Custom flags to use for compiler optimizations
# The default on Posix is "-O2"
# The default on Windows is "/Ox /GF"
IFW_OPTIMIZE_FLAGS ?=

#Add compiler flags
IFW_CFLAGS ?=