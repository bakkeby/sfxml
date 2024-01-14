# sfxml - sloppy format XML (a simple XML formatter)
# See LICENSE file for copyright and license details.

include config.mk

SRC = sfxml.c
OBJ = $(SRC:.c=.o)

all: sfxml

.c.o:
	$(CC) -c $(CFLAGS) $<

sfxml: sfxml.o
	$(CC) -o $@ sfxml.o $(LDFLAGS)

clean:
	rm -f sfxml $(OBJ) sfxml-$(VERSION).tar.gz

dist: clean
	mkdir -p sfxml-$(VERSION)
	cp LICENSE Makefile ${SRC} config.mk sfxml-$(VERSION)
	tar -cf sfxml-$(VERSION).tar sfxml-$(VERSION)
	gzip sfxml-$(VERSION).tar
	rm -rf sfxml-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sfxml $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sfxml

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/sfxml

.PHONY: all clean dist install uninstall
