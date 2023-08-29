
#pragma once
#include <stdint.h>

struct GDTDescriptor
{
    uint16_t Size;
    uint64_t Offset;


}__attribute((packed));


struct GDTEntry 
{
    uint16_t Limit;
    uint16_t BaseLow;
    uint8_t BaseMid;
    uint8_t AccessByte;
    uint8_t Granularity;
    uint8_t BaseHigh;
}__attribute((packed));


struct GDT
{
    GDTEntry Null;
    GDTEntry KernelCode;
    GDTEntry KernelData;
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;

}__attribute((packed))
__attribute((aligned(0x1000)));


extern GDT DefaultGDT;

extern "C" void LoadGDT(GDTDescriptor* gdtDescritpor);
