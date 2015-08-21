MAJOR_VERSION=1
MINOR_VERSION=0

all: libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION)

install: libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) confread.h
	install -D libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) $(DSTDIR)/usr/lib/libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION)
	install -D confread.h $(DSTDIR)/usr/include/confread.h
	ldconfig
	ln -fs $(DSTDIR)/usr/lib/libconfread.so.$(MAJOR_VERSION) $(DSTDIR)/usr/lib/libconfread.so

libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION): confread.o
	gcc -shared -fPIC -Wl,-soname,libconfread.so.$(MAJOR_VERSION) -o libconfread.so.$(MAJOR_VERSION).$(MINOR_VERSION) confread.o -lc

confread.o: confread.c confread.h
	gcc -fPIC -c -Wall confread.c

uninstall:
	rm -f $(DSTDIR)/usr/lib/libconfread.so*
	rm -f $(DSTDIR)/usr/include/confread.h

clean:
	rm -f *.o
	rm -f *.so*
