override OUTPUT := kernel.elf


CC = gcc
LD = ld

    
CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -I ../
# -w

override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -Bsymbolic              \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T linker.ld

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif


override CPPFILES  := $(shell find . -type f -name '*.cpp')

override OBJ       := $(CPPFILES:.cpp=.o)


# Default target.
.PHONY: all
all: $(OUTPUT)


# Link rules for the final kernel.
$(OUTPUT): $(OBJ)
	$(LD) $(OBJ) ../kernel/FINAL_kernel.o ../libm/FINAL_libm.o.kernel_src $(LDFLAGS) -o $@


%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $^ -o $@


.PHONY: clean
clean:
	@rm -rf $(OUTPUT) $(OBJ)
