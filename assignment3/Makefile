CC = g++

INCLUDE = -I.

CFLAGS = -g

LDFLAGS = -L. -L/usr/lib

LDLIBS = -lc -lm

.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

OBJECTS = \
pgm.o \
filter.o

all: main

main: pgm.o main.o filter.o main.cpp 
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $@.o $(OBJECTS) $(LDFLAGS) $(LDLIBS)

main.o: main.cpp

clean:
	rm -f *.o
	rm -rf main