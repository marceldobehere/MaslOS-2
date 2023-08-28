# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := FINAL_libm.o
# Convenience macro to reliably declare overridable command variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
#$(eval $(call DEFAULT_VAR,CC,cc))
CC = gcc

# Same thing for "ld" (the linker).
$(eval $(call DEFAULT_VAR,LD,ld))

# User controllable CFLAGS.
CFLAGS ?= -g -O2 -pipe -Wall -Wextra

# User controllable preprocessor flags. We set none by default.
CPPFLAGS ?=

# User controllable nasm flags.
NASMFLAGS ?= -F dwarf -g

# User controllable linker flags. We set none by default.
LDFLAGS ?=

# Internal C flags that should not be changed by the user.

override CFLAGS +=       \
    -std=c14             \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-lto             \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD                 \
    -I.
    
CFLAGS2 = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -w -I ../
CFLAGS3 = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -w -I ../

#-ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fcommon

# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
    -m elf_x86_64           \
    -r

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -f elf64

# Use find to glob all *.c, *.S, and *.asm files in the directory and extract the object names.


override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override ASFILES   := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')

override OBJ         := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(ASFILES:.S=.o)
override OBJ2         := $(CFILES:.c=.o.kernel) $(CPPFILES:.cpp=.o.kernel) $(ASFILES:.S=.o.kernel) $(NASMFILES:.asm=_asm.o.kernel)
override HEADER_DEPS := $(CFILES:.c=.d) $(CPPFILES:.cpp=.d) $(ASFILES:.S=.d)

# Default target.
.PHONY: all
all: $(KERNEL)

limine.h:
	curl https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@
	$(LD) $(OBJ2) $(LDFLAGS) -o $@.kernel

# Include header dependencies.
-include $(HEADER_DEPS)



# Compilation rules for *.cpp files.
%.o: %.cpp
	$(CC) $(CFLAGS2) -c $^ -o $@
	$(CC) $(CFLAGS2) -c $^ -o $@.kernel -D _KERNEL_SRC
	
# Compilation rules for *.cpp files.
%.o: %.c
	$(CC) $(CFLAGS3) -c $^ -o $@


# Compilation rules for *.c files.
%.o: %.c limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	
	
# Compilation rules for *.S files.
%.o: %.S limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@



# Compilation rules for *.asm (nasm) files.
%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)
	rm -rf $(KERNEL) $(OBJ2) $(HEADER_DEPS)

.PHONY: distclean
distclean: clean
	rm -f limine.h
