OUTPUT = $(shell basename "$(shell pwd)").elf
OBJDIR = ../../objects/programs/$(shell basename "$(shell pwd)")


CC = gcc
LD = ld


LDFLAGS += -m elf_x86_64 -pic -unresolved-symbols=ignore-all --gc-sections

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif


NASMFLAGS ?= -F dwarf -g -f elf64
CFLAGS   = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../../
CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../../ -fpermissive -Wno-pmf-conversions 
# -w


CFILES    = $(shell find . -type f -name '*.c')
CPPFILES  = $(shell find . -type f -name '*.cpp')
NASMFILES = $(shell find . -type f -name '*.asm')


OBJ       = $(patsubst %.c,   $(OBJDIR)/%.o,     $(CFILES))
OBJ      += $(patsubst %.cpp, $(OBJDIR)/%.o,     $(CPPFILES))
OBJ      += $(patsubst %.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ) ../../objects/libm/FINAL_libm.o.user_module
	$(LD) $(OBJ) ../../objects/libm/FINAL_libm.o.user_module $(LDFLAGS) -o $(OBJDIR)/$@

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
	@rm -rf $(OUTPUT) $(OBJ) $(OBJ2) $(HEADER_DEPS)
