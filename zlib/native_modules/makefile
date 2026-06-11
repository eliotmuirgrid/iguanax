# Space separated list of module dirs we want to build.
# There should be a matching "makefile.ModuleName" in this directory.
MODULES=HelloWorld

all:
	$(foreach module,$(MODULES),$(MAKE) -f makefile.$(module);)

clean::
	$(foreach module,$(MODULES),$(MAKE) -f makefile.$(module) clean;)

info:
	$(foreach module,$(MODULES),$(MAKE) -f makefile.$(module) info;)
