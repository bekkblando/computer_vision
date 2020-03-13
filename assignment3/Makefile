CC = g++

INCLUDE = -I.

CFLAGS = -g

LDFLAGS = -L. -L/usr/lib

LDLIBS = -lc -lm

STD = -std=c++11

.c.o:
	$(CC) $(STD) $(INCLUDE) $(CFLAGS) -c -o $@ $<

OBJECTS = \
pgm.o \
filter.o

MODULES = \
pgm.cpp \
filter.cpp \
main.cpp

all: main

main:
	$(CC) $(STD) $(CFLAGS) $(INCLUDE) $(MODULES) -o $@ $(LDFLAGS) $(LDLIBS)

main.o: main.cpp

clean:
	rm -f *.o
	rm -rf main