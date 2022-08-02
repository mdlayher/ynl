SUBDIRS = lib generated samples

all: $(SUBDIRS)

$(SUBDIRS):
	@if [ -f "$@/Makefile" ] ; then \
		$(MAKE) -C $@ ; \
	fi

clean:
	@for dir in $(SUBDIRS) ; do \
		if [ -f "$$dir/Makefile" ] ; then \
			$(MAKE) -C $$dir $@; \
		fi \
	done

.PHONY: clean all $(SUBDIRS)
