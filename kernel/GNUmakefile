include ../defaults.mk
OUTPUT = FINAL_kernel.o
OBJDIR = ../objects/kernel

LDFLAGS = $(LD_RELO_FLAGS)
NASMFLAGS = $(BASE_NASM_FLAGS)
CFLAGS    = $(C_RELO_FLAGS)
CPPFLAGS  = $(CPP_RELO_FLAGS)

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
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@ -I ../ -D _KERNEL_SRC

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@ -D _KERNEL_SRC
	
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@ -D _KERNEL_SRC

$(OBJDIR)/%_asm.o: %.asm
	@mkdir -p $(@D)
	$(AS) $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
