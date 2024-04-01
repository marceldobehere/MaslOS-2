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

#include "../rnd/rnd.h"


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

#include "../devices/keyboard/keyboard.h"
 
__attribute__((interrupt)) void KeyboardInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    osStats.lastKeyboardCall = PIT::TimeSinceBootMS();
    uint8_t scancode = inb(0x60);
    //GlobalRenderer->Println("Pressed: {}", to_string((uint64_t)scancode));
    if (osData.booting)
        osData.booting = false;
    else
        Keyboard::HandleKeyboardInterrupt(scancode);//AddScancodeToKeyboardList(scancode);
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

#include "../devices/mouse/mouse.h"

__attribute__((interrupt)) void MouseInt_handler(interrupt_frame* frame)
{ 
    AddToStack();
    //Serial::Writelnf("INT> MOUSE START");
    //Panic("GENERIC INTERRUPT BRUH", true);   
    //osStats.lastMouseCall = PIT::TimeSinceBootMS();
    //io_wait();
    //Mousewait();

	uint8_t status = inb(MOUSE_STATUS);
	while (status & MOUSE_BBIT) 
    {
        int8_t mouse_in = inb(MOUSE_PORT);
		if (status & MOUSE_F_BIT)
        {
            Mouse::HandlePS2Mouse(mouse_in);
        }
        status = inb(MOUSE_STATUS);
    }


    // //uint8_t mousedata = inb(0x60);
    // //PIC_EndSlave();
    // Mousewait();
    // //io_wait();
    
    // //HandlePS2Mouse(mousedata);

    PIC_EndSlave();
    //PIC_EndMaster();
    //Serial::Writelnf("INT> MOUSE END");
    RemoveFromStack();
}

bool speakA = false;

//#include "../audio/audioDevStuff.h"

#include "../paging/PageTableManager.h"
#include "../devices/rtc/rtc.h"
#include "../memory/heap.h"

int _usedHeapCount = 0;
int _usedPages = 0;

void DrawStats()
{    
    Point tempPoint = GlobalRenderer->CursorPosition;
    GlobalRenderer->CursorPosition.x = 0;
    GlobalRenderer->CursorPosition.y = GlobalRenderer->framebuffer->Height - (osData.inBootProcess ? 16 : 80);

    GlobalRenderer->Clear(0, GlobalRenderer->CursorPosition.y, GlobalRenderer->framebuffer->Width - 1, GlobalRenderer->CursorPosition.y + 15, Colors.black);

    uint32_t currCol = 0;
    
    // currCol = Colors.orange;
    // GlobalRenderer->Print("DATE: ", currCol);
    // GlobalRenderer->Print("{}.", to_string((int)RTC::Day), currCol);
    // GlobalRenderer->Print("{}.", to_string((int)RTC::Month), currCol);
    // GlobalRenderer->Print("{}", to_string((int)RTC::Year), currCol);
    // GlobalRenderer->Print(" - ", Colors.white);

    // currCol = Colors.yellow;
    // GlobalRenderer->Print("{}:", to_string((int)RTC::Hour), currCol);
    // GlobalRenderer->Print("{}:", to_string((int)RTC::Minute), currCol);
    // GlobalRenderer->Print("{}", to_string((int)RTC::Second), currCol);
    // GlobalRenderer->Print(" - ", Colors.white);

    currCol = Colors.bgreen;
    GlobalRenderer->Print("HEAP: ", currCol);
    GlobalRenderer->Print("Used Count: {}, ", to_string(usedHeapCount), currCol);
    GlobalRenderer->Print("Used Amount: {} Bytes", to_string(usedHeapAmount), currCol);
    // GlobalRenderer->Print("Malloc Count: {}", to_string((int)mallocCount), currCol);
    GlobalRenderer->Print(" - ", Colors.white);


    currCol = Colors.cyan;
    GlobalRenderer->Print("GLOB ALLOC: ", currCol);
    GlobalRenderer->Print("Used: {} KB / ", to_string(GlobalAllocator->GetUsedRAM() / 1024), currCol);
    GlobalRenderer->Print("{} KB", to_string(GlobalAllocator->GetFreeRAM() / 1024), currCol);
    GlobalRenderer->Print("  - ", Colors.white);

    currCol = Colors.lime;
    GlobalRenderer->Print("Runnings Tasks: ", currCol);
    if (!Scheduler::osTasks.IsLocked())
    {
        Scheduler::osTasks.Lock();
        GlobalRenderer->Print("{}", to_string(Scheduler::osTasks.obj->GetCount()), currCol);
        Scheduler::osTasks.Unlock();
    }
    //GlobalRenderer->Print("  - ", Colors.white);

    #define PRINT_MEM_STATS_TO_SERIAL false

    if (mallocCount > 0 && PRINT_MEM_STATS_TO_SERIAL)
        Serial::TWritelnf("MEM> Malloced %d times", mallocCount);
    if (freeCount > 0 && PRINT_MEM_STATS_TO_SERIAL)
        Serial::TWritelnf("MEM> Freed %d times", freeCount);

    freeCount = 0;
    mallocCount = 0;
    
    GlobalRenderer->CursorPosition = tempPoint;

    if (usedHeapCount != _usedHeapCount && PRINT_MEM_STATS_TO_SERIAL)
    {
        _usedHeapCount = usedHeapCount;
        Serial::TWritelnf("MEM> Used Heap Count: %d", usedHeapCount);
        Serial::TWritelnf("MEM> Used Heap Amount: %d", usedHeapAmount);
    }

    if (GlobalAllocator->GetUsedRAM() / 0x1000 != _usedPages && PRINT_MEM_STATS_TO_SERIAL)
    {
        _usedPages = GlobalAllocator->GetUsedRAM() / 0x1000;

        Serial::TWritelnf("MEM> Used Pages: %d", _usedPages);
    }
}

#include "../audio/audioDevStuff.h"

int _pitCount = 0;
int _pitCount2 = 0;

void TempPitRoutine(interrupt_frame* frame)
{
    //GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);

    AddToStack();
    PIT::Tick();
    PIC_EndMaster();

    

    AudioDeviceStuff::play(PIT::FreqAdder);
    // if (osData.serialManager != NULL)
    //     osData.serialManager->DoStuff();

    int silly = 1;
    if (PIT::Divisor != 0)
        silly = PIT::NonMusicDiv / PIT::Divisor;
    if (silly < 1)
        silly = 1;

    if (++_pitCount >= 80 * silly && true)   
    {
        _pitCount = 0;
        DrawStats();
    }


    RemoveFromStack();

    if (++_pitCount2 >= 3 * silly)
    {
        _pitCount2 = 0;
        Scheduler::SchedulerInterrupt(frame);
    }

    // if (Scheduler::CurrentRunningTask != NULL)
    //     MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
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

void* currentMappedTask = NULL;

// void MapMemoryOfCurrentTask(osTask* task)
// {
//     if (task == NULL)
//         return;
    
//     if (task->pageTableContext == NULL)
//         return;

//     if (currentMappedTask == task)
//         return;
//     currentMappedTask = task;

//     //GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);

//     Serial::Writelnf("INT> Mapping %d pages of task %X (Start %X)", task->requestedPages->GetCount(), (uint64_t)task, (uint64_t)task->addrOfVirtPages);
//     // //PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);

//     // // we map the requested pages into the global space so we can access em rn
//     // for (int i = 0; i < task->requestedPages->GetCount(); i++)
//     // {
//     //     void* realPageAddr = task->requestedPages->ElementAt(i);
//     //     void* virtPageAddr = (void*)((uint64_t)task->addrOfVirtPages + 0x1000 * i);
//     //     //Serial::Writelnf("    > Mapping %X to %X", (uint64_t)realPageAddr, (uint64_t)virtPageAddr);
//     //     GlobalPageTableManager.MapMemory(virtPageAddr, realPageAddr, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
//     //     //manager.MapMemory(virtPageAddr, (void*)realPageAddr, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
//     // }
// }

bool SendMessageToTask(GenericMessagePacket* oldPacket, uint64_t targetPid, uint64_t sourcePid)
{
    if (oldPacket == NULL)
        return false;
    
    osTask* otherTask = Scheduler::GetTask(targetPid);

    if (otherTask == NULL)
        return false;

    //Serial::Writelnf("> Sending2 %d from %X to %X", oldPacket->Size, sourcePid, otherTask->pid);
    GenericMessagePacket* newPacket = oldPacket->Copy();
    otherTask->messages->Enqueue(newPacket);
    return true;
}

#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/mousePacket/mousePacket.h>

bool InterruptGoingOn = false;
int currentInterruptCount = 0;
int lastInt = 0;

Mouse::MiniMousePacket lastMousePacket = Mouse::InvalidMousePacket;

extern "C" void intr_common_handler_c(interrupt_frame* frame) 
{
    //asm volatile("mov %0, %%cr3" : : "r"((uint64_t)GlobalPageTableManager.PML4) : "memory");
    //GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);
    
    //Serial::Writelnf("INT> INT %d, (%X, %X)", frame->interrupt_number, frame->cr3, frame->cr0);

    AddToStack();


    if (InterruptGoingOn)
    {
        Serial::TWritelnf("WAAAA> INT %d IS INTERRUPTING INT %d!", frame->interrupt_number, lastInt);
        //Panic("INT IN INT", true);

        for (int i = 0; i < 20; i++)
            GlobalRenderer->ClearDotted(Colors.bred);

        GlobalRenderer->Println("INT IN INT", Colors.white);

        for (int i = 0; i < 40; i++)
            GlobalRenderer->ClearButDont();

        //while (true);
        //return;
    }
    lastInt = frame->interrupt_number;
    InterruptGoingOn = true;

    int rnd = RND::RandomInt();
    
    //Panic("WAAAAAAAAA {}", to_string(regs->interrupt_number), true);
    if (frame->interrupt_number == 32)
        TempPitRoutine(frame);
    else if (frame->interrupt_number == 0x31)
    {
        Syscall_handler(frame);
        //Serial::Writeln("> SYS DONE");
    }
    else if (frame->interrupt_number == 254)
        Serial::TWritelnf("> Generic Interrupt");
    else
    {
        currentInterruptCount++;
        Serial::TWritelnf("> Interrupt/Exception: %d (Count %d) -> Closing Task...", frame->interrupt_number, currentInterruptCount);
        Serial::Writeln();

        GlobalRenderer->Clear(Colors.black);
        GlobalRenderer->Print("Interrupt/Exception: {}", to_string(frame->interrupt_number), Colors.bred);
        GlobalRenderer->Println(" (Count: {})", to_string(currentInterruptCount), Colors.bred);
        GlobalRenderer->Println();   
        
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        Serial::Writeln();
        PrintRegisterDump(GlobalRenderer);
        Serial::Writeln();
        PrintTaskRegisterDump(GlobalRenderer, frame);
        Serial::Writeln();

        GlobalRenderer->Println();   
        GlobalRenderer->Println("ERROR CODE: {}", to_string(frame->error_code), Colors.yellow);   
        Serial::Writelnf("ERROR CODE: %d", frame->error_code);


        if (osData.inBootProcess)
        {
            Serial::TWritelnf("INT> CRASH IN BOOT IS FATAL -> HALT");
            while (true);
        }

        if (Scheduler::CurrentRunningTask != NULL)
        {
            Serial::Writeln("Task info:");
            osTask* task = Scheduler::CurrentRunningTask;
            Serial::Writelnf("Task: %X", (uint64_t)task);
            Serial::Writelnf("Task->kernelStack: %X", (uint64_t)task->kernelStack);
            Serial::Writelnf("Task->userStack: %X", (uint64_t)task->userStack);
            Serial::Writelnf("Task->pageTableContext: %X", (uint64_t)task->pageTableContext);
            Serial::Writelnf("Task->frame: %X", (uint64_t)task->frame);
            Serial::Writelnf(" - Task->frame->rip: %X", task->frame->rip);
            Serial::Writelnf(" - Task->frame->rsp: %X", task->frame->rsp);
            Serial::Writelnf(" - Task->frame->rbp: %X", task->frame->rbp);
            Serial::Writelnf(" - Task->frame->rflags: %X", task->frame->rflags);
            Serial::Writelnf(" - Task->frame->cs: %X", task->frame->cs);
            Serial::Writelnf(" - Task->frame->ss: %X", task->frame->ss);
            Serial::Writelnf(" - Task->frame->error_code: %X", task->frame->error_code);
    
            Serial::Writelnf("Task->requestedPages: %d", task->requestedPages->GetCount());
            Serial::Writelnf("Task->Priority: %d/%d", task->priorityStep, task->priority);
            Serial::Writelnf("Task->Timeout: %d", task->taskTimeoutDone);

            Serial::Writelnf("Task->removeMe: %B", task->removeMe);
            Serial::Writelnf("Task->active: %B", task->active);
            Serial::Writelnf("Task->doExit: %B", task->doExit);
            Serial::Writelnf("Task->isUserMode: %B", !task->isKernelModule);

            Serial::Writelnf("Task->pid: %X", task->pid);
            Serial::Writelnf("Task->parentPid: %X", task->parentPid);
            Serial::Writelnf("Task->elfPath: \"%s\"", task->elfPath);
            Serial::Writelnf("Task->startedAtPath: \"%s\"", task->startedAtPath);
        }

        {
            Serial::Writeln("> Resetting MStackPointer");
            MStackData::stackPointer = 1;
            for (int i = 1; i < 1000; i++)
                MStackData::stackArr[i] = MStack();
        }

        if (Scheduler::CurrentRunningTask != NULL)
        {
            Scheduler::CurrentRunningTask->active = false;
            Scheduler::CurrentRunningTask->removeMe = true;
        }
        Scheduler::CurrentRunningTask = NULL;

        DrawStats();

        for (int i = 0; i < 20; i++)
            GlobalRenderer->ClearButDont();

        //while (true);

        //Serial::Writelnf("> END OF INT (%X, %X)", frame->cr3, frame->cr0);
        InterruptGoingOn = false;
        Scheduler::SchedulerInterrupt(frame);
        //frame.cr3 = (uint64_t)Scheduler::CurrentRunningTask->pageTableContext;
        // if (Scheduler::CurrentRunningTask != NULL)
        //     MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        RemoveFromStack();
        return;
    }

    //Serial::Write("<#");
    if (Scheduler::DesktopTask != NULL && !Scheduler::DesktopTask->removeMe)
    {
        int keysToDo = min(150, Keyboard::KeysAvaiable());
        for (int i = 0; i < keysToDo; i++)
        {
            Keyboard::MiniKeyInfo info = Keyboard::DoAndGetKey();
            if (info.Scancode == 0)
                continue;

            KeyMessagePacketType keyType;
            if (info.IsPressed)
                keyType = KeyMessagePacketType::KEY_PRESSED;
            else
                keyType = KeyMessagePacketType::KEY_RELEASE;

            KeyMessagePacket keyPacket = KeyMessagePacket(keyType, info.Scancode, info.AsciiChar);
            
            GenericMessagePacket* packet = new GenericMessagePacket(
                MessagePacketType::KEY_EVENT,
                (uint8_t*)&keyPacket,
                sizeof(KeyMessagePacket)
            );
            
            //Serial::Writelnf("INT> Sending key packet to desktop task");
            SendMessageToTask(packet, Scheduler::DesktopTask->pid, 1);
            
            packet->Free();
            _Free(packet);
        }

        int mouseToDo = min(150, Mouse::MousePacketsAvailable());
        for (int i = 0; i < mouseToDo; i++)
        {
            if (Mouse::mousePackets.IsLocked())
                break;
        
            Mouse::mousePackets.Lock();
            MousePacket mPacket = Mouse::mousePackets.obj->Dequeue();
            Mouse::mousePackets.Unlock();

            //Serial::Writelnf("INT> Doing mouse packet");
            Mouse::MiniMousePacket packet = Mouse::ProcessMousePacket(mPacket);
            if (!packet.Valid)
                continue;


            MouseMessagePacket mousePacket = MouseMessagePacket(packet.X, packet.Y);

            mousePacket.Left = packet.LeftButton;
            mousePacket.Right = packet.RightButton;
            mousePacket.Middle = packet.MiddleButton;

            mousePacket.PrevLeft = lastMousePacket.LeftButton;
            mousePacket.PrevRight = lastMousePacket.RightButton;
            mousePacket.PrevMiddle = lastMousePacket.MiddleButton;

            bool clickDone = false;
            if (packet.LeftButton && !lastMousePacket.LeftButton)
                clickDone = true;
            if (packet.RightButton && !lastMousePacket.RightButton)
                clickDone = true;
            if (packet.MiddleButton && !lastMousePacket.MiddleButton)
                clickDone = true;

            lastMousePacket = packet;

            if (clickDone)
            {
                mousePacket.Type = MouseMessagePacketType::MOUSE_CLICK;
                //Serial::Writelnf("INT> Sending mouse click packet to desktop task");
            }

            //Serial::Writelnf("INT> Sending mouse packet to desktop task 2");
            GenericMessagePacket* packet2 = new GenericMessagePacket(
                MessagePacketType::MOUSE_EVENT,
                (uint8_t*)&mousePacket,
                sizeof(MouseMessagePacket)
            );
            //Serial::Writelnf("INT> Sending mouse packet to desktop task 3");
            
            //Serial::Writelnf("INT> Sending mouse packet to desktop task");
            SendMessageToTask(packet2, Scheduler::DesktopTask->pid, 1);
            //Serial::Writelnf("INT> Sending mouse packet to desktop task 4");
            
            packet2->Free();
            _Free(packet2);

            //Serial::Writelnf("INT> Sending mouse packet to desktop task 5");
        }
    }
    //Serial::Write("#>");

    if (Scheduler::CurrentRunningTask == NULL)
    {
        //Serial::Writelnf("> END OF INTERRUPT");
        InterruptGoingOn = false;
        RemoveFromStack();
        return;
    }

    //frame->cr3 = (uint64_t)Scheduler::CurrentRunningTask->pageTableContext;

    //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
    //Panic("WAAAAAAAAA {}", to_string(regs->interrupt_number), true);

    //Serial::Writelnf("> END OF INT (%X, %X)", frame->cr3, frame->cr0);
    InterruptGoingOn = false;
    RemoveFromStack();
}


extern "C" void CloseCurrentTask()
{
    Serial::TWritelnf("> PROGRAM REACHED END");
    if (Scheduler::CurrentRunningTask != NULL)
    {
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;
    }
    // Scheduler::RemoveTask();
    // Scheduler::CurrentRunningTask = NULL;
}

#include <libm/syscallList.h>

uint64_t _tempGenericMessageConvoId = 0;
bool _tempGenericMessageChecker(GenericMessagePacket* msg)
{
    if (msg == NULL)
        return false;
    
    return msg->ConvoID == _tempGenericMessageConvoId;
}

bool IsAddressValidForTask(const void* addr)
{
    return IsAddressValidForTask(addr, Scheduler::CurrentRunningTask);
}
bool IsAddressValidForTask(const void* addr, osTask* task)
{
    if (task == NULL)
        return false;
    
    if (addr == NULL)
        return false;

    if (task->pageTableContext == NULL)
        return false;


    if (addr >= task->kernelStack && addr < task->kernelStack + KERNEL_STACK_PAGE_SIZE * 0x1000)
        return true;

    if (addr >= task->userStack && addr < task->userStack + USER_STACK_PAGE_SIZE * 0x1000)
        return true;

    if (addr >= (void*)task->addrOfVirtPages && addr < (void*)((uint64_t)task->addrOfVirtPages + 0x1000 * task->requestedPages->GetCount()))
        return true;

    if (addr >= task->elfFile.offset && addr < (char*)task->elfFile.offset + task->elfFile.size * 0x1000)
        return true;

    if (task->isKernelModule)
        return true;

    return false;
}

#include <libm/heap/heap.h>
#include <libm/cstrTools.h>
#include <libm/mouseState.h>
#include <libm/rtc/rtcInfo.h>
#include <libm/fsStuff/extra/fsExtra.h>

#include "../fsStuff/fsStuff.h"

void FS_Syscall_handler(int syscall, interrupt_frame* frame)
{
    if (syscall == SYSCALL_FS_CREATE_FILE)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            frame->rax = (bool)FS_STUFF::CreateFileIfNotExist(path);
        }
    }
    else if (syscall == SYSCALL_FS_CREATE_FILE_WITH_SIZE)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            uint64_t size = frame->rcx;
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->CreateFile(path2, size) == FilesystemInterface::FSCommandResult.SUCCESS);
                    _Free((void*)path2);
                }
            }
        }
    }
    else if (syscall == SYSCALL_FS_CREATE_FOLDER)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->CreateFolder(path2) == FilesystemInterface::FSCommandResult.SUCCESS);
                    _Free((void*)path2);
                }
            }
        }       
    }
    else if (syscall == SYSCALL_FS_DELETE_FILE)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->DeleteFile(path2) == FilesystemInterface::FSCommandResult.SUCCESS);
                    _Free((void*)path2);
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_DELETE_FOLDER)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->DeleteFolder(path2) == FilesystemInterface::FSCommandResult.SUCCESS);
                    _Free((void*)path2);
                }
            }
        } 
    }
    // TODO: RENAME/COPY BETWEEN DISKS
    else if (syscall == SYSCALL_FS_RENAME_FILE)
    {
        const char* path = (const char*)frame->rbx;
        const char* newPath = (const char*)frame->rcx;
        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(newPath))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                const char* newPath2 = FS_EXTRA::GetFilePathFromFullPath(newPath);
                if (path2 != NULL && newPath2 != NULL)
                {
                    frame->rax = (bool)(fs->RenameFile(path2, newPath2) == FilesystemInterface::FSCommandResult.SUCCESS);
                }
                if (path2 != NULL)
                    _Free((void*)path2);
                if (newPath2 != NULL)
                    _Free((void*)newPath2);
            }
        } 
    }
    else if (syscall == SYSCALL_FS_RENAME_FOLDER)
    {
        const char* path = (const char*)frame->rbx;
        const char* newPath = (const char*)frame->rcx;
        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(newPath))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                const char* newPath2 = FS_EXTRA::GetFilePathFromFullPath(newPath);
                if (path2 != NULL && newPath2 != NULL)
                {
                    frame->rax = (bool)(fs->RenameFolder(path2, newPath2) == FilesystemInterface::FSCommandResult.SUCCESS);
                }
                if (path2 != NULL)
                    _Free((void*)path2);
                if (newPath2 != NULL)
                    _Free((void*)newPath2);
            }
        } 
    }
    else if (syscall == SYSCALL_FS_COPY_FILE)
    {
        const char* path = (const char*)frame->rbx;
        const char* newPath = (const char*)frame->rcx;
        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(newPath))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                const char* newPath2 = FS_EXTRA::GetFilePathFromFullPath(newPath);
                if (path2 != NULL && newPath2 != NULL)
                {
                    frame->rax = (bool)(fs->CopyFile(path2, newPath2) == FilesystemInterface::FSCommandResult.SUCCESS);
                }
                if (path2 != NULL)
                    _Free((void*)path2);
                if (newPath2 != NULL)
                    _Free((void*)newPath2);
            }
        } 
    }
    else if (syscall == SYSCALL_FS_COPY_FOLDER)
    {
        const char* path = (const char*)frame->rbx;
        const char* newPath = (const char*)frame->rcx;
        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(newPath))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                const char* newPath2 = FS_EXTRA::GetFilePathFromFullPath(newPath);
                if (path2 != NULL && newPath2 != NULL)
                {
                    frame->rax = (bool)(fs->CopyFolder(path2, newPath2) == FilesystemInterface::FSCommandResult.SUCCESS);
                }
                if (path2 != NULL)
                    _Free((void*)path2);
                if (newPath2 != NULL)
                    _Free((void*)newPath2);
            }
        } 
    }
    else if (syscall == SYSCALL_FS_FILE_EXISTS)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->FileExists(path2));
                    _Free((void*)path2);
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_FOLDER_EXISTS)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->FolderExists(path2));
                    _Free((void*)path2);
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_GET_FILES_IN_PATH)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        frame->rbx = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFolderPathFromFullPath(path);
                if (path2 != NULL)
                {
                    if (StrEquals(path2, ""))
                    {
                        _Free(path2);
                        path2 = StrCopy("/");
                    }

                    uint64_t fileCount = 0;
                    const char** res = fs->GetFiles(path2, &fileCount);
                    
                    if (res != NULL)
                    {
                        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
                        char** res2 = (char**)taskHeap->_Xmalloc(sizeof(char*) * fileCount, "Malloc for fsGetFiles");
                        if (IsAddressValidForTask(res2))
                        {
                            for (int i = 0; i < fileCount; i++)
                            {
                                const char* cV = res[i];
                                int len = StrLen(cV);
                                res2[i] = (char*)taskHeap->_Xmalloc(len + 1, "Malloc for fsGetFiles Str");
                                if (IsAddressValidForTask(res2[i]))
                                {
                                    _memcpy((void*)cV, (void*)res2[i], len);
                                    res2[i][len] = 0;
                                }
                            }
                        }

                        frame->rax = (uint64_t)res2;
                        frame->rbx = fileCount;

                        for (int i = 0; i < fileCount; i++)
                            _Free(res[i]);
                        _Free(res);
                    }

                    _Free((void*)path2);
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_GET_FOLDERS_IN_PATH)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        frame->rbx = 0;
        if (IsAddressValidForTask(path))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            //Serial::Writelnf("FS: %X", fs);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFolderPathFromFullPath(path);
                //Serial::Writelnf("PATH: \"%s\"", path2);
                if (path2 != NULL)
                {
                    if (StrEquals(path2, ""))
                    {
                        _Free(path2);
                        path2 = StrCopy("/");
                        //Serial::Writelnf("> PATH: \"%s\"", path2);
                    }

                    uint64_t folderCount = 0;
                    const char** res = fs->GetFolders(path2, &folderCount);
                    //Serial::Writelnf("RES: %X", res);
                    
                    if (res != NULL)
                    {
                        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
                        char** res2 = (char**)taskHeap->_Xmalloc(sizeof(char*) * folderCount, "Malloc for fsGetFolders");
                        if (IsAddressValidForTask(res2))
                        {
                            for (int i = 0; i < folderCount; i++)
                            {
                                const char* cV = res[i];
                                int len = StrLen(cV);
                                res2[i] = (char*)taskHeap->_Xmalloc(len + 1, "Malloc for fsGetFolders Str");
                                if (IsAddressValidForTask(res2[i]))
                                {
                                    _memcpy((void*)cV, (void*)res2[i], len);
                                    res2[i][len] = 0;
                                }
                            }
                        }

                        frame->rax = (uint64_t)res2;
                        frame->rbx = folderCount;

                        for (int i = 0; i < folderCount; i++)
                            _Free(res[i]);
                        _Free(res);
                    }

                    _Free((void*)path2);
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_GET_DRIVES_IN_ROOT)
    {
        frame->rax = 0;
        frame->rbx = 0;

        int driveCount = 0;
        for (int i = 0; i < osData.diskInterfaces.GetCount(); i++)
        {
            DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
            if (diskInterface->partitionInterface == NULL)
                continue;

            PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
            for (int i2 = 0; i2 < partInterface->partitionList.GetCount(); i2++)
            {
                PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
                if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
                    driveCount++;
            }
        }

        if (driveCount > 0)
        {
            Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;

            char** res2 = (char**)taskHeap->_Xmalloc(sizeof(char*) * driveCount, "Malloc for fsGetDisks");
            if (IsAddressValidForTask(res2))
            {
                int i3 = 0;
                for (int i = 0; i < osData.diskInterfaces.GetCount(); i++)
                {
                    DiskInterface::GenericDiskInterface* diskInterface = osData.diskInterfaces[i];
                    if (diskInterface->partitionInterface == NULL)
                        continue;

                    PartitionInterface::GenericPartitionInterface* partInterface = (PartitionInterface::GenericPartitionInterface*)diskInterface->partitionInterface;
                    for (int i2 = 0; i2 < partInterface->partitionList.GetCount(); i2++)
                    {
                        PartitionInterface::PartitionInfo* partInfo = partInterface->partitionList[i2];
                        if (!partInfo->hidden && partInfo->type == PartitionInterface::PartitionType::Normal)
                        {
                            const char* driveName = partInfo->driveName;
                            int len = StrLen(driveName);
                            res2[i3] = (char*)taskHeap->_Xmalloc(len + 1, "Malloc for fsGetDisks Str");
                            if (IsAddressValidForTask(res2[i3]))
                            {
                                _memcpy((void*)driveName, (void*)res2[i3], len);
                                res2[i3][len] = 0;
                            }
                            i3++;
                        }
                    }
                }

                frame->rax = (uint64_t)res2;
                frame->rbx = driveCount;
            }
        }
    }
    else if (syscall == SYSCALL_FS_GET_FILE_INFO)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FsInt::FileInfo* info = FS_STUFF::GetFileInfoFromFullPath(path);
            if (info != NULL)
            {
                Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
                FsInt::FileInfo* info2 = (FsInt::FileInfo*)taskHeap->_Xmalloc(sizeof(FsInt::FileInfo), "Malloc for fsGetFileInfo");
                if (IsAddressValidForTask(info2))
                {
                    info2->baseInfo = (FsInt::BaseInfo*)taskHeap->_Xmalloc(sizeof(FsInt::BaseInfo), "Malloc for fsGetFileInfo baseInfo");
                    if (IsAddressValidForTask(info2->baseInfo))
                    {
                        info2->baseInfo->hidden = info->baseInfo->hidden;
                        info2->baseInfo->pathLen = info->baseInfo->pathLen;
                        info2->baseInfo->systemFile = info->baseInfo->systemFile;
                        info2->baseInfo->writeProtected = info->baseInfo->writeProtected;
                        info2->baseInfo->path = (char*)taskHeap->_Xmalloc(info->baseInfo->pathLen + 1, "Malloc for fsGetFileInfo baseInfo path");
                        if (IsAddressValidForTask(info2->baseInfo->path))
                        {
                            _memcpy((void*)info->baseInfo->path, (void*)info2->baseInfo->path, info->baseInfo->pathLen);
                            ((char*)info2->baseInfo->path)[info->baseInfo->pathLen] = 0;

                            info2->sizeInBytes = info->sizeInBytes;
                            info2->locationInBytes = info->locationInBytes;

                            frame->rax = (uint64_t)info2;
                        }
                    }
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_GET_FOLDER_INFO)
    {
        const char* path = (const char*)frame->rbx;
        frame->rax = 0;
        if (IsAddressValidForTask(path))
        {
            FsInt::FolderInfo* info = FS_STUFF::GetFolderInfoFromFullPath(path);
            if (info != NULL)
            {
                Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
                FsInt::FolderInfo* info2 = (FsInt::FolderInfo*)taskHeap->_Xmalloc(sizeof(FsInt::FolderInfo), "Malloc for fsGetFolderInfo");
                if (IsAddressValidForTask(info2))
                {
                    info2->baseInfo = (FsInt::BaseInfo*)taskHeap->_Xmalloc(sizeof(FsInt::BaseInfo), "Malloc for fsGetFolderInfo baseInfo");
                    if (IsAddressValidForTask(info2->baseInfo))
                    {
                        info2->baseInfo->hidden = info->baseInfo->hidden;
                        info2->baseInfo->pathLen = info->baseInfo->pathLen;
                        info2->baseInfo->systemFile = info->baseInfo->systemFile;
                        info2->baseInfo->writeProtected = info->baseInfo->writeProtected;
                        info2->baseInfo->path = (char*)taskHeap->_Xmalloc(info->baseInfo->pathLen + 1, "Malloc for fsGetFolderInfo baseInfo path");
                        if (IsAddressValidForTask(info2->baseInfo->path))
                        {
                            _memcpy((void*)info->baseInfo->path, (void*)info2->baseInfo->path, info->baseInfo->pathLen);
                            ((char*)info2->baseInfo->path)[info->baseInfo->pathLen] = 0;

                            frame->rax = (uint64_t)info2;
                        }
                    }
                }
            }
        } 
    }
    else if (syscall == SYSCALL_FS_READ_FILE_INTO_BUFFER)
    {
        const char* path = (const char*)frame->rbx;
        char* buffer = (char*)frame->rcx;
        uint64_t start = frame->rdx;
        uint64_t byteCount = frame->rsi;

        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(buffer) && IsAddressValidForTask(buffer + byteCount))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    const char* res = fs->ReadFileBuffer(path2, start, byteCount, buffer);
                    frame->rax = (bool)(res == FilesystemInterface::FSCommandResult.SUCCESS);

                    _Free((void*)path2);
                }
            }
        }
    }
    else if (syscall == SYSCALL_FS_WRITE_FILE_FROM_BUFFER)
    {
        const char* path = (const char*)frame->rbx;
        char* buffer = (char*)frame->rcx;
        uint64_t byteCount = frame->rdx;

        frame->rax = 0;
        if (IsAddressValidForTask(path) && IsAddressValidForTask(buffer) && IsAddressValidForTask(buffer + byteCount))
        {
            FilesystemInterface::GenericFilesystemInterface* fs = FS_STUFF::GetFsInterfaceFromFullPath(path);
            if (fs != NULL)
            {
                const char* path2 = FS_EXTRA::GetFilePathFromFullPath(path);
                if (path2 != NULL)
                {
                    frame->rax = (bool)(fs->WriteFile(path2, byteCount, buffer) == FilesystemInterface::FSCommandResult.SUCCESS);

                    _Free((void*)path2);
                }
            }
        }
    }
    else
    {
        Serial::TWritelnf("Unknown FS syscall: %d", syscall);
    }


}

void Syscall_handler(interrupt_frame* frame)
{
    //Serial::Writelnf("> Syscall: %d (%X, %X, %X, %X), task %X", frame->rax, frame->rbx, frame->rcx, frame->rdx, frame->rsi, (uint64_t)Scheduler::CurrentRunningTask);
    if (Scheduler::CurrentRunningTask == NULL)
        return;

    AddToStack();

    int syscall = frame->rax;
    frame->rax = 0;
    if (syscall == SYSCALL_GET_ARGC)
    {
        frame->rax = Scheduler::CurrentRunningTask->argC;
        Serial::TWritelnf("> Get argc %d", frame->rax);
    }
    else if (syscall == SYSCALL_GET_ARGV)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
        
        int argC = Scheduler::CurrentRunningTask->argC;
        char** argV = (char**)taskHeap->_Xmalloc(sizeof(const char*) * argC, "Malloc for argV");
        //Serial::Writelnf("> Malloced argV %X, (argc: %d, argv: %X)", argV, argC, Scheduler::CurrentRunningTask->argV);
        if (IsAddressValidForTask(argV))
            for (int i = 0; i < argC; i++)
            {
                const char* cV = Scheduler::CurrentRunningTask->argV[i];
                int len = StrLen(cV);
                //Serial::Writelnf("> ArgV %d: %s", i, cV);

                argV[i] = (char*)taskHeap->_Xmalloc(len + 1, "Malloc argV Str");
                //Serial::Writelnf("> Malloced argV %X", argV[i]);
                if (IsAddressValidForTask(argV[i]))
                {
                    //Serial::Writelnf("> Copying argV %X", argV[i]);
                    _memcpy((void*)cV, (void*)argV[i], len);
                    argV[i][len] = 0;
                }
            }

        frame->rax = (uint64_t)argV;
        Serial::TWritelnf("> Get argv %X, %X, (%X - %X), %X", argV, taskHeap, taskHeap->_heapStart, taskHeap->_heapEnd, Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_GET_ENV)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        if (!Scheduler::CurrentRunningTask->isKernelModule)
        {
            Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;

            Serial::TWritelnf("> TASK HEAP: %X, (%X - %X)", taskHeap, taskHeap->_heapStart, taskHeap->_heapEnd);

            ENV_DATA* env = (ENV_DATA*)taskHeap->_Xmalloc(sizeof(ENV_DATA), "Malloc for env");
            if (IsAddressValidForTask(env))
            {
                PSF1_FONT* font2 = (PSF1_FONT*)taskHeap->_Xmalloc(sizeof(PSF1_FONT), "Malloc for font");
                if (IsAddressValidForTask(font2))
                {
                    font2->psf1_Header = (PSF1_HEADER*)taskHeap->_Xmalloc(sizeof(PSF1_HEADER), "Malloc for font header");
                    if (IsAddressValidForTask(font2->psf1_Header))
                    {
                        *font2->psf1_Header = *GlobalRenderer->psf1_font->psf1_Header;

                        int amt = (font2->psf1_Header->mode + 1) * 256 * font2->psf1_Header->charsize;
                        font2->glyphBuffer = (void*)taskHeap->_Xmalloc(amt, "Malloc for font glyph buffer");
                        if (IsAddressValidForTask(font2->glyphBuffer))
                        {
                            _memcpy(GlobalRenderer->psf1_font->glyphBuffer, font2->glyphBuffer, amt);
                        }
                    }
                }
                env->globalFont = font2;//GlobalRenderer->psf1_font;
                env->globalFrameBuffer = NULL;//GlobalRenderer->framebuffer;
            }

            frame->rax = (uint64_t)env;
            Serial::TWritelnf("> Get env (user prog) %X", frame->rax);
        }
        else
        {
            Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;

            Serial::TWritelnf("> TASK HEAP: %X, (%X - %X)", taskHeap, taskHeap->_heapStart, taskHeap->_heapEnd);

            ENV_DATA* env = (ENV_DATA*)taskHeap->_Xmalloc(sizeof(ENV_DATA), "Malloc for env");
            if (IsAddressValidForTask(env))
            {
                env->globalFont = GlobalRenderer->psf1_font;
                env->globalFrameBuffer = GlobalRenderer->framebuffer;
            }

            frame->rax = (uint64_t)env;
            Serial::TWritelnf("> Get env (kernel module) %X", frame->rax);
        }
    }
    else if (syscall == SYSCALL_REQUEST_NEXT_PAGES)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        int pageCount = frame->rbx;
        osTask* task = Scheduler::CurrentRunningTask;
        PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);

        char* newAddr = (char*)((uint64_t)task->addrOfVirtPages + 0x1000 * task->requestedPages->GetCount());
        void* resAddr = (void*)newAddr;
        
        for (int i = 0; i < pageCount; i++)
        {
            void* tempPage = GlobalAllocator->RequestPage();
            task->requestedPages->Add(tempPage);
            
            manager.MapMemory((void*)newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
            GlobalPageTableManager.MapMemory((void*)newAddr, (void*)tempPage, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);

            newAddr += 0x1000;
        }

        frame->rax = (uint64_t)resAddr;
        Serial::TWritelnf("> Requested next %d pages to %X (Task %X)", pageCount, frame->rax, task->pid);
    }
    else if (syscall == SYSCALL_SERIAL_PRINT)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            Serial::Write(str);
        else
            Serial::TWritelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_SERIAL_PRINTLN)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            Serial::Writeln(str);
        else
            Serial::TWritelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_SERIAL_PRINT_CHAR)
    {
        char ch = (char)frame->rbx;
        Serial::Write(ch);
    }
    else if (syscall == SYSCALL_SERIAL_READ_CHAR)
    {
        char chr = 0;
        if (Serial::CanRead())
            chr = Serial::Read();
        
        frame->rax = chr;
    }
    else if (syscall == SYSCALL_SERIAL_CAN_READ_CHAR)
    {
        frame->rax = Serial::CanRead();
    }
    else if (syscall == SYSCALL_GLOBAL_PRINT)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        
        char* str = (char*)frame->rbx;
        //Serial::Writelnf("> Global Print %X \"%s\" (%X)", str, str, Scheduler::CurrentRunningTask);
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            GlobalRenderer->Print(str);
        else
            Serial::TWritelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINTLN)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        //Serial::Writelnf("> Global Println %X \"%s\" (%X)", str, str, Scheduler::CurrentRunningTask);

        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            GlobalRenderer->Println(str);
        else
            Serial::TWritelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
        //Serial::Writelnf("< Global Println %X", str);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINT_CHAR)
    {
        char ch = (char)frame->rbx;
        GlobalRenderer->Print(ch);
    }
    else if (syscall == SYSCALL_GLOBAL_CLS)
    {
        Serial::TWritelnf("> Clearing Screen");
        GlobalRenderer->Clear(Colors.black);
    }
    else if (syscall == SYSCALL_EXIT)
    {
        Serial::TWritelnf("> EXITING PROGRAM %d", frame->rbx);
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;

        // Scheduler::RemoveTask(Scheduler::CurrentRunningTask);
        // Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_CRASH)
    {
        Serial::TWritelnf("> EXITING PROGRAM bc it CRASHED");
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_YIELD)
    {
        if (Scheduler::CurrentRunningTask != Scheduler::NothingDoerTask)
            if (LOG_SCHED_STUFF)
                Serial::TWritelnf("> YIELDING TASK %X", Scheduler::CurrentRunningTask->pid);
        Scheduler::CurrentRunningTask->justYielded = true;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_WAIT)
    {
        //Serial::Writelnf("> WAITING PROGRAM %d ms", frame->rbx);
        Scheduler::CurrentRunningTask->taskTimeoutDone = PIT::TimeSinceBootMS() + frame->rbx;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_WAIT_MSG)
    {
        Scheduler::CurrentRunningTask->waitTillMessage = true;
        Scheduler::CurrentRunningTask->taskTimeoutDone = PIT::TimeSinceBootMS() + 500;
        Scheduler::CurrentRunningTask->justYielded = true;

        if (Scheduler::CurrentRunningTask->messages->GetCount() > 0)
            ;//Serial::TWritelnf("> TASK %X WAITING FOR MSG BUT ALREADY HAS %d", Scheduler::CurrentRunningTask->pid, Scheduler::CurrentRunningTask->messages->GetCount());

        if (LOG_SCHED_STUFF)
            Serial::TWritelnf("> MSG YIELDING TASK %X", Scheduler::CurrentRunningTask->pid);

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_SET_PRIORITY)
    {
        int prio = frame->rbx;
        if (prio < 0)
            prio = 0;

        #define BEST_USERMODE_PRIO 5

        // user space programs cant get a prio of 1 to (BEST_USERMODE_PRIO - 1)       
        if (prio != 0 && prio < BEST_USERMODE_PRIO && !Scheduler::CurrentRunningTask->isKernelModule)
            prio = BEST_USERMODE_PRIO;
    
        Serial::TWritelnf("> SETTING PRIORITY TO %d (wanted %d)", prio, frame->rbx);
        Scheduler::CurrentRunningTask->priority = prio;
        frame->rax = prio;
    }
    else if (syscall == SYSCALL_ENV_GET_TIME_MS)
    {
        frame->rax = PIT::TimeSinceBootMS();
    }
    else if (syscall == SYSCALL_ENV_GET_TIME_MICRO_S)
    {
        frame->rax = PIT::TimeSinceBootMicroS();
    }
    else if (syscall == SYSCALL_ENV_GET_TIME_RTC)
    {
        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;

        RTC_Info* info2 = (RTC_Info*)taskHeap->_Xmalloc(sizeof(RTC_Info), "Malloc for RTC_Info");
        if (IsAddressValidForTask(info2))
        {
            RTC_Info info;
            info.Day = RTC::Day;
            info.Month = RTC::Month;
            info.Year = RTC::Year;
            info.Hour = RTC::Hour;
            info.Minute = RTC::Minute;
            info.Second = RTC::Second;

            *info2 = info;
            frame->rax = (uint64_t)info2;
        }
    }
    else if (syscall == SYSCALL_ENV_GET_DESKTOP_PID)
    {
        if (Scheduler::DesktopTask != NULL)
            frame->rax = Scheduler::DesktopTask->pid;
        else
            frame->rax = 0;
    }
    else if (syscall == SYSCALL_ENV_GET_START_MENU_PID)
    {
        if (Scheduler::StartMenuTask != NULL)
            frame->rax = Scheduler::StartMenuTask->pid;
        else
            frame->rax = 0;
    }
    else if (syscall == SYSCALL_GET_PID)
    {
        frame->rax = Scheduler::CurrentRunningTask->pid;
    }
    else if (syscall == SYSCALL_GET_PARENT_PID)
    {
        frame->rax = Scheduler::CurrentRunningTask->parentPid;
    }
    else if (syscall == SYSCALL_GET_ELF_PATH)
    {
        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
        frame->rax = 0;

        const char* path = Scheduler::CurrentRunningTask->elfPath;
        char* path2 = (char*)taskHeap->_Xmalloc(StrLen(path) + 1, "Malloc for elf path");
        if (IsAddressValidForTask(path2))
        {
            _memcpy((void*)path, (void*)path2, StrLen(path));
            path2[StrLen(path)] = 0;
            frame->rax = (uint64_t)path2;
        }
    }
    else if (syscall == SYSCALL_GET_ELF_PATH_PID)
    {
        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
        frame->rax = 0;
        uint64_t pid = frame->rbx;

        osTask* task = Scheduler::GetTask(pid);
        if (task != NULL)
        {
            const char* path = task->elfPath;
            char* path2 = (char*)taskHeap->_Xmalloc(StrLen(path) + 1, "Malloc for elf path");
            if (IsAddressValidForTask(path2))
            {
                _memcpy((void*)path, (void*)path2, StrLen(path));
                path2[StrLen(path)] = 0;
                frame->rax = (uint64_t)path2;
            }
        }
    }
    else if (syscall == SYSCALL_GET_WORKING_PATH)
    {        
        Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
        frame->rax = 0;

        const char* path = Scheduler::CurrentRunningTask->startedAtPath;
        char* path2 = (char*)taskHeap->_Xmalloc(StrLen(path) + 1, "Malloc for working path");
        if (IsAddressValidForTask(path2))
        {
            _memcpy((void*)path, (void*)path2, StrLen(path));
            path2[StrLen(path)] = 0;
            frame->rax = (uint64_t)path2;
        }
    }
    else if (syscall == SYSCALL_RNG_UINT64)
    {
        frame->rax = RND::RandomInt();
    }
    else if (syscall == SYSCALL_LAUNCH_TEST_ELF_USER)
    {
        Serial::TWritelnf("> Launching User Test Elf");
        Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)Scheduler::TestElfFile);
        if (!elf.works)
            Panic("FILE NO WORK :(", true);

        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, true, "bruh:programs/test/test.elf", "");
        task->parentPid = Scheduler::CurrentRunningTask->pid;

        //osTask* task = Scheduler::CreateTaskFromElf(Scheduler::testElfFile, 0, NULL, true);
        //task->active = false;
        Scheduler::AddTask(task);
    }
    else if (syscall == SYSCALL_LAUNCH_TEST_ELF_KERNEL)
    {
        Serial::TWritelnf("> Launching Kernel Test Elf");
        Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)Scheduler::TestElfFile);
        if (!elf.works)
            Panic("FILE NO WORK :(", true);

        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false, "bruh:modules/test/test.elf", "");
        task->parentPid = Scheduler::CurrentRunningTask->pid;

        //osTask* task = Scheduler::CreateTaskFromElf(Scheduler::testElfFile, 0, NULL, true);
        //task->active = false;
        Scheduler::AddTask(task);
    }
    else if (syscall == SYSCALL_PID_EXISTS)
    {
        uint64_t pid = frame->rbx;
        
        bool exists = Scheduler::GetTask(pid) != NULL;

        frame->rax = exists;
    }
    // else if (syscall == SYSCALL_ENV_GET_MOUSE_STATE)
    // {
    //     Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;

    //     MouseState* packet = (MouseState*)taskHeap->_Xmalloc(sizeof(MouseState), "Malloc for mouse state");

    //     if (IsAddressValidForTask(packet))
    //     {
    //         packet->MouseX = Mouse::MousePosition.x;
    //         packet->MouseY = Mouse::MousePosition.y;
    //         packet->Left = Mouse::clicks[0];
    //         packet->Right = Mouse::clicks[1];
    //         packet->Middle = Mouse::clicks[2];
    //     }

    //     frame->rax = (uint64_t)packet;
    // }
    else if (syscall == SYSCALL_ENV_GET_KEY_STATE)
    {
        int scancode = frame->rbx;
        frame->rax = Keyboard::IsKeyPressed(scancode);
    }
    else if (syscall == SYSCALL_ENV_GET_MOUSE_SENS)
    {
        frame->rax = osData.mouseSensitivity;
    }
    else if (syscall == SYSCALL_ENV_SET_MOUSE_SENS)
    {
        int mouseSens = frame->rbx;
        if (mouseSens < 10)
            mouseSens = 10;
        if (mouseSens > 1000)
            mouseSens = 1000;
        osData.mouseSensitivity = mouseSens;
        frame->rax = osData.mouseSensitivity;
    }
    else if (syscall == SYSCALL_MSG_GET_COUNT)
    {
        Queue<GenericMessagePacket*>* queue = Scheduler::CurrentRunningTask->messages;
        frame->rax = 0;
        if (queue != NULL)
            frame->rax = queue->GetCount();
    }
    else if (syscall == SYSCALL_MSG_GET_MSG)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        
        Queue<GenericMessagePacket*>* queue = Scheduler::CurrentRunningTask->messages;
        frame->rax = 0;
        if (queue != NULL && queue->GetCount() > 0)
        {
            Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
            GenericMessagePacket* oldPacket = queue->Dequeue();
            if (oldPacket != NULL && taskHeap != NULL)
            {
                GenericMessagePacket* newPacket = oldPacket->Copy(taskHeap);
                oldPacket->Free();
                _Free(oldPacket);
                frame->rax = (uint64_t)newPacket;
            }
        }
    }
    else if (syscall == SYSCALL_MSG_GET_MSG_CONVO)
    {
        Queue<GenericMessagePacket*>* queue = Scheduler::CurrentRunningTask->messages;
        frame->rax = 0;
        uint64_t convoId = frame->rbx;
        if (queue != NULL && queue->GetCount() > 0)
        {
            _tempGenericMessageConvoId = convoId;
            GenericMessagePacket** oldPacketPtr = queue->First(_tempGenericMessageChecker);

            if (oldPacketPtr != NULL)
            {
                AddToStack();
                GenericMessagePacket* oldPacket = *oldPacketPtr;
                queue->Remove(oldPacketPtr);
                Heap::HeapManager* taskHeap = (Heap::HeapManager*)Scheduler::CurrentRunningTask->addrOfVirtPages;
                if (oldPacket != NULL && taskHeap != NULL)
                {
                    AddToStack();
                    //Serial::Writelnf("> %X: %d msgs left.", Scheduler::CurrentRunningTask, queue->GetCount());
                    GenericMessagePacket* newPacket = oldPacket->Copy(taskHeap);
                    oldPacket->Free();
                    _Free(oldPacket);
                    frame->rax = (uint64_t)newPacket;
                    RemoveFromStack();
                }
                RemoveFromStack();
            }
        }
    }
    else if (syscall == SYSCALL_MSG_SEND_MSG)
    {
        //MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        frame->rax = 0;
        GenericMessagePacket* oldPacket = (GenericMessagePacket*)frame->rbx;
        if (IsAddressValidForTask(oldPacket, Scheduler::CurrentRunningTask))
        {
            osTask* task = Scheduler::CurrentRunningTask;
            if (task != NULL)
            {
                uint64_t targetPid = frame->rcx;
                osTask* otherTask = Scheduler::GetTask(targetPid);
                bool allowSend = true;

                if (allowSend && otherTask == NULL)
                    allowSend = false;

                if (allowSend && (oldPacket->Type == MessagePacketType::KEY_EVENT && !task->isKernelModule))
                    allowSend = false;

                if (allowSend && (oldPacket->Type == MessagePacketType::MOUSE_EVENT && !task->isKernelModule))
                    allowSend = false;
                
                if (allowSend)
                {
                    // Serial::Writelnf("> Sending %d from %X to %X", oldPacket->Size, task->pid, otherTask->pid);
                    GenericMessagePacket* newPacket = oldPacket->Copy();
                    newPacket->FromPID = task->pid;
                    otherTask->messages->Enqueue(newPacket);
                    frame->rax = 1;
                }
            }
        }
    }
    else if (syscall >= 700 && syscall <= 999)
    {
        FS_Syscall_handler(syscall, frame);
    }
    else if (syscall == SYSCALL_CLOSE_PROCESS)
    {
        uint64_t pid = frame->rbx;
        osTask* task = Scheduler::GetTask(pid);

        // add checks if the process belongs to the current process
        if (task != NULL)
        {
            task->removeMe = true;
            frame->rax = 1;
        }
        else
            frame->rax = 0;
    }
    else if (syscall == SYSCALL_START_PROCESS)
    {
        // asm("int $0x31" : "=a"(pid) : "a"(syscall), "b"(path), "c"(argc), "d"(argv));
        frame->rax = 0;
        const char* path = (const char*)frame->rbx;
        int argc = frame->rcx;
        const char** argv = (const char**)frame->rdx;
        const char* wPath = (const char*)frame->rsi;

        if (IsAddressValidForTask(path) && (IsAddressValidForTask(argv) || argc == 0) && IsAddressValidForTask(wPath))
        {
            bool ok = true;
            for (int i = 0; i < argc; i++)
                if (!IsAddressValidForTask(argv[i]))
                    ok = false;
            
            if (ok)
            {
                char* resBuffer = NULL;
                int resLen = 0;
                if (FS_STUFF::ReadFileFromFullPath(path, &resBuffer, &resLen))
                {
                    Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)resBuffer);
                    if (elf.works)
                    {
                        osTask* task = Scheduler::CreateTaskFromElf(elf, argc, argv, true, path, wPath);
                        task->parentPid = Scheduler::CurrentRunningTask->pid;
                        Scheduler::AddTask(task);
                        frame->rax = task->pid;
                    }
                    else
                        Serial::TWritelnf("> Elf file %s is invalid", path);
                    _Free(resBuffer);
                }
                else
                    Serial::TWritelnf("> File %s does not exist", path);
                {

                }
            }
        }
    }
    else if (syscall == SYSCALL_START_FILE)
    {
        frame->rax = 0;
        const char* path = (const char*)frame->rbx;
        const char* wPath = (const char*)frame->rcx;

        if (IsAddressValidForTask(path) && IsAddressValidForTask(wPath))
        {
            const char* actualPath = NULL;
            if (StrEndsWith(path, ".txt"))
                actualPath = "bruh:programs/notepad/notepad.elf";
            if (StrEndsWith(path, ".maab"))
                actualPath = "bruh:programs/maab/maab.elf";
            if (StrEndsWith(path, ".elf"))
                actualPath = "bruh:modules/elfLauncher/elfLauncher.elf";
            if (StrEndsWith(path, ".mbif"))
                actualPath = "bruh:programs/imgView/imgView.elf";

            if (actualPath != NULL)
            {
                const char** tempArgV = (const char**)_Malloc(sizeof(const char**));
                tempArgV[0] = StrCopy(path);

                char* resBuffer = NULL;
                int resLen = 0;
                if (FS_STUFF::ReadFileFromFullPath(actualPath, &resBuffer, &resLen))
                {
                    Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)resBuffer);
                    if (elf.works)
                    {
                        osTask* task = Scheduler::CreateTaskFromElf(elf, 1, tempArgV, true, actualPath, wPath);
                        task->parentPid = Scheduler::CurrentRunningTask->pid;
                        Scheduler::AddTask(task);
                        frame->rax = task->pid;
                    }
                    else
                        Serial::TWritelnf("> Elf file %s is invalid", path);
                    _Free(resBuffer);
                }
                else
                    Serial::TWritelnf("> File %s does not exist", path);

                _Free(tempArgV[0]);
                _Free(tempArgV);
            }
        }
    }
    else if (syscall == SYSCALL_START_THREAD)
    {
        void* func = (void*)frame->rbx;
        osTask* nTask = Scheduler::CreateThreadFromTask(Scheduler::CurrentRunningTask, func);
        Scheduler::AddTask(nTask);
        frame->rax = nTask->pid;
    }
    else if (syscall == SYSCALL_AUDIO_SETUP_BUFFER)
    {
        osTask* task = Scheduler::CurrentRunningTask;
        
        if (task->audioOutput != NULL)
        {
            task->audioOutput->Destroy();
            task->audioOutput = NULL;
        }

        int sampleRate = frame->rbx;
        int sampleCount = frame->rcx;
        int bitsPerSample = frame->rdx;
        int channelCount = frame->rsi;

        if (sampleRate < 1000 || sampleRate > 1000000 || 
            sampleCount < 1 || sampleCount > 1000000 || 
            bitsPerSample < 8 || bitsPerSample > 32 ||
            channelCount < 1 || channelCount > 2)
        {
            Serial::TWritelnf("> Setup audio buffer for task %X failed! (Sample Rate: %d Hz, Sample Count: %d, Channel Count: %d, Bits per sample: %d)", task->pid, sampleRate, sampleCount, channelCount, bitsPerSample);
            frame->rax = false;
        }
        else
        {
            task->audioOutput = new Audio::BasicAudioSource(
                new Audio::AudioBuffer(bitsPerSample, sampleRate, channelCount, sampleCount)
            );

            task->audioOutput->ConnectTo(osData.defaultAudioOutputDevice->destination);
            Serial::TWritelnf("> Setup audio buffer for task %X succeded! (Sample Rate: %d Hz, Sample Count: %d, Channel Count: %d, Bits per sample: %d)", task->pid, sampleRate, sampleCount, channelCount, bitsPerSample);

            frame->rax = true;
        }
    }
    else if (syscall == SYSCALL_AUDIO_SEND_DATA)
    {
        osTask* task = Scheduler::CurrentRunningTask;
        
        if (task->audioOutput != NULL)
        {
            void* data = (void*)frame->rbx;
            uint64_t byteCount = frame->rcx;
            int sampleCount = frame->rdx;
            Audio::BasicAudioSource* source = task->audioOutput;
            Serial::TWritelnf("> SYSCALL send audio data for task %X, %d bytes", task->pid, byteCount);

            if (IsAddressValidForTask(data, task) && IsAddressValidForTask((char*)data + byteCount, task))
            {
                if (byteCount == source->buffer->byteCount)
                {
                    // Clear the buffer
                    // source->buffer->ClearBuffer();
                    
                    if (sampleCount < 0)
                        sampleCount = 0;
                    if (sampleCount > source->buffer->totalSampleCount)
                        sampleCount = source->buffer->totalSampleCount;

                    // Copy the data
                    _memcpy(data, source->buffer->data, byteCount);
                    source->buffer->sampleCount = sampleCount;
                    source->samplesSent = 0;
                    source->readyToSend = true;


                    frame->rax = true;
                }
                else
                {
                    frame->rax = false;
                    Serial::TWritelnf("> SYSCALL send audio data for task %X failed! (Byte count mismatch: %d != %d)", task->pid, byteCount, source->buffer->byteCount);
                }
            }
            else
            {
                frame->rax = false;
                Serial::TWritelnf("> SYSCALL send audio data for task %X failed! (Invalid address)", task->pid);
            }
        }
        else
        {
            frame->rax = false;
            Serial::TWritelnf("> SYSCALL send audio data for task %X failed! (No audio buffer)", task->pid);
        }
    }
    else if (syscall == SYSCALL_AUDIO_DATA_NEEDED)
    {
        osTask* task = Scheduler::CurrentRunningTask;
        if (task->audioOutput != NULL)
        {
            Audio::BasicAudioSource* source = task->audioOutput;
            frame->rax = !source->readyToSend;// || source->samplesSent >= source->buffer->sampleCount;
        }
        else
        {
            frame->rax = 0;
        }
    }
    else
    {
        Serial::TWritelnf("> Unknown Syscall: %d", syscall);
    }
    //Scheduler::SchedulerInterrupt(frame);


    RemoveFromStack();
}
