override OUTPUT := FINAL_libm.o

CC = gcc
LD = ld

LDFLAGS += -m elf_x86_64 -r
# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

NASMFLAGS ?= -F dwarf -g -f elf64
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -I ../
CPPFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -I ../
# -w



override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override NASMFILES := $(shell find . -type f -name '*.asm')

override OBJ          := $(CFILES:.c=.o)               $(CPPFILES:.cpp=.o)               $(NASMFILES:.asm=_asm.o)
override OBJ1         := $(CFILES:.c=.o.user_module)   $(CPPFILES:.cpp=.o.user_module)   $(NASMFILES:.asm=_asm.o.user_module)
override OBJ2         := $(CFILES:.c=.o.kernel_src)    $(CPPFILES:.cpp=.o.kernel_src)    $(NASMFILES:.asm=_asm.o.kernel_src)
override OBJ3         := $(CFILES:.c=.o.kernel_module) $(CPPFILES:.cpp=.o.kernel_module) $(NASMFILES:.asm=_asm.o.kernel_module)
override OBJ4         := $(CFILES:.c=.o.kernel)        $(CPPFILES:.cpp=.o.kernel)        $(NASMFILES:.asm=_asm.o.kernel)


.PHONY: all
all: $(OUTPUT)


$(OUTPUT): $(OBJ)
	$(LD)  $(OBJ1) $(LDFLAGS) -o $@.user_module
	@$(LD) $(OBJ2) $(LDFLAGS) -o $@.kernel_src
	@$(LD) $(OBJ3) $(LDFLAGS) -o $@.kernel_module


%.o: %.cpp
	$(CC)  $(CPPFLAGS) -c $^ -o $@.kernel_src -D _KERNEL_SRC
	@$(CC) $(CPPFLAGS) -c $^ -o $@.kernel_module -D _KERNEL_MODULE
	@$(CC) $(CPPFLAGS) -c $^ -o $@.user_module -D _USER_MODULE
	
%.o: %.c
	$(CC)  $(CFLAGS) -c $^ -o $@.kernel_src -D _KERNEL_SRC
	@$(CC) $(CFLAGS) -c $^ -o $@.kernel_module -D _KERNEL_MODULE
	@$(CC) $(CFLAGS) -c $^ -o $@.user_module -D _USER_MODULE

%_asm.o: %.asm
	@nasm $(NASMFLAGS) $< -o $@



.PHONY: clean
clean:
	@rm -rf $(OBJ)
	@rm -rf $(OBJ1)
	@rm -rf $(OBJ2)
	@rm -rf $(OBJ3)
	@rm -rf $(OBJ4)
	
	@rm -rf $(HEADER_DEPS)
	
	@rm -rf $(OUTPUT).kernel_src
	@rm -rf $(OUTPUT).kernel_module
	@rm -rf $(OUTPUT).user_module
	@rm -rf $(OUTPUT).kernel
