#include "interrupts.h"

#include "../rendering/BasicRenderer.h"
//#include "../userinput/keyboard.h"
//#include "../userinput/mouse.h"
#include "panic.h"
#include "../kernelStuff/IO/IO.h"
#include "../devices/pit/pit.h"
#include "../osData/MStack/MStackM.h"
#include "../osData/osStats.h"

#include "../devices/acpi/acpiShutdown.h"
//#include "../serialManager/serialManager.h"

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/rendering/Cols.h>


#include "../kernel.h"

#include "../scheduler/scheduler.h"

#include "../devices/serial/serial.h"


extern "C" void BruhusSafus()
{

    //GlobalRenderer->Clear(Colors.green);
    //PIT::Sleep(500);
    //GlobalRenderer->Clear(Colors.red);
    //PIT::Sleep(500);
    //RecoverDed();

    for (int i = 0; i < 50; i++)
        GlobalRenderer->ClearDotted(Colors.black);    
    for (int i = 0; i < 50; i++)
        GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->color = Colors.white;
    GlobalRenderer->Println("Shutting down...");
    for (int i = 0; i < 50; i++)
        GlobalRenderer->ClearButDont();    

    // PowerOffAcpi();


    GlobalRenderer->Clear(Colors.black);
    GlobalRenderer->Println("The ACPI shutdown failed!", Colors.yellow);

    GlobalRenderer->Println();
    GlobalRenderer->Println("Please shut down the computer manually.", Colors.white);

    while (true)
        asm("hlt");
}

void DoSafe()
{
    BruhusSafus();
}

//void** search(void** addr, void* value) __attribute__((noinline));
void** search(void** addr, void* value)
{
    while(*addr != value) addr++;
    return addr;
}
// https://stackoverflow.com/questions/27213382/how-to-modify-return-address-on-stack-in-c-or-assembly


#define SURVIVE_CRASH_OLD     void** p = search((void**)&p, __builtin_return_address(0)); \
    *p = (void*)BruhusSafus;

#define SURVIVE_CRASH asm volatile("mov %0 ,8(%%rbp)" : : "r"((void*)BruhusSafus) );

//asm volatile("mov 8(%%rbp),%0" : "=r"(returnaddr) : : );
//asm volatile("mov %%rax, %0" : "=r"(Register));
//asm volatile("mov %0, %%cr3" : : "r" (PML4) );
//asm volatile("mov %0, 8(%%rbp)" : "r"((void*)BruhusSafus) : : );


__attribute__((interrupt)) void GenericInt_handler(interrupt_frame* frame)
{
    AddToStack();
    //Panic("GENERIC INTERRUPT BRUH", true);   
    //Serial::Writelnf("> Generic Interrupt: %d", frame->interrupt_number);
    RemoveFromStack();
}


__attribute__((interrupt)) void PageFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Page Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void DoubleFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Double Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

void GPFaultRoutine(interrupt_frame* frame)
{
    AddToStack();

    //HeapCheck(false);

    Panic("General Protection Fault Detected! (ERROR: {})", to_string(frame->error_code), false);
    //Panic("General Protection Fault Detected! {}", to_string(*((uint64_t*)frame)), true);
    RemoveFromStack();

    osData.NO_INTERRUPTS = true;
    BruhusSafus();

}

__attribute__((interrupt)) void GPFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    GPFaultRoutine(frame);
    //SURVIVE_CRASH
}   

__attribute__((interrupt)) void GenFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFault1_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault 1 Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFault2_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Fault 2 Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void GenFaultWithError_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("General Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}


__attribute__((interrupt)) void GenFloatFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Float Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}



__attribute__((interrupt)) void GenMathFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("General Math Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void Debug_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Debug thing Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void Breakpoint_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Breakpoint Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}
 
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    osStats.lastKeyboardCall = PIT::TimeSinceBootMS();
    uint8_t scancode = inb(0x60);
    //GlobalRenderer->Println("Pressed: {}", to_string((uint64_t)scancode));
    if (osData.booting)
        osData.booting = false;
    else
        ;//AddScancodeToKeyboardList(scancode);
        //HandleKeyboard(scancode);  
    PIC_EndMaster();
    RemoveFromStack();
}

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08
// https://github.com/stevej/osdev/blob/master/kernel/devices/mouse.c


__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    // //Panic("GENERIC INTERRUPT BRUH", true);   
    // //osStats.lastMouseCall = PIT::TimeSinceBootMS();
    // //io_wait();
    // //Mousewait();

	// uint8_t status = inb(MOUSE_STATUS);
	// while (status & MOUSE_BBIT) 
    // {
    //     int8_t mouse_in = inb(MOUSE_PORT);
	// 	if (status & MOUSE_F_BIT)
    //     {
    //         HandlePS2Mouse(mouse_in);
    //     }
    //     status = inb(MOUSE_STATUS);
    // }


    // //uint8_t mousedata = inb(0x60);
    // //PIC_EndSlave();
    // Mousewait();
    // //io_wait();
    
    // //HandlePS2Mouse(mousedata);

    PIC_EndSlave();
    //PIC_EndMaster();
    RemoveFromStack();
}

bool speakA = false;

//#include "../audio/audioDevStuff.h"

#include "../paging/PageTableManager.h"
#include "../devices/rtc/rtc.h"

void TempPitRoutine(interrupt_frame* frame)
{
    GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);

    AddToStack();
    PIT::Tick();
    PIC_EndMaster();

    

    // AudioDeviceStuff::play(PIT::FreqAdder);
    // if (osData.serialManager != NULL)
    //     osData.serialManager->DoStuff();
    

    Point tempPoint = GlobalRenderer->CursorPosition;
    GlobalRenderer->CursorPosition.x = 0;
    GlobalRenderer->CursorPosition.y = GlobalRenderer->framebuffer->Height - 16;

    GlobalRenderer->Clear(0, GlobalRenderer->CursorPosition.y, GlobalRenderer->framebuffer->Width - 1, GlobalRenderer->CursorPosition.y + 15, Colors.black);

    GlobalRenderer->Print("DATE: ", Colors.yellow);
    GlobalRenderer->Print("{}.", to_string((int)RTC::Day), Colors.yellow);
    GlobalRenderer->Print("{}.", to_string((int)RTC::Month), Colors.yellow);
    GlobalRenderer->Print("{}", to_string((int)RTC::Year), Colors.yellow);

    GlobalRenderer->Print("  ", Colors.yellow);

    GlobalRenderer->Print("TIME: ", Colors.yellow);
    GlobalRenderer->Print("{}:", to_string((int)RTC::Hour), Colors.yellow);
    GlobalRenderer->Print("{}:", to_string((int)RTC::Minute), Colors.yellow);
    GlobalRenderer->Print("{}", to_string((int)RTC::Second), Colors.yellow);

    GlobalRenderer->CursorPosition = tempPoint;



    // TestSetSpeakerPosition(speakA);
    // speakA = !speakA;
    RemoveFromStack();

    Scheduler::SchedulerInterrupt(frame);
}

__attribute__((interrupt)) void PITInt_handler(interrupt_frame* frame)
{ 
    TempPitRoutine(frame);
}

__attribute__((interrupt)) void VirtualizationFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Virtualization Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void ControlProtectionFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Control Protection Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void HypervisorFault_handler(interrupt_frame* frame)
{
    AddToStack();
    Panic("Hypervisor Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void VMMCommunicationFault_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("VMM Communication Fault Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

__attribute__((interrupt)) void SecurityException_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Security Exception Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}


__attribute__((interrupt)) void InvalidOpCode_handler(interrupt_frame* frame)//, uint64_t error)
{
    AddToStack();
    Panic("Invalid Opcode Detected!", false);
    RemoveFromStack();

    SURVIVE_CRASH
}

/*
typedef struct 
{
    struct 
    {
        uint64_t    cr4;
        uint64_t    cr3;
        uint64_t    cr2;
        uint64_t    cr0;
    } control_registers;

    struct 
    {
        uint64_t    rdi;
        uint64_t    rsi;
        uint64_t    rdx;
        uint64_t    rcx;
        uint64_t    rbx;
        uint64_t    rax;
    } general_registers;
	
    struct 
    {
        uint64_t    rbp;
        uint64_t    vector;
        uint64_t    error_code;
        uint64_t    rip;
        uint64_t    cs;
        uint64_t    rflags;
        uint64_t    rsp;
        uint64_t    dss;
    } base_frame;
} interrupt_frame;
*/












void TestSetSpeakerPosition(bool in)
{
    uint8_t t = inb(0x61);
    //io_wait();

    if (in)
        t |= 3;
    else
        t &= 0xFC;
    
    outb(0x61, t);
    //io_wait();
}


void RemapPIC(uint8_t _a1, uint8_t _a2) 
{
    AddToStack();
    // uint8_t a1, a2;
    
    // a1 = inb(PIC1_DATA);
    // //io_wait();
    // a2 = inb(PIC2_DATA);
    // //io_wait();

    AddToStack();
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
    RemoveFromStack();

    AddToStack();
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    RemoveFromStack();

    // AddToStack();
    // outb(PIC1_DATA, 0xa1);
    // io_wait();
    // outb(PIC2_DATA, 0xa2);
    // io_wait();
    // RemoveFromStack();
    
    // restore masks
    // outb(PIC1_DATA, a1);
    // outb(PIC2_DATA, a2);  
    io_wait();
    outb(PIC1_DATA, _a1);
    io_wait();
    outb(PIC2_DATA, _a2);  
    io_wait();

    io_wait();
    outb(PIC1_DATA, _a1);
    io_wait();
    outb(PIC2_DATA, _a2);  
    io_wait();

    RemoveFromStack();
}

void PIC_EndMaster()
{
    //AddToStack();
    io_wait();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    //RemoveFromStack();
}


void PIC_EndSlave()
{
    AddToStack();
    io_wait();
    outb(PIC1_COMMAND, PIC_EOI);
    io_wait();
    outb(PIC2_COMMAND, PIC_EOI);
    io_wait();
    RemoveFromStack();
}


#define IRQHandlerCode(irq) \
    {AddToStack(); \
    IRQGenericDriverHandler(irq, frame); \
    RemoveFromStack(); }\


__attribute__((interrupt)) void IRQ2_handler(interrupt_frame* frame)
    IRQHandlerCode(2); //Cascade
__attribute__((interrupt)) void IRQ3_handler(interrupt_frame* frame)
    IRQHandlerCode(3); //COM2
__attribute__((interrupt)) void IRQ4_handler(interrupt_frame* frame)
    IRQHandlerCode(4); //COM1
__attribute__((interrupt)) void IRQ5_handler(interrupt_frame* frame)
    IRQHandlerCode(5); //LPT2
__attribute__((interrupt)) void IRQ6_handler(interrupt_frame* frame)
    IRQHandlerCode(6); //Floppy
__attribute__((interrupt)) void IRQ7_handler(interrupt_frame* frame)
    IRQHandlerCode(7); //LPT1
__attribute__((interrupt)) void IRQ8_handler(interrupt_frame* frame)
    IRQHandlerCode(8); //RTC
__attribute__((interrupt)) void IRQ9_handler(interrupt_frame* frame)
    IRQHandlerCode(9); //Free
__attribute__((interrupt)) void IRQ10_handler(interrupt_frame* frame)
    IRQHandlerCode(10); //Free
__attribute__((interrupt)) void IRQ11_handler(interrupt_frame* frame)
    IRQHandlerCode(11); //Free
__attribute__((interrupt)) void IRQ14_handler(interrupt_frame* frame)
    IRQHandlerCode(14); //Primary ATA
__attribute__((interrupt)) void IRQ15_handler(interrupt_frame* frame)
    IRQHandlerCode(15); //Secondary ATA

void* IRQHandlerCallbackHelpers[256];
void* IRQHandlerCallbackFuncs[256];

// void (*IRQHandlerCallbacks[256]) (interrupt_frame*) = {
//     NULL, //PIT
//     NULL, //Keyboard
//     NULL, //Cascade
//     NULL, //COM2
//     NULL, //COM1
//     NULL, //LPT2
//     NULL, //Floppy
//     NULL, //LPT1
//     NULL, //RTC
//     NULL, //Free
//     NULL, //Free
//     NULL, //Free
//     NULL, //PS2 Mouse
//     NULL, //FPU
//     NULL, //Primary ATA
//     NULL, //Secondary ATA
// };

void IRQGenericDriverHandler(int irq, interrupt_frame* frame)
{
    //Panic("BRUH IRQ {}", to_string(irq), true);
    if (irq < 0 || irq > 15)
    {
        Panic("Invalid IRQ {}", to_string(irq), true);
    }

    void* callbackFunc = IRQHandlerCallbackFuncs[irq];
    if (callbackFunc != NULL)
    {
        void* classInstance = IRQHandlerCallbackHelpers[irq];
        if (classInstance == NULL)
        {
            // Static Func
            ((void (*)(interrupt_frame*))callbackFunc)(frame);
        }
        else
        {
            // Member Func
            ((void (*)(void*, interrupt_frame*))callbackFunc)(classInstance, frame);
        }
    }
    else
        ;//Panic("Unhandled IRQ {}", to_string(irq), false);

    if (irq >= 8)
        PIC_EndSlave();
    else
        PIC_EndMaster();
}


extern "C" void intr_common_handler_c(interrupt_frame* frame) 
{
    //Panic("WAAAAAAAAA {}", to_string(regs->interrupt_number), true);
    if (frame->interrupt_number == 32)
        TempPitRoutine(frame);
    else if (frame->interrupt_number == 0x31)
        Syscall_handler(frame);
    else if (frame->interrupt_number == 254)
        Serial::Writelnf("> Generic Interrupt");
    else
    {
        Serial::Writelnf("> Interrupt/Exception: %d -> Closing Task...", frame->interrupt_number);
        Serial::Writeln();

        GlobalRenderer->Clear(Colors.black);
        GlobalRenderer->Println("Interrupt/Exception: {}", to_string(frame->interrupt_number), Colors.bred);
        GlobalRenderer->Println();   
        
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        Serial::Writeln();
        PrintRegisterDump(GlobalRenderer);
        Serial::Writeln();
        

        Scheduler::RemoveTask(Scheduler::CurrentRunningTask);
        Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
        
    }




    //Panic("WAAAAAAAAA {}", to_string(regs->interrupt_number), true);

    //Serial::Writelnf("> END OF INTERRUPT");
}


extern "C" void CloseCurrentTask()
{
    Serial::Writelnf("> PROGRAM REACHED END");
    Scheduler::RemoveTask(Scheduler::CurrentRunningTask);
    Scheduler::CurrentRunningTask = NULL;
}

#include <libm/syscallList.h>

void Syscall_handler(interrupt_frame* frame)
{
    Serial::Writelnf("> Syscall: %d", frame->rax);
    if (Scheduler::CurrentRunningTask == NULL)
        return;

    int syscall = frame->rax;
    frame->rax = 0;
    if (syscall == SYSCALL_GET_ARGC)
    {
        char* stack = (char*)Scheduler::CurrentRunningTask->kernelEnvStack;
        frame->rax = *((int*)(stack - 4));
        Serial::Writelnf("> Get argc %d", frame->rax);
    }
    else if (syscall == SYSCALL_GET_ARGV)
    {
        char* stack = (char*)Scheduler::CurrentRunningTask->kernelEnvStack;
        frame->rax = *((int*)(stack - 12));
        Serial::Writelnf("> Get argv %d", frame->rax);
    }
    else if (syscall == SYSCALL_GET_ENV)
    {
        char* stack = (char*)Scheduler::CurrentRunningTask->kernelEnvStack;
        frame->rax = (uint64_t)((int*)(stack - 12 - sizeof(ENV_DATA)));
        Serial::Writelnf("> Get env %d", frame->rax);
    }
    else if (syscall == SYSCALL_SERIAL_PRINT)
    {
        char* str = (char*)frame->rbx;
        Serial::Write(str);
    }
    else if (syscall == SYSCALL_SERIAL_PRINTLN)
    {
        char* str = (char*)frame->rbx;
        Serial::Writeln(str);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINT)
    {
        char* str = (char*)frame->rbx;
        Serial::Writelnf("> Printing: \"%s\"", str);
        GlobalRenderer->Print(str);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINTLN)
    {
        char* str = (char*)frame->rbx;
        Serial::Writelnf("> Printing: \"%s\"", str);
        GlobalRenderer->Println(str);
    }
    else if (syscall == SYSCALL_GLOBAL_CLS)
    {
        Serial::Writelnf("> Clearing Screen");
        GlobalRenderer->Clear(Colors.black);
    }
    else if (syscall == SYSCALL_EXIT)
    {
        Serial::Writelnf("> EXITING PROGRAM %d", frame->rbx);
        Scheduler::RemoveTask(Scheduler::CurrentRunningTask);
        Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
    }
    else
    {
        Serial::Writelnf("> Unknown Syscall: %d", syscall);
    }
    //Scheduler::SchedulerInterrupt(frame);
}