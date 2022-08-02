CC=gcc
CFLAGS=-std=gnu99 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow -g \
	-I../lib/ -I../generated/ -fsanitize=address -static-libasan
LDLIBS=-lmnl ../lib/ynl.a ../generated/protos.a

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
