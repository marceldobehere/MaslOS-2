override DIRECTORIES = $(wildcard */)

.PHONY: all clean $(DIRECTORIES)

.SUFFIXES:

all: $(DIRECTORIES)

$(DIRECTORIES):
	$(MAKE) -C $@

clean:
	@$(foreach dir,$(DIRECTORIES),$(MAKE) --silent -C $(dir) clean;)
