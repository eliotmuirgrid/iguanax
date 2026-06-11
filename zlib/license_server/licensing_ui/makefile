all:

prep:
	$(MAKE) -C development

install:
ifeq (,$(DEST))
	$(error Must specify DEST for install)
endif
	rsync -v -a --delete --delete-excluded --exclude development/ --exclude makefile ./ $(DEST)
