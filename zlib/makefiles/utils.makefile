###############################################################################
# Utility/path definitions
# Included by makefiles that need these constants at the very top
# This file holds constants like paths, and libraries, as well as some handy
# functions and recipes for other makefiles to use
###############################################################################

# Default target - no definition
all: ;

# Target to force rebuilding
.PHONY: .FORCE
.FORCE: ;
MAKEFILEDIR := $(dir $(lastword $(MAKEFILE_LIST)))

###############################################################################
# Some flags implicitly activate other flags
# This section defines those rules

# Allow third party settings to override environment variables
ifeq ($(IFW_MAKE_THIRD_PARTY), 1)
   override IFW_MAKE_NO_WARNINGS = 1
   override IFW_MAKE_DISABLE_WARNINGS_AS_ERRORS = 1
endif

# Lua extensions are shared libraries
ifeq ($(IFW_MAKE_LUA_EXTENSION), 1)
   IFW_MAKE_SHARED_LIBRARY = 1
endif

###############################################################################
# Utilities for flags/command generation
JOINSTRINGS ?= $(strip $(1))$(strip $(2))

# Helper - don't use this directly
APPLYFLAGTOARGS = $(foreach arg, $(1), $(call JOINSTRINGS, $(2), $(arg)))
ifeq ($(IFW_POSIX), 1)
   # POSIX

   INCLUDEFLAG = -I
   DEFINEFLAG = -D
   LINKFLAG = -L
   LIBFLAG = -l
   MAKEDYNAMICLINKFLAG = $(call APPLYFLAGTOARGS, $(1), -l)

   IFW_ENABLE_ALL_WARNINGS = -Wall -Wextra
   IFW_DISABLE_ALL_WARNINGS = -w
   IFW_TREAT_WARNINGS_AS_ERRORS = -Werror
else
   # WINDOWS

   INCLUDEFLAG = -I
   DEFINEFLAG = -D
   LINKFLAG = -L
   LIBFLAG = -l

   IFW_ENABLE_ALL_WARNINGS = -Wall -Wextra
   IFW_DISABLE_ALL_WARNINGS = -w
   IFW_TREAT_WARNINGS_AS_ERRORS = -Werror

   EXE = .exe
endif

MAKEINCLUDEFLAG = $(call APPLYFLAGTOARGS, $(1), $(INCLUDEFLAG))
MAKEDEFINEFLAG = $(call APPLYFLAGTOARGS, $(1), $(DEFINEFLAG))
MAKELINKFLAG = $(call APPLYFLAGTOARGS, $(1), $(LINKFLAG))
MAKELIBFLAG = $(call APPLYFLAGTOARGS, $(1), $(LIBFLAG))

###############################################################################
# Utilities for makefile-writing

#TODO Looks like all the checks for whether the default variable is used are always true so this check might be useless?
ISMAKEDEFAULT = $(strip $(if $(ORIGINDEFAULT), 1))
# Helpers for the above - probably not useful to call directly:
STRIP1 = $(strip $(1))
ORIGINDEFAULT = $(findstring default, $(origin $(STRIP1)))

#TODO very involved recipe generator, used in common.makefile, unclear if we really need it
# Helper that generates recipes for each item in two lists
# This can be thought of as applying a recipe to each pair in the zipped list
# e.g. map(a b, c d, FUNC) =>
#      a: c
#      	$(call FUNC, c, a)
#      b: d
#      	$(call FUNC, d, b)
# Useful for defining rules based on lists generated via 'patsubst'/'addprefix'
# usage: $(call MAKEMAPPEDRECIPES, list1..., list2..., CMD_VARIABLE)
#        CMD_VARIABLE is the name of a function to apply to each pair
#           It is called with the current item in list1 as $(1), and the
#           current item in list2 as $(2)
#           e.g. CPVERBOSE = cp -fp "$(strip $(1))" "$(strip $(2))"
#                $(call MAKEMAPPEDRECIPES, 1.txt 2.txt, a.txt b.txt, CPVERBOSE)
#             =>    a.txt: 1.txt
#                   	$(call CPVERBOSE, 1.txt, a.txt)
#                   b.txt: 2.txt
#                   	$(call CPVERBOSE, 2.txt, b.txt)
#        Additional parameters may be supplied after CMD_VARIABLE - these will
#        be passed as $(3), $(4), and $(5) to each call of CMD_VARIABLE
#           e.g. TARVERBOSE = tar $(3) $(1) && touch $(2)
#                $(call MAKEMAPPEDRECIPES, a.tar.gz b.tar.gz, a_ok b_ok, \
#                                          TARVERBOSE, zxf)
#                $(call MAKEMAPPEDRECIPES, dir1/ dir2/, d1.tar.gz d2.tar.gz, \
#                                          TARVERBOSE, zcf)
#             =>    a_ok: a.tar.gz
#                   	$(call TARVERBOSE, a.tar.gz, a_ok, zxf)
#                   b_ok: b.tar.gz
#                    $(call TARVERBOSE, b.tar.gz, b_ok, zxf)
#                   d1.tar.gz: dir1/
#                    $(call TARVERBOSE, d1.tar.gz, dir1/, zcf)
#                   d2.tar.gz: dir2/
#                    $(call TARVERBOSE, d2.tar.gz, dir2/, zcf)
#           Additional parameters may include references to automatic variables
#           (e.g. "$@" or "$<") by escaping them (e.g. "$$@" or "$$<")
#              e.g. $(call MAKEMAPPEDRECIPES, a b, c d, CP, "$$< -> $$@")
#           ...but be careful when using parameters (e.g. "$$(2)") - this may
#           not work as expected
# NOTE : This is implemented using tail-end recursion - that's currently the
#        only way to go about zipping lists - so it looks rather ugly...
# NOTE : To debug/view the generated recipes, replace 'eval' with 'info' in the
#        definition of MAKEMAPPEDRECIPES
_RECURSIVE_HEAD = $(firstword $(1))
_RECURSIVE_TAIL = $(wordlist 2, $(words $(1)), $(1))

_RH1 = $(strip $(call _RECURSIVE_HEAD, $(1)))
_RT1 = $(strip $(call _RECURSIVE_TAIL, $(1)))

_RH2 = $(strip $(call _RECURSIVE_HEAD, $(2)))
_RT2 = $(strip $(call _RECURSIVE_TAIL, $(2)))

# usage: $(call GENRECIPE, src, target[, command[, extraparams...]])
# run $(eval $(call GENRECIPE, ...)) to load/activate the recipe
define GENRECIPE
$(strip $(2)): $(strip $(1))
$(if $(strip $(3)),	$(strip $(3)))
endef
ISNONEMPTYLIST_12 = $(strip $(1) $(2))
COMMA := ,
ADDCMDPARAMS_456 = $(if $(strip $(4)), $(COMMA) $(4)$(strip \
                     $(if $(strip $(5)), $(COMMA) $(5)$(strip \
                       $(if $(strip $(6)), $(COMMA) $(6))))))
EXPANDRECIPE_RH_12 = $$(call $(strip $(3)), $(_RH1), $(_RH2)$(strip \
                                            $(ADDCMDPARAMS_456)))
MAKEMAPPEDRECIPES = \
   $(if $(ISNONEMPTYLIST_12), \
        $(eval $(call GENRECIPE, $(_RH1), $(_RH2), $(EXPANDRECIPE_RH_12))) \
        $(call MAKEMAPPEDRECIPES, $(_RT1), $(_RT2), \
                                  $(3), $(4), $(5), $(6)) \
   )

# Variant of the above - creates order-only pre-requisites
MAKEMAPPEDRECIPES_ORDERONLY = \
   $(if $(ISNONEMPTYLIST_12), \
        $(eval $(call GENRECIPE, | $(_RH1), $(_RH2), $(EXPANDRECIPE_RH_12))) \
        $(call MAKEMAPPEDRECIPES_ORDERONLY, $(_RT1), $(_RT2), \
                                            $(3), $(4), $(5), $(6)) \
   )

###############################################################################
# Paths for each platform

IFW_LUA_MODULE = LUAC
IFW_LUA_INCLUDE = $(call MAKEINCLUDEFLAG, ../$(IFW_LUA_MODULE))
IFW_LUA_EXPORTS_LIST = ../$(IFW_LUA_MODULE)/lua.exports_list

IFW_PYTHON_INCLUDE = $(call MAKEINCLUDEFLAG, ../PC ../PythonInclude)

ifeq ($(IFW_POSIX), 1)
   # POSIX

   IFW_DL_LINK = $(call MAKEDYNAMICLINKFLAG, dl)
   IFW_PTHREAD_LINK = $(call MAKEDYNAMICLINKFLAG, pthread) 

   ifeq ($(IFW_MAC), 1)
      # Linker option used for Lua modules
      IFW_CPP_EXPORT_DYNAMIC_FLAG = -Wl,-export_dynamic

      IFW_LINK_RPATH = -Wl,-rpath,@executable_path/.
      MAKESHLIBEXPORTS = -exported_symbols_list $(1)
   else

      # Linker option used for Lua modules
      IFW_CPP_EXPORT_DYNAMIC_FLAG = -Wl,-E

      IFW_LINK_RPATH = -Wl,-rpath=\$$ORIGIN
      IFW_RT_LINK = -lrt
      IFW_UTIL_LINK = -lutil
      MAKESHLIBEXPORTS = -Wl,--version-script="$(strip $(1))"
   endif

   # Helpful utility that can be added to LDEXTRA for unit-tests
   # Contains links to any library the unit-test may need
   IFW_UNITTEST_LDEXTRA = $(IFW_IDN_LINK) $(IFW_RT_LINK) $(IFW_LINK_RPATH)
   # Linux already contains a -lpthread and -ldl, so only add it for Mac  
   ifeq ($(IFW_MAC), 1)
      IFW_UNITTEST_LDEXTRA += $(IFW_PTHREAD_LINK) $(IFW_DL_LINK)   
   endif
endif

###############################################################################
# Rules for generating the exports file on Mac and Linux
# This is used for shared-library symbols
CATREMOVECOMMENTS = cat $(1) | grep -v -e '^\#'
ifeq ($(IFW_MAC), 1)
   # On Mac, just ensure each symbol has an underscore prefix (C-style call)
   MAKEEXPORTSVERBOSE = $(CATREMOVECOMMENTS) | \
                        sed 's/^./_&/' > $(2)
endif
ifeq ($(IFW_LINUX), 1)
   # On Linux, build up a specification of global symbols
   # These just need to appear in the 'global' section with the correct number
   # of spaces and a semi-colon at the end
   MAKEEXPORTSVERBOSE = \
      echo "{" > $(2) ; \
      echo "  global:" >> $(2) ; \
      $(CATREMOVECOMMENTS) | \
         sed 's/^[^ ].*$$/    &;/' >> $(2) ; \
      echo "  local: *;" >> $(2) ; \
      echo "};" >> $(2)
endif

ifneq ($(IFW_VERBOSE), 1)
   MAKEEXPORTS = $(call PRINTCMD, EXP, $(2)) && $(MAKEEXPORTSVERBOSE)
else
   MAKEEXPORTS = $(MAKEEXPORTSVERBOSE)
endif

ifeq ($(IFW_POSIX), 1)
# Generic pattern for generating exports from exports_list
%.exports: %.exports_list
	$(call MAKEEXPORTS, $<, $@)
endif

###############################################################################
# Utility for keeping Lua library symbols in a stripped library/binary
ifeq ($(IFW_MAC), 1)
   # -s file.exports_list
   #    this preserves symbols named in the specified file
   # -i
   #    this ignores warnings/errors when symbols in -s are not present in
   #    the binary/object being stripped
   # -x
   #    remove all local symbols
   # -S
   #    remove all debug symbols
   IFW_LUA_STRIP_FLAGS = -s $(IFW_LUA_EXPORTS_LIST) -i -x -S
endif
ifeq ($(IFW_LINUX), 1)
   # -x
   #    remove all non-global symbols
   # -S
   #    remove all debug symbols
   IFW_LUA_STRIP_FLAGS = -x -S
endif

###############################################################################
#TODO almost certain this isn't really necessary
# Helpers that disable warnings, or disables the warnings-as-errors behaviour 
ifeq ($(IFW_MAKE_NO_WARNINGS), 1)
   override IFW_WARN_FLAGS :=
   override IFW_NOWARN_FLAGS := $(IFW_DISABLE_ALL_WARNINGS)
endif

ifeq ($(IFW_MAKE_DISABLE_WARNINGS_AS_ERRORS), 1)
   override IFW_WARN_FLAGS := \
      $(filter-out $(IFW_TREAT_WARNINGS_AS_ERRORS), $(IFW_WARN_FLAGS))
   override IFW_NOWARN_FLAGS := \
      $(filter-out $(IFW_TREAT_WARNINGS_AS_ERRORS), $(IFW_NOWARN_FLAGS))
endif

###############################################################################
# Utility that expands to the first defined variable in either the binary,
# library, or module name, or empty if none are defined
# This is useful for adding prerequisites onto whatever is being built
IFW_TARGET_NAME = $(strip $(if $(strip $(BINARY)), $(BINARY), $(IFLIBELSEMOD)))
IFLIBELSEMOD = $(if $(strip $(LIBRARY)), $(LIBRARY), $(MODULE))

###############################################################################
# Utilities for copying and cleaning common libraries/binaries
# Note that for any of these copy/clean operations, the DEST variable can be
# overridden to place them in a different spot
# TODO - get rid of global helpers in make system - Eliot

ifneq ($(IFW_LUA_EXPORTS_LIST),)
$(IFW_LUA_EXPORTS_LIST): | $(IFW_LUA_MODULE)
endif

.PHONY: $(IFW_LUA_MODULE)
