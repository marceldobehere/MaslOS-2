#include "initialGdt.h"
#include "gdt.h"
#include "../serial/serial.h"
#include "../../paging/PageFrameAllocator.h"

TSS* tss;

static void InitialGdtSetEntry(GDTEntry* gdt, int i, unsigned int base, unsigned int limit, int flags) 
{
	Serial::Writelnf("GDT> Setting initial gdt entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", i, base, limit, flags);

    // gdt[i].Base0 = (base & 0xFFFF);
    // gdt[i].Base1 = (base >> 16) & 0xFF;
    // gdt[i].Base2 = (base >> 24) & 0xFF;

    // gdt[i].Limit0 = (limit & 0xFFFF);
    // gdt[i].Limit1_Flags = (limit >> 16) & 0x0F;
    // gdt[i].Limit1_Flags |= flags & 0xF0;

    // gdt[i].AccessByte = flags & 0xFF;

    uint64_t tempGdt = 0;

    tempGdt = limit & 0xffffLL;
	tempGdt |= (base & 0xffffffLL) << 16;
	tempGdt |= (flags & 0xffLL) << 40;
	tempGdt |= ((limit >> 16) & 0xfLL) << 48;
	tempGdt |= ((flags >> 8 )& 0xffLL) << 52;
	tempGdt |= ((base >> 24) & 0xffLL) << 56;

    *((uint64_t*)(gdt+i)) = tempGdt;
}

static void InitialGdtSetEntry2(GDTEntry* gdt, int i, uint8_t a, uint8_t g) 
{
	Serial::Writelnf("GDT> Setting initial gdt entry at %d with %d and %d", i, a, g);

    gdt[i].BaseLow = 0;
    gdt[i].BaseMid = 0;
    gdt[i].BaseHigh = 0;

    gdt[i].AccessByte = a;
    gdt[i].Granularity = g;

    gdt[i].Limit = 0;
}

// https://github.com/ethan4984/rock/blob/751b9af1009b622bedf384c1f80970b333c436c3/kernel/int/gdt.cpp#L33
void InitInitialGdt()
{
    Serial::Writelnf("GDT> Initializing GDT");

    GDT gdt;
    GDTEntry* gdtEntries = (GDTEntry*)&gdt;
    tss = (TSS*)GlobalAllocator->RequestPage();
    Serial::Writelnf("GDT> TSS is at 0x%X", tss);
    
    tss->rsp0 = 0;
    tss->rsp1 = 0;
    tss->rsp2 = 0;
    

    InitialGdtSetEntry2(gdtEntries, 0, 0, 0); // NULL
    InitialGdtSetEntry2(gdtEntries, 1, 0b10011000, 0b00100000); // KERNEL CODE 64
    InitialGdtSetEntry2(gdtEntries, 2, 0b10010110, 0); // KERNEL DATA 64
    InitialGdtSetEntry2(gdtEntries, 3, 0b11110010, 0); // USER DATA 64
    InitialGdtSetEntry2(gdtEntries, 4, 0b11111010, 0b00100000); // USER CODE 64

    // InitialGdtSetEntry(gdtEntries, 0, 0, 0, 0);
    // InitialGdtSetEntry(gdtEntries, 1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	// InitialGdtSetEntry(gdtEntries, 2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
	// InitialGdtSetEntry(gdtEntries, 3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	// InitialGdtSetEntry(gdtEntries, 4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	// InitialGdtSetEntry(gdtEntries, 5, (uint32_t)(uint64_t)tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

    DefaultGDT = gdt;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
}