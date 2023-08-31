override OUTPUT := $(shell basename $(shell pwd)).elf

CC = gcc
LD = ld

    
LDFLAGS += -m elf_x86_64 -pic
# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

NASMFLAGS ?= -F dwarf -g -f elf64
CFLAGS   = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../../ -g
CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../../ -g -fpermissive -Wno-pmf-conversions
#  -w


override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override NASMFILES := $(shell find . -type f -name '*.asm')

override OBJ         := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(NASMFILES:.asm=_asm.o)


.PHONY: all
all: $(OUTPUT)


$(OUTPUT): $(OBJ)
	$(LD) $(OBJ) ../../libm/FINAL_libm.o.user_module $(LDFLAGS) -o $@

%.elf: %.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@
	
%.elf: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%_asm.o: %.asm
	nasm $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ) $(OBJ2) $(HEADER_DEPS)
