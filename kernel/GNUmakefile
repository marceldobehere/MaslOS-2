# RESULT YES
override OUTPUT := FINAL_kernel.o
OBJDIR = ../objects/kernel

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

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# override CFILES    = $(call rwildcard,./,*.c)
# override CPPFILES  = $(call rwildcard,./,*.cpp)
# override ASFILES   = $(call rwildcard,./,*.S)
# override NASMFILES = $(call rwildcard,./,*.asm)

override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override ASFILES   := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')

OBJ       = $(patsubst %.c,   $(OBJDIR)/%.o, $(CFILES))
OBJ      += $(patsubst %.cpp, $(OBJDIR)/%.o, $(CPPFILES))
OBJ      += $(patsubst %.S,   $(OBJDIR)/%.o, $(ASFILES))
OBJ      += $(patsubst %.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))

# override CFILES    := $(shell find . -type f -name '*.c')
# override CPPFILES  := $(shell find . -type f -name '*.cpp')
# override ASFILES   := $(shell find . -type f -name '*.S')
# override NASMFILES := $(shell find . -type f -name '*.asm')

# override OBJ       := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=_asm.o)


.PHONY: all
all: $(OUTPUT)


$(OUTPUT): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@


$(OBJDIR)/./interrupts/interrupts.o: interrupts/interrupts.cpp
	mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@ -I ../

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@
	
$(OBJDIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: %.asm
	mkdir -p $(@D)
	nasm $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
