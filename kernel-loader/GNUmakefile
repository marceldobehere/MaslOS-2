OUTPUT = kernel.elf
OBJDIR = ../objects/kernel-loader


CC = gcc
LD = ld

    
LDFLAGS =                                   \
    -unresolved-symbols=ignore-all          \
    -nostdlib                               \
    -static                                 \
    -Bsymbolic                              \
    -m elf_x86_64                           \
    -z max-page-size=0x1000                 \
    --gc-sections                                \
    -T linker.ld

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif


CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -I ../
# -w


CPPFILES = $(shell find . -type f -name '*.cpp')


OBJ      = $(patsubst %.cpp, $(OBJDIR)/%.o, $(CPPFILES))


.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OBJ) ../objects/kernel/FINAL_kernel.o ../objects/libm/FINAL_libm.o.kernel_src
	$(LD) $(OBJ) ../objects/kernel/FINAL_kernel.o ../objects/libm/FINAL_libm.o.kernel_src $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@ -D _KERNEL_SRC


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
