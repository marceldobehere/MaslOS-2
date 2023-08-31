# RESULT YES
override OUTPUT := FINAL_kernel.o

CC = gcc
LD = ld

LDFLAGS += -m elf_x86_64 -r
# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif


NASMFLAGS ?= -F dwarf -g -f elf64
CFLAGS   = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../ -g
CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../ -g -fpermissive -Wno-pmf-conversions
#  -w 


override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override ASFILES   := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')

override OBJ       := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=_asm.o)


.PHONY: all
all: $(OUTPUT)


$(OUTPUT): $(OBJ)
	@$(LD) $(OBJ) $(LDFLAGS) -o $@


interrupts/interrupts.o: interrupts/interrupts.cpp
	@$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@ -I ../

%.o: %.cpp
	@$(CC) $(CPPFLAGS) -c $^ -o $@
	
%.o: %.c
	@$(CC) $(CFLAGS) -c $^ -o $@

%_asm.o: %.asm
	@nasm $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
