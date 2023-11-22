OUTPUT = FINAL_libm.o
OBJDIR = ../objects/libm


CC = gcc
LD = ld


LDFLAGS = -m elf_x86_64 -r -unresolved-symbols=ignore-all


NASMFLAGS = -F dwarf -g -f elf64
CFLAGS    = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../
CPPFLAGS  = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -I ../
# -w -Os


CFILES    = $(shell find . -type f -name '*.c')
CPPFILES  = $(shell find . -type f -name '*.cpp')
NASMFILES = $(shell find . -type f -name '*.asm')


OBJ       = $(patsubst %.c,   $(OBJDIR)/%.o,               $(CFILES)) $(patsubst %.cpp,   $(OBJDIR)/%.o,               $(CPPFILES)) $(patsubst %.asm,   $(OBJDIR)/%_asm.o,               $(NASMFILES))
OBJ1      = $(patsubst %.c,   $(OBJDIR)/%.o.user_module,   $(CFILES)) $(patsubst %.cpp,   $(OBJDIR)/%.o.user_module,   $(CPPFILES)) $(patsubst %.asm,   $(OBJDIR)/%_asm.o.user_module,   $(NASMFILES))
OBJ2      = $(patsubst %.c,   $(OBJDIR)/%.o.kernel_src,    $(CFILES)) $(patsubst %.cpp,   $(OBJDIR)/%.o.kernel_src,    $(CPPFILES)) $(patsubst %.asm,   $(OBJDIR)/%_asm.o.kernel_src,    $(NASMFILES))
OBJ3      = $(patsubst %.c,   $(OBJDIR)/%.o.kernel_module, $(CFILES)) $(patsubst %.cpp,   $(OBJDIR)/%.o.kernel_module, $(CPPFILES)) $(patsubst %.asm,   $(OBJDIR)/%_asm.o.kernel_module, $(NASMFILES))


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(LD)  $(OBJ1) $(LDFLAGS) -o $(OBJDIR)/$@.user_module
	@$(LD) $(OBJ2) $(LDFLAGS) -o $(OBJDIR)/$@.kernel_src
	@$(LD) $(OBJ3) $(LDFLAGS) -o $(OBJDIR)/$@.kernel_module
	@echo foo > $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC)  $(CPPFLAGS) -c $^ -o $@.kernel_src -D _KERNEL_SRC
	@$(CC) $(CPPFLAGS) -c $^ -o $@.kernel_module -D _KERNEL_MODULE
	@$(CC) $(CPPFLAGS) -c $^ -o $@.user_module -D _USER_MODULE
	@echo foo > $@
	
$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC)  $(CFLAGS) -c $^ -o $@.kernel_src -D _KERNEL_SRC
	@$(CC) $(CFLAGS) -c $^ -o $@.kernel_module -D _KERNEL_MODULE
	@$(CC) $(CFLAGS) -c $^ -o $@.user_module -D _USER_MODULE
	@echo foo > $@

$(OBJDIR)/%_asm.o: %.asm
	@mkdir -p $(@D)
	@nasm $(NASMFLAGS) $< -o $@
	@echo foo > $@


.PHONY: clean
clean:
	@rm -rf $(OBJ)
	@rm -rf $(OBJ1)
	@rm -rf $(OBJ2)
	@rm -rf $(OBJ3)
	
	@rm -rf $(HEADER_DEPS)
	
	@rm -rf $(OUTPUT) || true
	@rm -rf $(OUTPUT).kernel_src
	@rm -rf $(OUTPUT).kernel_module
	@rm -rf $(OUTPUT).user_module
