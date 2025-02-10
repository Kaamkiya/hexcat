# SPDX-License-Identifier: MIT

.POSIX:

VERSION = 0.1

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man
DOCPREFIX = $(PREFIX)/share/doc

SRC = hexcat.c
OBJ = $(SRC:.c=.o)

HEXCAT_CFLAGS = $(CFLAGS) -Wall -Werror -Wpedantic -std=c99
HEXCAT_LDFLAGS = $(LDFLAGS)

all: hexcat

.c.o:
	$(CC) -c $< $(HEXCAT_CFLAGS) $(HEXCAT_CPPFLAGS)

hexcat: $(OBJ) $(LIBS)
	$(CC) -o $@ $(OBJ) $(LIBS) $(HEXCAT_LDFLAGS)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	mkdir -p $(DESTDIR)$(DOCPREFIX)/hexcat
	install -m 644 README.md LICENSE $(DESTDIR)$(DOCPREFIX)/hexcat
	install -m 775 hexcat $(DESTDIR)$(PREFIX)/bin
	sed "s/VERSION/$(VERSION)/g" < hexcat.1 > $(DESTDIR)$(MANPREFIX)/man1/hexcat.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/hexcat.1

uninstall: all
	rm -f $(DESTDIR)$(MANPREFIX)/man1/hexcat.1 $(DESTDIR)$(PREFIX)/bin/hexcat
	rm -rf $(DESTDIR)$(DOCPREFIX)/hexcat

dist: clean
	mkdir -p hexcat-$(VERSION)
	cp -R Makefile README.md LICENSE hexcat.c hexcat.1 hexcat-$(VERSION)
	tar -cf - hexcat-$(VERSION) | gzip -c > hexcat-$(VERSION).tar.gz
	rm -rf hexcat-$(VERSION)

clean:
	rm -f hexcat *.o
