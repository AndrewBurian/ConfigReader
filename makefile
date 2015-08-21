MAJOR_VERSION=1
MINOR_VERSION=4

all: libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION)

install: libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) confread.h
	install -D libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) $(DESTDIR)/usr/lib/libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION)
	install -D confread.h $(DESTDIR)/usr/include/confread.h
	# Sym-links are created and installed in 2 steps to avoid packaging craziness
	ln -s libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) libconfread.so.$(MAJOR_VERSION)
	ln -s libconfread.so.$(MAJOR_VERSION) libconfread.so
	cp -a libconfread.so.$(MAJOR_VERSION) $(DESTDIR)/usr/lib/libconfread.so.$(MAJOR_VERSION)
	cp -a libconfread.so $(DESTDIR)/usr/lib/libconfread.so

libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION): confread.o
	gcc -shared -fPIC -Wl,-soname,libconfread.so.$(MAJOR_VERSION) -o libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) confread.o -lc

confread.o: confread.c confread.h
	gcc -fPIC -c -Wall confread.c

uninstall:
	rm -f $(DESTDIR)/usr/lib/libconfread.so*
	rm -f $(DESTDIR)/usr/include/confread.h

clean:
	rm -f *.o
	rm -f *.so*
