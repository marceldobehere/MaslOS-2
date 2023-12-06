struct registers {
    int eax;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
};

int strlen(char* s) {
    int l = 0;
    while (*s++) {
        l++;
    }
    return l;
}

void puts(char* s) {
    struct registers regs = { 0 };
    regs.eax = 4;
    regs.ebx = 1;
    regs.ecx = s;
    regs.edx = strlen(s);
    regs.esi = 0;
    _syscall(&regs);
}

void exit(int code) {
    struct registers regs = { 0 };
    regs.eax = 0xd;
    regs.ebx = code;
    _syscall(&regs);
}

int main() {
    puts("Hello world!\n");
    return 0;
}

extern int _bss_start;
extern int _bss_end;

void _start() {
    char* start = &_bss_start;
    char* end = &_bss_end;

    while (start < end) {
        *start++ = 0;
    }

    exit(main());
}