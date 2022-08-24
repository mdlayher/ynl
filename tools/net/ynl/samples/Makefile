CC=gcc
CFLAGS=-std=gnu99 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow -g \
	-I../lib/ -I../generated/ -static
LDLIBS=/usr/lib64/libmnl.a ../lib/ynl.a ../generated/protos.a /usr/lib64/libmnl.a

SRCS=$(wildcard *.c)
BINS=$(patsubst %.c,%,${SRCS})

include $(wildcard *.d)

all: $(BINS)

clean:
	rm -f *.o *.d *~

hardclean: clean
	rm -f $(BINS)

.PHONY: all clean
.DEFAULT_GOAL=all
