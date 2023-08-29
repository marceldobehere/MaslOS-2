
// #pragma once
// #include <stdint.h>

// struct GDTDescriptor
// {
//     uint16_t Size;
//     uint64_t Offset;


// }__attribute((packed));


// struct GDTEntry 
// {
//     uint16_t Limit;
//     uint16_t BaseLow;
//     uint8_t BaseMid;
//     uint8_t AccessByte;
//     uint8_t Granularity;
//     uint8_t BaseHigh;
// }__attribute((packed));

// struct MaybeGDTEntry 
// {
// 	unsigned int limit_low              : 16;
// 	unsigned int base_low               : 24;
// 	unsigned int accessed               :  1;
// 	unsigned int read_write             :  1; // readable for code, writable for data
// 	unsigned int conforming_expand_down :  1; // conforming for code, expand down for data
// 	unsigned int code                   :  1; // 1 for code, 0 for data
// 	unsigned int code_data_segment      :  1; // should be 1 for everything but TSS and LDT
// 	unsigned int DPL                    :  2; // privilege level
// 	unsigned int present                :  1;
// 	unsigned int limit_high             :  4;
// 	unsigned int available              :  1; // only used in software; has no effect on hardware
// 	unsigned int long_mode              :  1;
// 	unsigned int big                    :  1; // 32-bit opcodes for code, uint32_t stack for data
// 	unsigned int gran                   :  1; // 1 to use 4k page addressing, 0 for byte addressing
// 	unsigned int base_high              :  8;
// }__attribute((packed));


// struct GDT
// {
//     GDTEntry Null;
//     GDTEntry KernelCode;
//     GDTEntry KernelData;
//     GDTEntry UserNull;
//     GDTEntry UserCode;
//     GDTEntry UserData;

// }__attribute((packed))
// __attribute((aligned(0x1000)));


// extern GDT DefaultGDT;

// extern "C" void LoadGDT(GDTDescriptor* gdtDescritpor);
// extern "C" void FlushTSS();
