OUTPUT = FINAL_kernel.o
OBJDIR = ../objects/kernel


CC = gcc
LD = ld


LDFLAGS = -m elf_x86_64 -r -unresolved-symbols=ignore-all

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif


NASMFLAGS = -F dwarf -g -f elf64
CFLAGS    = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../ 
CPPFLAGS  = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../ -fpermissive -Wno-pmf-conversions
# -w 


CFILES    = $(shell find . -type f -name '*.c')
CPPFILES  = $(shell find . -type f -name '*.cpp')
ASFILES   = $(shell find . -type f -name '*.S')
NASMFILES = $(shell find . -type f -name '*.asm')


OBJ       = $(patsubst %.c,   $(OBJDIR)/%.o,     $(CFILES))
OBJ      += $(patsubst %.cpp, $(OBJDIR)/%.o,     $(CPPFILES))
OBJ      += $(patsubst %.S,   $(OBJDIR)/%.o,     $(ASFILES))
OBJ      += $(patsubst %.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/./interrupts/interrupts.o: interrupts/interrupts.cpp
	@mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@ -I ../

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@
	
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: %.asm
	@mkdir -p $(@D)
	nasm $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
