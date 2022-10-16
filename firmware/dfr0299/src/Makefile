CC=gcc
HEADERS=rc_dfr0299.h
OBJECTS=rc_dfr0299.o
CFLAGS=-W -Wall -ansi

dfr0299: main.c $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS) $<

clean:
	rm -f dfr0299
