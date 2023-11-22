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
	dd if=/dev/zero of="../objects/kernel/FINAL_kernel.o" bs=1 seek=16 conv=notrunc count=2
	dd if=/dev/zero of="../objects/libm/FINAL_libm.o.kernel_src" bs=1 seek=16 conv=notrunc count=2
	$(LD) $(OBJ) ../objects/kernel/FINAL_kernel.o ../objects/libm/FINAL_libm.o.kernel_src $(LDFLAGS) -o $(OBJDIR)/$@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $^ -o $@ -D _KERNEL_SRC


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
