CC = gcc
LD = ld
AS = nasm

BASE_C_FLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -ffunction-sections -fdata-sections
BASE_CPP_FLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -ffunction-sections -fdata-sections
BASE_NASM_FLAGS = -F dwarf -g -f elf64
# -w

C_RELO_FLAGS = $(BASE_C_FLAGS) -I ../ 
CPP_RELO_FLAGS = $(BASE_CPP_FLAGS) -I ../ 

C_APP_FLAGS = $(BASE_C_FLAGS) -fPIC -I ../../
CPP_APP_FLAGS = $(BASE_CPP_FLAGS) -fPIC -I ../../

BASE_LD_FLAGS = -m elf_x86_64 -unresolved-symbols=ignore-all
LD_RELO_FLAGS = $(BASE_LD_FLAGS) -r
LD_APP_FLAGS = $(BASE_LD_FLAGS) -pic --gc-sections

LD_KERNEL_LOADER_FLAGS = $(BASE_LD_FLAGS)   \
    -nostdlib                               \
    -static                                 \
    -Bsymbolic                              \
    -z max-page-size=0x1000                 \
    --gc-sections                           \
    -T linker.ld

# IDK if i need this lol
## Check if the linker supports -no-pie and enable it if it does
#ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
#    override LDFLAGS += -no-pie
#endif
