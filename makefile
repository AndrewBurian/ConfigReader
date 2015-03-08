
all: libconfread.a

install: libconfread.a confread.h
	cp -f libconfread.a /usr/local/lib/
	cp -f confread.h /usr/local/include/

libconfread.a: confread.o
	ar rcs libconfread.a confread.o

confread.o: confread.c confread.h
	gcc -c confread.c

uninstall:
	rm -f /usr/local/lib/libconfread.a
	rm -f /usr/local/include/confread.h

clean:
	rm -f *.o
	rm -f *.a
