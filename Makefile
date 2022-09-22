# Top level makefile for romtools.

TOOLS := \
	bsplit \
	binvert \
	binclude \
	bscoop

all:
	for dir in $(TOOLS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(TOOLS); do \
		$(MAKE) -C $$dir clean; \
	done

install:
	for dir in $(TOOLS); do \
		$(MAKE) -C $$dir install; \
	done

.PHONY: all
