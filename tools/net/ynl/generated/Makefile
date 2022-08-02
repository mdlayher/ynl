CC=gcc
CFLAGS=-std=gnu99 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow -g

TOOL:=/home/kicinski/devel/linux/gen.py

YAML:=$(wildcard ../../../../Documentation/netlink/bindings/*.yaml)
GENS:=$(patsubst ../../../../Documentation/netlink/bindings/%.yaml,%,${YAML})
SRCS=$(patsubst %,%-user.c,${GENS})
HDRS=$(patsubst %,%-user.h,${GENS})
OBJS=$(patsubst %,%-user.o,${GENS})

all: protos.a

protos.a: $(OBJS)
	ar rcs $@ $(OBJS)

%-user.c: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	$(TOOL) --mode user --user-header $(patsubst %.c,%.h,$@) ynl.h \
		--source --spec $< > $@

%-user.h: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	$(TOOL) --mode user --user-header ynl.h --header --spec $< > $@

%-user.o: %-user.c %-user.h
	$(COMPILE.c) -I../lib/ -c -o $@ $<

clean:
	rm *.o

hardclean: clean
	rm *.c *.h

.PHONY: all clean hardclean
.DEFAULT_GOAL=all
