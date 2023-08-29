// #pragma once
// #include <stdint.h>
// #include <stddef.h>
// #include "gdt.h"

// #define GDT_FLAG_DATASEG 0x02
// #define GDT_FLAG_CODESEG 0x0a
// #define GDT_FLAG_TSS	 0x09

// #define GDT_FLAG_SEGMENT 0x10
// #define GDT_FLAG_RING0   0x00
// #define GDT_FLAG_RING3   0x60
// #define GDT_FLAG_PRESENT 0x80

// #define GDT_FLAG_4K	  0x800
// #define GDT_FLAG_32_BIT  0
// //0x400

// #define GDT_ENTRIES 6

// typedef struct {
// 	uint16_t limit;
// 	void* pointer;
// } __attribute__((packed)) gdt_ptr;

// struct TSS
// {
//     // its a 64 bit tss
//     uint32_t reserved0;
//     uint64_t rsp0;
//     uint64_t rsp1;
//     uint64_t rsp2;
//     uint64_t reserved1;
//     uint64_t ist1;
//     uint64_t ist2;
//     uint64_t ist3;
//     uint64_t ist4;
//     uint64_t ist5;
//     uint64_t ist6;
//     uint64_t ist7;
//     uint64_t reserved2;
//     uint16_t reserved3;
//     uint16_t iomap_base;
// }__attribute__((packed));

// struct TSS_DESCRIPTOR
// {
//     uint16_t length;
//     uint16_t base_low;
//     uint8_t base_mid;
//     uint8_t flags1;
//     uint8_t flags2; 
//     uint8_t base_high;
//     uint32_t base_high32;
//     uint32_t reserved;

//     uint16_t selector;
// }__attribute__((packed));

// void writeTSS(MaybeGDTEntry* g);
// void InitInitialGdt();
// void InitInitialGdtOG();