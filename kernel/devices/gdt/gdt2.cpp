#include "gdt2.h"

/*
 * File:		gdt.c
 * *****************************************************************************
 * Copyright 2020-2021 Scott Maday
 * Check the LICENSE file that came with this program for licensing terms
 */

//#include "memory/pageframe.h"
//#include "memory/paging.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../paging/PageTableManager.h"


void gdt_init(struct GDTBlock* gdt_block) 
{
	uint16_t gdt_total_size = GDT_NUM * sizeof(struct GDTEntry);
	_memset(gdt_block->tss, 0, sizeof(struct TSSEntry) * GDT_TSS_NUM);
	// Set entries
	_memset(gdt_block->gdt, 0, gdt_total_size);									// implictly define null descriptor
	gdt_set_entry(gdt_block, 1, GDT_ACCESS_EXECUTABLE, 0);						// kernel code segment
	gdt_set_entry(gdt_block, 2, GDT_ACCESS_WRITABLE, 0);						// kernel data segment
	gdt_set_entry(gdt_block, 4, (GDTAccess)(GDT_ACCESS_DPL | GDT_ACCESS_WRITABLE), 0);		// user data segment
	gdt_set_entry(gdt_block, 5, (GDTAccess)(GDT_ACCESS_DPL | GDT_ACCESS_EXECUTABLE), 0);		// user code segment
	gdt_set_tss(gdt_block, 6);													// task state segment
	// Set descriptor
	gdt_block->gdt_descriptor.size = gdt_total_size - 1;
	gdt_block->gdt_descriptor.offset = gdt_block->gdt;
}

void gdt_set_entry(struct GDTBlock* gdt_block, size_t gdt_index, enum GDTAccess access, uint8_t/*enum GDTFlags*/ flags) {
	gdt_block->gdt[gdt_index].limit_low = 0xffff;
	gdt_block->gdt[gdt_index].access = GDT_ACCESS_TYPE | GDT_ACCESS_PRESENT | access;
	gdt_block->gdt[gdt_index].flags = GDT_FLAG_LONG_MODE | GDT_FLAG_PAGE_GRANULARITY | flags;
}

void gdt_set_tss(struct GDTBlock* gdt_block, size_t gdt_index) {
	size_t tss_index = 0;
	struct TSSEntry* tss = (struct TSSEntry*)(gdt_block->tss + tss_index);
	*tss = (struct TSSEntry){
		.io_map_base = 0xffff
	};
	// Set TSS descriptor entry
	struct TSSDescriptor* tss_descriptor = (struct TSSDescriptor*)(gdt_block->gdt + gdt_index);
	*tss_descriptor = (TSSDescriptor)
    {
		.limit_low = sizeof(TSSEntry),
		.base_low = (uint64_t)tss & 0xffff,
		.base_mid = ((uint64_t)tss >> 16) & 0xff,
        .access = 0b10000000 | 0b00001001,	// Present, Type (Intel Manual 3A 3.4.5.1) Execute-Only, accessed
		.flags = 0b00010000,					// Available
        .base_mid2 = ((uint64_t)tss >> 24) & 0xff,
		.base_high = ((uint64_t)tss >> 32) & 0xffffffff,
		
	};
}

void gdt_set_tss_ist(struct GDTBlock* gdt_block, size_t ist_num, void* stack) {
	size_t tss_index = 0;
	struct TSSEntry* tss = (struct TSSEntry*)(gdt_block->tss + tss_index);
	tss->ist[(ist_num - 1) * 2 + 0] = (uint32_t)(uint64_t)stack;
	tss->ist[(ist_num - 1) * 2 + 1] = (uint32_t)((uint64_t)stack >> 32);
}

void gdt_set_tss_ring(struct GDTBlock* gdt_block, size_t ring_num, void* stack) {
	size_t tss_index = 0;
	struct TSSEntry* tss = (struct TSSEntry*)(gdt_block->tss + tss_index);
	tss->rsp[ring_num * 2 + 0] = (uint32_t)(uint64_t)stack;
	tss->rsp[ring_num * 2 + 1] = (uint32_t)((uint64_t)stack >> 32);
}