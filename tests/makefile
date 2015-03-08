# generic makefile
CC=gcc
CFLAGS=-c -g
LDFLAGS=
LIBS=-lconfread
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=confreader.exe

SOURCES=main.c

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
