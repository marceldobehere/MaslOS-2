include ../../defaults.mk
OUTPUT = $(shell basename "$(shell pwd)").elf
OBJDIR = ../../objects/modules/$(shell basename "$(shell pwd)")

LDFLAGS = $(LD_APP_FLAGS)
NASMFLAGS = $(BASE_NASM_FLAGS)
CFLAGS   = $(C_APP_FLAGS)
CPPFLAGS = $(CPP_APP_FLAGS)

CFILES    = $(shell find . -type f -name '*.c')
CPPFILES  = $(shell find . -type f -name '*.cpp')
NASMFILES = $(shell find . -type f -name '*.asm')

OBJ       = $(patsubst %.c,   $(OBJDIR)/%.o,     $(CFILES))
OBJ      += $(patsubst %.cpp, $(OBJDIR)/%.o,     $(CPPFILES))
OBJ      += $(patsubst %.asm, $(OBJDIR)/%_asm.o, $(NASMFILES))


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ) ../../objects/libm/FINAL_libm.o.kernel_module
	$(LD) $(OBJ) ../../objects/libm/FINAL_libm.o.kernel_module $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@
	
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: %.asm
	@mkdir -p $(@D)
	$(AS) $(NASMFLAGS) $< -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ) $(OBJ2) $(HEADER_DEPS)
