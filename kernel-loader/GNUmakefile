include ../defaults.mk
OUTPUT = kernel.elf
OBJDIR = ../objects/kernel-loader

LDFLAGS = $(LD_KERNEL_LOADER_FLAGS)
CPPFLAGS = $(CPP_RELO_FLAGS)

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
