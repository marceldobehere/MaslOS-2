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
    RemoveFromStack();
}

bool speakA = false;

//#include "../audio/audioDevStuff.h"

#include "../paging/PageTableManager.h"
#include "../devices/rtc/rtc.h"
#include "../memory/heap.h"

int _usedHeapCount = 0;
int _usedPages = 0;

int _pitCount = 0;

void TempPitRoutine(interrupt_frame* frame)
{
    //GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);

    AddToStack();
    PIT::Tick();
    PIC_EndMaster();

    

    // AudioDeviceStuff::play(PIT::FreqAdder);
    // if (osData.serialManager != NULL)
    //     osData.serialManager->DoStuff();


    if (_pitCount++ >= 80)   
    {
        _pitCount = 0;
        
        Point tempPoint = GlobalRenderer->CursorPosition;
        GlobalRenderer->CursorPosition.x = 0;
        GlobalRenderer->CursorPosition.y = GlobalRenderer->framebuffer->Height - 16;

        GlobalRenderer->Clear(0, GlobalRenderer->CursorPosition.y, GlobalRenderer->framebuffer->Width - 1, GlobalRenderer->CursorPosition.y + 15, Colors.black);

        uint32_t currCol = 0;
        
        currCol = Colors.orange;
        GlobalRenderer->Print("DATE: ", currCol);
        GlobalRenderer->Print("{}.", to_string((int)RTC::Day), currCol);
        GlobalRenderer->Print("{}.", to_string((int)RTC::Month), currCol);
        GlobalRenderer->Print("{}", to_string((int)RTC::Year), currCol);
        GlobalRenderer->Print(" - ", Colors.white);

        currCol = Colors.yellow;
        GlobalRenderer->Print("{}:", to_string((int)RTC::Hour), currCol);
        GlobalRenderer->Print("{}:", to_string((int)RTC::Minute), currCol);
        GlobalRenderer->Print("{}", to_string((int)RTC::Second), currCol);
        GlobalRenderer->Print(" - ", Colors.white);

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
            Serial::Writelnf("MEM> Malloced %d times", mallocCount);
        if (freeCount > 0 && PRINT_MEM_STATS_TO_SERIAL)
            Serial::Writelnf("MEM> Freed %d times", freeCount);

        freeCount = 0;
        mallocCount = 0;
        
        GlobalRenderer->CursorPosition = tempPoint;

        if (usedHeapCount != _usedHeapCount && PRINT_MEM_STATS_TO_SERIAL)
        {
            _usedHeapCount = usedHeapCount;
            Serial::Writelnf("MEM> Used Heap Count: %d", usedHeapCount);
            Serial::Writelnf("MEM> Used Heap Amount: %d", usedHeapAmount);
        }

        if (GlobalAllocator->GetUsedRAM() / 0x1000 != _usedPages && PRINT_MEM_STATS_TO_SERIAL)
        {
            _usedPages = GlobalAllocator->GetUsedRAM() / 0x1000;

            Serial::Writelnf("MEM> Used Pages: %d", _usedPages);
        }

        // TestSetSpeakerPosition(speakA);
        // speakA = !speakA;
    }


    RemoveFromStack();

    Scheduler::SchedulerInterrupt(frame);

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

void MapMemoryOfCurrentTask(osTask* task)
{
    if (task == NULL)
        return;
    
    if (task->pageTableContext == NULL)
        return;

    if (currentMappedTask == task)
        return;
    currentMappedTask = task;

    //GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);

    Serial::Writelnf("INT> Mapping %d pages of task %X", task->requestedPages->GetCount(), (uint64_t)task);
    //PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);

    // we map the requested pages into the global space so we can access em rn
    for (int i = 0; i < task->requestedPages->GetCount(); i++)
    {
        void* realPageAddr = task->requestedPages->ElementAt(i);
        void* virtPageAddr = (void*)(MEM_AREA_USER_PROGRAM_REQUEST_START + 0x1000 * i);
        //Serial::Writelnf("    > Mapping %X to %X", (uint64_t)realPageAddr, (uint64_t)virtPageAddr);
        GlobalPageTableManager.MapMemory(virtPageAddr, realPageAddr, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
        //manager.MapMemory(virtPageAddr, (void*)realPageAddr, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
    }
}

bool SendMessageToTask(GenericMessagePacket* oldPacket, uint64_t targetPid, uint64_t sourcePid)
{
    if (oldPacket == NULL)
        return false;
    
    osTask* otherTask = Scheduler::GetTask(targetPid);

    if (otherTask == NULL)
        return false;

    GenericMessagePacket* newPacket = oldPacket->Copy();
    otherTask->messages->Enqueue(newPacket);
    return true;
}

#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/mousePacket/mousePacket.h>

bool InterruptGoingOn = false;
int currentInterruptCount = 0;

extern "C" void intr_common_handler_c(interrupt_frame* frame) 
{
    //asm volatile("mov %0, %%cr3" : : "r"((uint64_t)GlobalPageTableManager.PML4) : "memory");
    GlobalPageTableManager.SwitchPageTable(GlobalPageTableManager.PML4);
    
    //Serial::Writelnf("INT> INT %d, (%X, %X)", frame->interrupt_number, frame->cr3, frame->cr0);

    AddToStack();
    
    

    if (InterruptGoingOn)
    {
        Serial::Writelnf("WAAAA> INT %d IS INTERRUPTING INT!", frame->interrupt_number);
        //Panic("INT IN INT", true);
        
        for (int i = 0; i < 20; i++)
            GlobalRenderer->ClearButDont();

        //return;
    }
    InterruptGoingOn = true;

    int rnd = RND::RandomInt();
    
    //Panic("WAAAAAAAAA {}", to_string(regs->interrupt_number), true);
    if (frame->interrupt_number == 32)
        TempPitRoutine(frame);
    else if (frame->interrupt_number == 0x31)
        Syscall_handler(frame);
    else if (frame->interrupt_number == 254)
        Serial::Writelnf("> Generic Interrupt");
    else
    {
        currentInterruptCount++;
        Serial::Writelnf("> Interrupt/Exception: %d (Count %d) -> Closing Task...", frame->interrupt_number, currentInterruptCount);
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

        if (Scheduler::CurrentRunningTask != NULL)
        {
            Serial::Writeln("Task info:");
            osTask* task = Scheduler::CurrentRunningTask;
            Serial::Writelnf("Task: %X", (uint64_t)task);
            Serial::Writelnf("Task->kernelStack: %X", (uint64_t)task->kernelStack);
            Serial::Writelnf("Task->userStack: %X", (uint64_t)task->userStack);
            Serial::Writelnf("Task->pageTableContext: %X", (uint64_t)task->pageTableContext);
            Serial::Writelnf("Task->frame: %X", (uint64_t)task->frame);
            
            Serial::Writelnf("Task->requestedPages: %d", task->requestedPages->GetCount());
            Serial::Writelnf("Task->Priority: %d/%d", task->priorityStep, task->priority);
            Serial::Writelnf("Task->Timeout: %d", task->taskTimeoutDone);

            Serial::Writelnf("Task->removeMe: %B", task->removeMe);
            Serial::Writelnf("Task->active: %B", task->active);
            Serial::Writelnf("Task->doExit: %B", task->doExit);
            Serial::Writelnf("Task->isUserMode: %B", !task->isKernelModule);
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

        //Serial::Writelnf("> END OF INT (%X, %X)", frame->cr3, frame->cr0);
        InterruptGoingOn = false;
        Scheduler::SchedulerInterrupt(frame);
        //frame.cr3 = (uint64_t)Scheduler::CurrentRunningTask->pageTableContext;
        // if (Scheduler::CurrentRunningTask != NULL)
        //     MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        RemoveFromStack();
        return;
    }


    if (Scheduler::DesktopTask != NULL && !Scheduler::DesktopTask->removeMe)
    {
        int keysToDo = min(50, Keyboard::KeysAvaiable());
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

        int mouseToDo = min(50, Mouse::MousePacketsAvailable());
        for (int i = 0; i < mouseToDo; i++)
        {
            MousePacket mPacket = Mouse::mousePackets->Dequeue();
            //Serial::Writelnf("INT> Doing mouse packet");
            Mouse::MiniMousePacket packet = Mouse::ProcessMousePacket(mPacket);
            if (!packet.Valid)
                continue;


            // TODO: add hold and release stuff here
            MouseMessagePacket mousePacket = MouseMessagePacket(packet.X, packet.Y);
            
            GenericMessagePacket* packet2 = new GenericMessagePacket(
                MessagePacketType::MOUSE_EVENT,
                (uint8_t*)&mousePacket,
                sizeof(MouseMessagePacket)
            );
            
            //Serial::Writelnf("INT> Sending mouse packet to desktop task");
            SendMessageToTask(packet2, Scheduler::DesktopTask->pid, 1);
            
            packet2->Free();
            _Free(packet2);
        }
    }
    

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
    Serial::Writelnf("> PROGRAM REACHED END");
    if (Scheduler::CurrentRunningTask != NULL)
    {
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;
    }
    // Scheduler::RemoveTask();
    // Scheduler::CurrentRunningTask = NULL;
}

#include <libm/syscallList.h>

bool IsAddressValidForTask(void* addr, osTask* task)
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

    if (addr >= (void*)MEM_AREA_USER_PROGRAM_REQUEST_START && addr < (void*)(MEM_AREA_USER_PROGRAM_REQUEST_START + 0x1000 * task->requestedPages->GetCount()))
        return true;

    if (addr >= task->elfFile.offset && addr < (char*)task->elfFile.offset + task->elfFile.size * 0x1000)
        return true;

    if (task->isKernelModule)
        return true;

    return false;
}

#include <libm/heap/heap.h>
#include <libm/cstrTools.h>

void Syscall_handler(interrupt_frame* frame)
{
    //Serial::Writelnf("> Syscall: %d, task %X", frame->rax, (uint64_t)Scheduler::CurrentRunningTask);
    if (Scheduler::CurrentRunningTask == NULL)
        return;

    AddToStack();

    int syscall = frame->rax;
    frame->rax = 0;
    if (syscall == SYSCALL_GET_ARGC)
    {
        frame->rax = Scheduler::CurrentRunningTask->argC;
        Serial::Writelnf("> Get argc %d", frame->rax);
    }
    else if (syscall == SYSCALL_GET_ARGV)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        Heap::HeapManager* taskHeap = MEM_AREA_USER_PROGRAM_HEAP;
        
        int argC = Scheduler::CurrentRunningTask->argC;
        char** argV = (char**)taskHeap->_Xmalloc(sizeof(const char*) * argC, "Malloc for argV");
        //Serial::Writelnf("> Malloced argV %X, (argc: %d, argv: %X)", argV, argC, Scheduler::CurrentRunningTask->argV);
        if (argV != NULL)
            for (int i = 0; i < argC; i++)
            {
                const char* cV = Scheduler::CurrentRunningTask->argV[i];
                int len = StrLen(cV);
                //Serial::Writelnf("> ArgV %d: %s", i, cV);

                argV[i] = (char*)taskHeap->_Xmalloc(len + 1, "Malloc argV Str");
                //Serial::Writelnf("> Malloced argV %X", argV[i]);
                if (argV[i] != NULL)
                {
                    //Serial::Writelnf("> Copying argV %X", argV[i]);
                    _memcpy((void*)cV, (void*)argV[i], len);
                    argV[i][len] = 0;
                }
            }

        frame->rax = (uint64_t)argV;
        Serial::Writelnf("> Get argv %X, %X, (%X - %X), %X", argV, taskHeap, taskHeap->_heapStart, taskHeap->_heapEnd, Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_GET_ENV)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        if (!Scheduler::CurrentRunningTask->isKernelModule)
        {
            frame->rax = 0;
            Serial::Writelnf("> Get env (userspace prg) %X", frame->rax);
        }
        else
        {
            Heap::HeapManager* taskHeap = MEM_AREA_USER_PROGRAM_HEAP;

            Serial::Writelnf("> TASK HEAP: %X, (%X - %X)", taskHeap, taskHeap->_heapStart, taskHeap->_heapEnd);

            ENV_DATA* env = (ENV_DATA*)taskHeap->_Xmalloc(sizeof(ENV_DATA), "Malloc for env");
            if (env != NULL)
            {
                env->globalFont = GlobalRenderer->psf1_font;
                env->globalFrameBuffer = GlobalRenderer->framebuffer;
            }
            frame->rax = (uint64_t)env;
            Serial::Writelnf("> Get env (kernel module) %X", frame->rax);
        }
    }
    else if (syscall == SYSCALL_REQUEST_NEXT_PAGES)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        int pageCount = frame->rbx;
        osTask* task = Scheduler::CurrentRunningTask;
        PageTableManager manager = PageTableManager((PageTable*)task->pageTableContext);

        char* newAddr = (char*)(MEM_AREA_USER_PROGRAM_REQUEST_START + 0x1000 * task->requestedPages->GetCount());
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
        Serial::Writelnf("> Requested next %d pages to %X", pageCount, frame->rax);
    }
    else if (syscall == SYSCALL_SERIAL_PRINT)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            Serial::Write(str);
        else
            Serial::Writelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_SERIAL_PRINTLN)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            Serial::Writeln(str);
        else
            Serial::Writelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if(syscall ==SYSCALL_SERIAL_PRINT_CHAR)
    {
        char ch = (char)frame->rbx;
        Serial::Write(ch);
    }
    else if(syscall == SYSCALL_SERIAL_READ_CHAR)
    {
        char chr = 0;
        if (Serial::CanRead())
            chr = Serial::Read();
        
        frame->rax = chr;
    }
    else if(syscall == SYSCALL_SERIAL_CAN_READ_CHAR)
    {
        frame->rax = Serial::CanRead();
    }
    else if (syscall == SYSCALL_GLOBAL_PRINT)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        
        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            GlobalRenderer->Print(str);
        else
            Serial::Writelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINTLN)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        char* str = (char*)frame->rbx;
        if (IsAddressValidForTask(str, Scheduler::CurrentRunningTask))
            GlobalRenderer->Println(str);
        else
            Serial::Writelnf("> Invalid address (%X) for task %X", (uint64_t)str, (uint64_t)Scheduler::CurrentRunningTask);
    }
    else if (syscall == SYSCALL_GLOBAL_PRINT_CHAR)
    {
        char ch = (char)frame->rbx;
        GlobalRenderer->Print(ch);
    }
    else if (syscall == SYSCALL_GLOBAL_CLS)
    {
        Serial::Writelnf("> Clearing Screen");
        GlobalRenderer->Clear(Colors.black);
    }
    else if (syscall == SYSCALL_EXIT)
    {
        Serial::Writelnf("> EXITING PROGRAM %d", frame->rbx);
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;

        // Scheduler::RemoveTask(Scheduler::CurrentRunningTask);
        // Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_CRASH)
    {
        Serial::Writelnf("> EXITING PROGRAM bc it CRASHED");
        Scheduler::CurrentRunningTask->removeMe = true;
        Scheduler::CurrentRunningTask = NULL;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_YIELD)
    {
        if (Scheduler::CurrentRunningTask != Scheduler::NothingDoerTask)
            ;//Serial::Writelnf("> YIELDING TASK %X", Scheduler::CurrentRunningTask);
        Scheduler::CurrentRunningTask->justYielded = true;

        Scheduler::SchedulerInterrupt(frame);
    }
    else if (syscall == SYSCALL_WAIT)
    {
        Serial::Writelnf("> WAITING PROGRAM %d ms", frame->rbx);
        Scheduler::CurrentRunningTask->taskTimeoutDone = PIT::TimeSinceBootMS() + frame->rbx;

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
    
        Serial::Writelnf("> SETTING PRIORITY TO %d (wanted %d)", prio, frame->rbx);
        Scheduler::CurrentRunningTask->priority = prio;
        frame->rax = prio;
    }
    else if (syscall == SYSCALL_ENV_GET_TIME_MS)
    {
        frame->rax = PIT::TimeSinceBootMS();
    }
    else if (syscall == SYSCALL_ENV_GET_DESKTOP_PID)
    {
        if (Scheduler::DesktopTask != NULL)
            frame->rax = Scheduler::DesktopTask->pid;
        else
            frame->rax = 0;
    }
    else if (syscall == SYSCALL_GET_PID)
    {
        frame->rax = Scheduler::CurrentRunningTask->pid;
    }
    else if (syscall == SYSCALL_RNG_UINT64)
    {
        frame->rax = RND::RandomInt();
    }
    else if (syscall == SYSCALL_LAUNCH_TEST_ELF_USER)
    {
        Serial::Writelnf("> Launching User Test Elf");
        Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)Scheduler::TestElfFile);
        if (!elf.works)
            Panic("FILE NO WORK :(", true);

        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, true);

        //osTask* task = Scheduler::CreateTaskFromElf(Scheduler::testElfFile, 0, NULL, true);
        //task->active = false;
        Scheduler::AddTask(task);
    }
    else if (syscall == SYSCALL_LAUNCH_TEST_ELF_KERNEL)
    {
        Serial::Writelnf("> Launching Kernel Test Elf");
        Elf::LoadedElfFile elf = Elf::LoadElf((uint8_t*)Scheduler::TestElfFile);
        if (!elf.works)
            Panic("FILE NO WORK :(", true);

        osTask* task = Scheduler::CreateTaskFromElf(elf, 0, NULL, false);

        //osTask* task = Scheduler::CreateTaskFromElf(Scheduler::testElfFile, 0, NULL, true);
        //task->active = false;
        Scheduler::AddTask(task);
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
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);
        
        Queue<GenericMessagePacket*>* queue = Scheduler::CurrentRunningTask->messages;
        frame->rax = 0;
        if (queue != NULL && queue->GetCount() > 0)
        {
            Heap::HeapManager* taskHeap = MEM_AREA_USER_PROGRAM_HEAP;
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
    else if (syscall == SYSCALL_MSG_SEND_MSG)
    {
        MapMemoryOfCurrentTask(Scheduler::CurrentRunningTask);

        frame->rax = 0;
        GenericMessagePacket* oldPacket = (GenericMessagePacket*)frame->rbx;
        if (oldPacket != NULL)
        {
            osTask* task = Scheduler::CurrentRunningTask;
            if (task != NULL)
            {
                uint64_t targetPid = frame->rcx;
                osTask* otherTask = Scheduler::GetTask(targetPid);
                bool allowSend = true;

                if (allowSend && otherTask == NULL)
                    allowSend = false;

                if (allowSend && oldPacket->Type == MessagePacketType::KEY_EVENT && !task->isKernelModule)
                    allowSend = false;

                if (allowSend && oldPacket->Type == MessagePacketType::MOUSE_EVENT && !task->isKernelModule)
                    allowSend = false;
                
                if (allowSend)
                {
                    GenericMessagePacket* newPacket = oldPacket->Copy();
                    newPacket->FromPID = task->pid;
                    otherTask->messages->Enqueue(newPacket);
                    frame->rax = 1;
                }
            }
        }
    }
    else
    {
        Serial::Writelnf("> Unknown Syscall: %d", syscall);
    }
    //Scheduler::SchedulerInterrupt(frame);


    RemoveFromStack();
}
