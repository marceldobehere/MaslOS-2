#pragma once
#include "stdint.h"
//#include "../userinput/mouse.h"

#define  PIC1_COMMAND 0x20
#define  PIC1_DATA 0x21 
#define  PIC2_COMMAND 0xA0
#define  PIC2_DATA 0xA1

#define  PIC_EOI 0x20   

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#include <libm/interrupt_frame.h>



__attribute__((interrupt)) void GenericInt_handler(interrupt_frame* frame);

__attribute__((interrupt)) void PageFault_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame);
__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame);
__attribute__((interrupt)) void PITInt_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GenFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GenFault1_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GenFault2_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GenFaultWithError_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void GenFloatFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void GenMathFault_handler(interrupt_frame* frame);

__attribute__((interrupt)) void Debug_handler(interrupt_frame* frame);
__attribute__((interrupt)) void Breakpoint_handler(interrupt_frame* frame);
 
__attribute__((interrupt)) void VirtualizationFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void ControlProtectionFault_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void HypervisorFault_handler(interrupt_frame* frame);
__attribute__((interrupt)) void VMMCommunicationFault_handler(interrupt_frame* frame);//, uint64_t error);
__attribute__((interrupt)) void SecurityException_handler(interrupt_frame* frame);//, uint64_t error);

__attribute__((interrupt)) void InvalidOpCode_handler(interrupt_frame* frame);//, uint64_t error);

// IRQ 0 (PIT) is Handled
// IRQ 1 (Keyboard) is Handled
__attribute__((interrupt)) void IRQ2_handler(interrupt_frame* frame); //COM2
__attribute__((interrupt)) void IRQ3_handler(interrupt_frame* frame); //COM2
__attribute__((interrupt)) void IRQ4_handler(interrupt_frame* frame); //COM1
__attribute__((interrupt)) void IRQ5_handler(interrupt_frame* frame); //LPT2
__attribute__((interrupt)) void IRQ6_handler(interrupt_frame* frame); //Floppy
__attribute__((interrupt)) void IRQ7_handler(interrupt_frame* frame); //LPT1
__attribute__((interrupt)) void IRQ8_handler(interrupt_frame* frame); //CMOS
__attribute__((interrupt)) void IRQ9_handler(interrupt_frame* frame); //Free
__attribute__((interrupt)) void IRQ10_handler(interrupt_frame* frame); //Free
__attribute__((interrupt)) void IRQ11_handler(interrupt_frame* frame); //Free
// IRQ 12 (Mouse) is Handled
// IRQ 13 (FPU) is Handled
__attribute__((interrupt)) void IRQ14_handler(interrupt_frame* frame); //Primary ATA
__attribute__((interrupt)) void IRQ15_handler(interrupt_frame* frame); //Secondary ATA

void IRQGenericDriverHandler(int irq, interrupt_frame* frame);

extern void* IRQHandlerCallbackHelpers[256];
extern void* IRQHandlerCallbackFuncs[256];

//extern void (*IRQHandlerCallbacks[256]) (interrupt_frame*);

void RemapPIC(uint8_t _a1, uint8_t _a2);
void PIC_EndMaster();
void PIC_EndSlave();
void TestSetSpeakerPosition(bool in);
void DoSafe();



extern "C" void intr_common_handler_c(interrupt_frame* regs);
extern "C" void CloseCurrentTask();

extern "C" void intr_stub_0(void);
extern "C" void intr_stub_1(void);
extern "C" void intr_stub_2(void);
extern "C" void intr_stub_3(void);
extern "C" void intr_stub_4(void);
extern "C" void intr_stub_5(void);
extern "C" void intr_stub_6(void);
extern "C" void intr_stub_7(void);
extern "C" void intr_stub_8(void);
extern "C" void intr_stub_9(void);
extern "C" void intr_stub_10(void);
extern "C" void intr_stub_11(void);
extern "C" void intr_stub_12(void);
extern "C" void intr_stub_13(void);
extern "C" void intr_stub_14(void);
extern "C" void intr_stub_15(void);
extern "C" void intr_stub_16(void);
extern "C" void intr_stub_17(void);
extern "C" void intr_stub_18(void);

extern "C" void intr_stub_19(void);
extern "C" void intr_stub_20(void);
extern "C" void intr_stub_21(void);

extern "C" void intr_stub_28(void);
extern "C" void intr_stub_29(void);
extern "C" void intr_stub_30(void);

extern "C" void intr_stub_32(void);
extern "C" void intr_stub_33(void);
extern "C" void intr_stub_34(void);
extern "C" void intr_stub_35(void);
extern "C" void intr_stub_36(void);
extern "C" void intr_stub_37(void);
extern "C" void intr_stub_38(void);
extern "C" void intr_stub_39(void);
extern "C" void intr_stub_40(void);
extern "C" void intr_stub_41(void);
extern "C" void intr_stub_42(void);
extern "C" void intr_stub_43(void);
extern "C" void intr_stub_44(void);
extern "C" void intr_stub_45(void);
extern "C" void intr_stub_46(void);
extern "C" void intr_stub_47(void);

extern "C" void intr_stub_49(void);

extern "C" void intr_stub_254(void);
extern "C" void intr_stub_255(void);

extern "C" void task_entry(void);

void Syscall_handler(interrupt_frame* frame);