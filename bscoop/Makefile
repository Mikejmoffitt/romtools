APPNAME := bscoop

CC := gcc
INSTALL_PREFIX := /usr/bin
ifdef SYSTEMROOT
	APPEXT := .exe
endif

EXECNAME := $(APPNAME)$(APPEXT)

.PHONY: clean

all: $(EXECNAME)

$(EXECNAME): $(APPNAME).c
	$(CC) $< -O3 -Wall -o $@

install: $(EXECNAME)
	cp $< $(INSTALL_PREFIX)/

clean:
	rm -f $(EXECNAME)
