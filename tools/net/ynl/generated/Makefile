CC=gcc
CFLAGS=-std=gnu99 -O2 -W -Wall -Wextra -Wno-unused-parameter -Wshadow -g -I../lib/

TOOL:=/home/kicinski/devel/linux/gen.py

YAML:=$(wildcard ../../../../Documentation/netlink/bindings/*.yaml)
GENS:=$(patsubst ../../../../Documentation/netlink/bindings/%.yaml,%,${YAML})
SRCS=$(patsubst %,%-user.c,${GENS})
HDRS=$(patsubst %,%-user.h,${GENS})
KSRCS=$(patsubst %,%-kernel.c,${GENS})
KHDRS=$(patsubst %,%-kernel.h,${GENS})
OBJS=$(patsubst %,%-user.o,${GENS})

all: protos.a $(HDRS) $(SRCS) $(KHDRS) $(KSRCS)

protos.a: $(OBJS)
	@echo -e "\tAR $@"
	@ar rcs $@ $(OBJS)

%-user.h: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	@echo -e "\tGEN $@"
	@$(TOOL) --mode user --user-header ynl.h --header --spec $< > $@

%-user.c: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	@echo -e "\tGEN $@"
	@$(TOOL) --mode user --user-header $(patsubst %.c,%.h,$@) ynl.h \
		--source --spec $< > $@

%-kernel.h: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	@echo -e "\tGEN $@"
	@$(TOOL) --mode kernel --header --spec $< > $@

%-kernel.c: ../../../../Documentation/netlink/bindings/%.yaml $(TOOL)
	@echo -e "\tGEN $@"
	@$(TOOL) --mode kernel --source --spec $< > $@

%-user.o: %-user.c %-user.h
	@echo -e "\tCC $@"
	@$(COMPILE.c) -c -o $@ $<

clean:
	rm -f *.o

hardclean: clean
	rm -f *.c *.h *.a

.PHONY: all clean hardclean
.DEFAULT_GOAL: all
