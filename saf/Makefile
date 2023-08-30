CFLAGS = -Wall -Wextra -Wno-unused-result -Ofast

SOURCES = $(wildcard *.c)
EXECUTABLES = $(basename $(SOURCES))

.PHONY: all clean

all: $(EXECUTABLES)

$(EXECUTABLES): %: %.c
	@echo Compiling $^...
	@$(CC) $(CFLAGS) $^ -o $@

clean:
	@rm -f $(EXECUTABLES)
