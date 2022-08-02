CC=gcc
CFLAGS=-std=gnu99 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow -g
LIBS=-lmnl

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,${SRCS})

include $(wildcard *.d)

all: ynl.a

ynl.a: $(OBJS)
	ar rcs $@ $(OBJS)
clean:
	rm -rf *.o *.d *~

%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

.PHONY: all clean
.DEFAULT_GOAL=all
