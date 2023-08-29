// #include "initialGdt.h"
// #include "../serial/serial.h"
// #include "../../paging/PageFrameAllocator.h"
// #include "../../paging/PageTableManager.h"

// TSS* tss;

// static void InitialGdtSetEntry(GDTEntry* gdt, int i, unsigned int base, unsigned int limit, int flags) 
// {
// 	Serial::Writelnf("GDT> Setting initial gdt entry at %d with base 0x%x and limit 0x%x and the flags 0x%x", i, base, limit, flags);

//     // gdt[i].Base0 = (base & 0xFFFF);
//     // gdt[i].Base1 = (base >> 16) & 0xFF;
//     // gdt[i].Base2 = (base >> 24) & 0xFF;

//     // gdt[i].Limit0 = (limit & 0xFFFF);
//     // gdt[i].Limit1_Flags = (limit >> 16) & 0x0F;
//     // gdt[i].Limit1_Flags |= flags & 0xF0;

//     // gdt[i].AccessByte = flags & 0xFF;

//     uint64_t tempGdt = 0;

//     tempGdt = limit & 0xffffLL;
// 	tempGdt |= (base & 0xffffffLL) << 16;
// 	tempGdt |= (flags & 0xffLL) << 40;
// 	tempGdt |= ((limit >> 16) & 0xfLL) << 48;
// 	tempGdt |= ((flags >> 8 )& 0xffLL) << 52;
// 	tempGdt |= ((base >> 24) & 0xffLL) << 56;

//     *((uint64_t*)(gdt+i)) = tempGdt;
// }

// static void InitialGdtSetEntry2(GDTEntry* gdt, int i, uint8_t a, uint8_t g) 
// {
// 	Serial::Writelnf("GDT> Setting initial gdt entry at %d with %d and %d", i, a, g);

//     gdt[i].BaseLow = 0;
//     gdt[i].BaseMid = 0;
//     gdt[i].BaseHigh = 0;

//     gdt[i].AccessByte = a;
//     gdt[i].Granularity = g;

//     gdt[i].Limit = 0;
// }

// void writeTSS(MaybeGDTEntry* g) 
// {
// 	// Compute the base and limit of the TSS for use in the GDT entry.
// 	uint32_t base = (uint32_t)tss;
// 	uint32_t limit = sizeof(TSS);
 
// 	// Add a TSS descriptor to the GDT.
// 	g->limit_low = limit;
// 	g->base_low = base;
// 	g->accessed = 1; // With a system entry (`code_data_segment` = 0), 1 indicates TSS and 0 indicates LDT
// 	g->read_write = 0; // For a TSS, indicates busy (1) or not busy (0).
// 	g->conforming_expand_down = 0; // always 0 for TSS
// 	g->code = 1; // For a TSS, 1 indicates 32-bit (1) or 16-bit (0).
// 	g->code_data_segment=0; // indicates TSS/LDT (see also `accessed`)
// 	g->DPL = 0; // ring 0, see the comments below
// 	g->present = 1;
// 	g->limit_high = (limit & (0xf << 16)) >> 16; // isolate top nibble
// 	g->available = 0; // 0 for a TSS
// 	g->long_mode = 0;
// 	g->big = 0; // should leave zero according to manuals.
// 	g->gran = 0; // limit is in bytes, not pages
// 	g->base_high = (base & (0xff << 24)) >> 24; //isolate top byte
 
// 	// Ensure the TSS is initially zero'd.
// 	_memset(tss, 0, sizeof(TSS));
 
// 	//tss->ss0  = REPLACE_KERNEL_DATA_SEGMENT;  // Set the kernel stack segment.
// 	//tss->esp0 = REPLACE_KERNEL_STACK_ADDRESS; // Set the kernel stack pointer.
// 	//note that CS is loaded from the IDT entry and should be the regular kernel code segment
// }
 
// // void set_kernel_stack(uint32_t stack) { // Used when an interrupt occurs
// // 	tss_entry.esp0 = stack;
// // }

// // https://wiki.osdev.org/Getting_to_Ring_3
// void InitInitialGdt()
// {
//     //InitInitialGdtOG();

//     Serial::Writelnf("GDT> Initializing GDT");
//     tss = (TSS*)GlobalAllocator->RequestPage();
//     GlobalPageTableManager.MapMemory(tss, tss);
//     Serial::Writelnf("GDT> TSS is at 0x%X", tss);

//     GDT gdt;
//     GDTEntry* gdtEntries = (GDTEntry*)&gdt;
//     MaybeGDTEntry* maybeGdtEntries = (MaybeGDTEntry*)&gdt;

//     InitialGdtSetEntry2(gdtEntries, 0, 0, 0); // NULL
//     InitialGdtSetEntry2(gdtEntries, 1, 0b10011000, 0b00100000); // KERNEL CODE 64
//     InitialGdtSetEntry2(gdtEntries, 2, 0b10010110, 0); // KERNEL DATA 64
//     InitialGdtSetEntry2(gdtEntries, 3, 0b11110010, 0); // USER DATA 64
//     InitialGdtSetEntry2(gdtEntries, 4, 0b11111010, 0b00100000); // USER CODE 64

//     MaybeGDTEntry* ring3_code = &maybeGdtEntries[3];
//     MaybeGDTEntry* ring3_data = &maybeGdtEntries[4];
    
//     ring3_code->limit_low = 0xFFFF;
//     ring3_code->base_low = 0;
//     ring3_code->accessed = 0;
//     ring3_code->read_write = 1; // since this is a code segment, specifies that the segment is readable
//     ring3_code->conforming_expand_down  = 0; // does not matter for ring 3 as no lower privilege level exists
//     ring3_code->code = 1;
//     ring3_code->code_data_segment = 1;
//     ring3_code->DPL = 3; // ring 3
//     ring3_code->present = 1;
//     ring3_code->limit_high = 0xF;
//     ring3_code->available = 1;
//     ring3_code->long_mode = 1;
//     ring3_code->big = 1; // it's 32 bits
//     ring3_code->gran = 1; // 4KB page addressing
//     ring3_code->base_high = 0;
    
//     *ring3_data = *ring3_code; // contents are similar so save time by copying
//     ring3_data->code = 0; // not code but data
    
//     writeTSS(&maybeGdtEntries[5]); // TSS segment will be the fifth 
    
//     FlushTSS();


//     DefaultGDT = gdt;

//     GDTDescriptor gdtDescriptor;
//     gdtDescriptor.Size = sizeof(GDT) - 1;
//     gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
//     LoadGDT(&gdtDescriptor);
// }


// // https://github.com/ethan4984/rock/blob/751b9af1009b622bedf384c1f80970b333c436c3/kernel/int/gdt.cpp#L33
// void InitInitialGdtOG()
// {
//     Serial::Writelnf("GDT> Initializing GDT");

//     GDT gdt;
//     GDTEntry* gdtEntries = (GDTEntry*)&gdt;
//     tss = (TSS*)GlobalAllocator->RequestPage();
//     GlobalPageTableManager.MapMemory(tss, tss);
//     Serial::Writelnf("GDT> TSS is at 0x%X", tss);
    
//     tss->rsp0 = 0;
//     tss->rsp1 = 0;
//     tss->rsp2 = 0;
    

//     // InitialGdtSetEntry2(gdtEntries, 0, 0, 0); // NULL
//     // InitialGdtSetEntry2(gdtEntries, 1, 0b10011000, 0b00100000); // KERNEL CODE 64
//     // InitialGdtSetEntry2(gdtEntries, 2, 0b10010110, 0); // KERNEL DATA 64
//     // InitialGdtSetEntry2(gdtEntries, 3, 0b11110010, 0); // USER DATA 64
//     // InitialGdtSetEntry2(gdtEntries, 4, 0b11111010, 0b00100000); // USER CODE 64

//     InitialGdtSetEntry(gdtEntries, 0, 0, 0, 0);
//     InitialGdtSetEntry(gdtEntries, 1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
// 	InitialGdtSetEntry(gdtEntries, 2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
// 	InitialGdtSetEntry(gdtEntries, 3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
// 	InitialGdtSetEntry(gdtEntries, 4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
// 	InitialGdtSetEntry(gdtEntries, 5, (uint32_t)(uint64_t)tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

//     DefaultGDT = gdt;

//     GDTDescriptor gdtDescriptor;
//     gdtDescriptor.Size = sizeof(GDT) - 1;
//     gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
//     LoadGDT(&gdtDescriptor);
// }