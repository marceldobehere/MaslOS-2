#include "panic.h"
#include "../rendering/BasicRenderer.h"
#include <libm/rendering/Cols.h>
#include "../osData/osData.h"
#include "../osData/MStack/MStackM.h"
//#include "../WindowStuff/SubInstances/customInstance/customInstance.h"
#include "..//devices/serial/serial.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/interrupt_frame.h>

void PrintReg(BasicRenderer* renderer, const char* reg, uint64_t val)
{
    renderer->Print(reg);
    renderer->Print(": ");
    renderer->Print("0x");
    renderer->Print(ConvertHexToString(val));
    renderer->Print("  ");

    Serial::Write(reg);
    Serial::Write(": ");
    Serial::Write("0x");
    Serial::Write(ConvertHexToString(val));
    Serial::Write(" ");
}

void PrintLn(BasicRenderer* renderer)
{
    renderer->Println();
    Serial::Writeln();
}

void PrintLn(BasicRenderer* renderer, const char* str)
{
    renderer->Println(str);
    Serial::Writeln(str);   
}

void PrintTaskRegisterDump(void* _renderer, void* _frame)
{
    BasicRenderer* renderer = (BasicRenderer*)_renderer;
    interrupt_frame* frame = (interrupt_frame*)_frame;

    PrintLn(renderer);
    PrintLn(renderer, "Register dump (TASK): ");
    PrintLn(renderer);

    PrintReg(renderer, "rax", frame->rax);
    PrintReg(renderer, "rcx", frame->rcx);
    PrintReg(renderer, "rdx", frame->rdx);
    PrintReg(renderer, "rbx", frame->rbx);
    PrintLn(renderer);

    PrintReg(renderer, "rsp", frame->rsp);
    PrintReg(renderer, "rbp", frame->rbp);
    PrintReg(renderer, "rsi", frame->rsi);
    PrintReg(renderer, "rdi", frame->rdi);
    PrintLn(renderer);

    PrintReg(renderer, " r8", frame->r8);
    PrintReg(renderer, " r9", frame->r9);
    PrintReg(renderer, "r10", frame->r10);
    PrintReg(renderer, "r11", frame->r11);
    PrintLn(renderer);

    PrintReg(renderer, "r12", frame->r12);
    PrintReg(renderer, "r13", frame->r13);
    PrintReg(renderer, "r14", frame->r14);
    PrintReg(renderer, "r15", frame->r15);
    PrintLn(renderer);

    PrintReg(renderer, "rip", frame->rip);
    PrintReg(renderer, " cs", frame->cs);
    PrintReg(renderer, "rsp", frame->rsp);
    PrintReg(renderer, " ss", frame->ss);
    PrintReg(renderer, "rflags", frame->rflags);
    PrintLn(renderer);

    PrintReg(renderer, "cr0", frame->cr0);
    PrintReg(renderer, "cr2", frame->cr2);
    PrintReg(renderer, "cr3", frame->cr3);
    PrintReg(renderer, "cr4", frame->cr4);
    PrintLn(renderer);
}

void PrintRegisterDump(void* _renderer)
{
    BasicRenderer* renderer = (BasicRenderer*)_renderer;
    uint64_t Register = 0;

    PrintLn(renderer);
    PrintLn(renderer, "Register dump: ");
    PrintLn(renderer);

    asm volatile("mov %%rax, %0" : "=r"(Register));
    PrintReg(renderer, "rax", Register);
    asm volatile("mov %%rcx, %0" : "=r"(Register));
    PrintReg(renderer, "rcx", Register);
    asm volatile("mov %%rdx, %0" : "=r"(Register));
    PrintReg(renderer, "rdx", Register);
    asm volatile("mov %%rbx, %0" : "=r"(Register));
    PrintReg(renderer, "rbx", Register);
    PrintLn(renderer);

    asm volatile("mov %%rsp, %0" : "=r"(Register));
    PrintReg(renderer, "rsp", Register);
    asm volatile("mov %%rbp, %0" : "=r"(Register));
    PrintReg(renderer, "rbp", Register);
    asm volatile("mov %%rsi, %0" : "=r"(Register));
    PrintReg(renderer, "rsi", Register);
    asm volatile("mov %%rdi, %0" : "=r"(Register));
    PrintReg(renderer, "rdi", Register);
    PrintLn(renderer);

    asm volatile("mov %%r8, %0" : "=r"(Register));
    PrintReg(renderer, " r8", Register);
    asm volatile("mov %%r9, %0" : "=r"(Register));
    PrintReg(renderer, " r9", Register);
    asm volatile("mov %%r10, %0" : "=r"(Register));
    PrintReg(renderer, "r10", Register);
    asm volatile("mov %%r11, %0" : "=r"(Register));
    PrintReg(renderer, "r11", Register);
    PrintLn(renderer);

    asm volatile("mov %%r12, %0" : "=r"(Register));
    PrintReg(renderer, "r12", Register);
    asm volatile("mov %%r13, %0" : "=r"(Register));
    PrintReg(renderer, "r13", Register);
    asm volatile("mov %%r14, %0" : "=r"(Register));
    PrintReg(renderer, "r14", Register);
    asm volatile("mov %%r15, %0" : "=r"(Register));
    PrintReg(renderer, "r15", Register);
    PrintLn(renderer);

    //asm volatile("mov %%rip, %0" : "=r"(Register));
    //PrintReg(renderer, "rip", 0);
    asm volatile("mov %%cs, %0" : "=r"(Register));
    PrintReg(renderer, " cs", Register);
    //asm volatile("mov %%rflags, %0" : "=r"(Register));
    //PrintReg(renderer, "rflags", Register);
    asm volatile("mov %%rsp, %0" : "=r"(Register));
    PrintReg(renderer, "rsp", Register);
    asm volatile("mov %%ss, %0" : "=r"(Register));
    PrintReg(renderer, " ss", Register);
    PrintLn(renderer);

    asm volatile("mov %%cr0, %0" : "=r"(Register));
    PrintReg(renderer, "cr0", Register);
    asm volatile("mov %%cr2, %0" : "=r"(Register));
    PrintReg(renderer, "cr2", Register);
    asm volatile("mov %%cr3, %0" : "=r"(Register));
    PrintReg(renderer, "cr3", Register);
    asm volatile("mov %%cr4, %0" : "=r"(Register));
    PrintReg(renderer, "cr4", Register);
    PrintLn(renderer);

}

/*
 renderer->Println();
	renderer->Println("Register dump: ");
    renderer->Println();
	renderer->Print("rax: ");
	asm volatile("mov %%rax, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rcx: ");
	asm volatile("mov %%rcx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rdx: ");
	asm volatile("mov %%rdx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rbx: ");
	asm volatile("mov %%rbx, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
    renderer->Println();
	renderer->Print("rsp: ");
	asm volatile("mov %%rsp, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rbp: ");
	asm volatile("mov %%rbp, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rsi: ");
	asm volatile("mov %%rsi, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
	renderer->Print("  ");
	renderer->Print("rdi: ");
	asm volatile("mov %%rdi, %0" : "=r"(Register));
	renderer->Print("0x");
	renderer->Print(ConvertHexToString(Register));
    renderer->Println();
    renderer->Print("cr0: ");
    asm volatile("mov %%cr0, %0" : "=r"(Register));
    renderer->Print("0x");
    renderer->Print(ConvertHexToString(Register));
    renderer->Print("  ");
    renderer->Print("cr2: ");
    asm volatile("mov %%cr2, %0" : "=r"(Register));
    renderer->Print("0x");
    renderer->Print(ConvertHexToString(Register));
    renderer->Print("  ");
    renderer->Print("cr3: ");
    asm volatile("mov %%cr3, %0" : "=r"(Register));
    renderer->Print("0x");
    renderer->Print(ConvertHexToString(Register));
    renderer->Print("  ");
    renderer->Println();
*/

int kernelPanicCount = 0;


void Panic(const char* panicMessage, const char* var, bool lock)
{
    if (!lock)
        AddToStack();
    //GlobalRenderer->CursorPosition.x = 0;
    //GlobalRenderer->CursorPosition.y = 0;
    // oh god please no test podcast
    //GlobalRenderer->Println("<BRUH START>", Colors.yellow);
    osData.crashed = true;

    // GlobalRenderer->Clear(Colors.black);
    // GlobalRenderer->Println("Remaining Count 0: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // GlobalRenderer->Println("Remaining Count 1: {}", to_string(osData.maxNonFatalCrashCount--), Colors.yellow);
    // while(true);

    // btw this is an multi crash kernel panic
    // since for some reason the cls somehow did the general protection fault multiple times so multiple crash windows -> so kernel panic
    // since ya know the mnfcc

    // ok so it crashes
    // just get a good cmd working with good fs stuff and a programming lang

    //osData.preCrashWindow = activeWindow;

    if (!lock && osData.maxNonFatalCrashCount-- > 0 && !osData.booting && !osData.tempCrash)
    {
        //GlobalRenderer->Println("BRUH 1", Colors.yellow);
        // if (osData.tempCrash)
        // {
        //     //GlobalRenderer->Println("BRUH 2", Colors.yellow);
        //     //for (int i = 0; i < osData.windows.GetCount(); i++)
        //     //    osData.windows[i]->hidden = true;
        //     //GlobalRenderer->Println("BRUH 3", Colors.yellow);
        // }
        // else
        // {
        //     AddToStack();
        //     osData.tempCrash = true;
        //     //GlobalRenderer->Println("BRUH 4", Colors.yellow);
        //     Window* crashWindow;
        //     {
        //         crashWindow = (Window*)_Malloc(sizeof(Window), "Crash Window");
        //         Size size = Size(800, 16*10 + (MStackData::stackPointer * (16*4)));
        //         Position pos = Position(((osData.windowPointerThing->actualScreenBuffer->Width - size.width) / 2), ((osData.windowPointerThing->actualScreenBuffer->Height) / 5));
                
        //         if (crashWindow != NULL)
        //         {
        //             //GlobalRenderer->Println("BRUH 4.5", Colors.yellow);
        //             *(crashWindow) = Window(new CustomInstance(InstanceType::WARNING), size, pos, "Crash Warning", true, true, true);
        //             //GlobalRenderer->Println("BRUH 4.6", Colors.yellow);
        //             osData.windows.Add(crashWindow);
                    
        //             //GlobalRenderer->Println("BRUH 4.7", Colors.yellow);

        //             // activeWindow = crashWindow;
        //             // //osData.mainTerminalWindow = crashWindow;
        //             // osData.activeCrashWindow = crashWindow;
        //             // crashWindow->moveToFront = true;
        //             osData.windowsToGetActive.Enqueue(crashWindow);
        //         }
        //     }
        //     // it crashes between 4 and 5, probably while trying to allocate memory since it used all the memory
        //     //GlobalRenderer->Println("BRUH 5", Colors.yellow);
            
        //     if (crashWindow != NULL)
        //     {
        //         //GlobalRenderer->Println("BRUH 5.1", Colors.yellow);
        //         //GlobalRenderer->Print("Win x: {}", to_string(crashWindow->size.width), Colors.yellow);
        //         //GlobalRenderer->Println(", y: {}", to_string(crashWindow->size.height), Colors.yellow);
        //         crashWindow->renderer->Clear(Colors.black);
        //         //GlobalRenderer->Println("BRUH 5.2", Colors.yellow);
        //         crashWindow->renderer->Println("-------------------------------------------------------", Colors.bred);
        //         crashWindow->renderer->Println("A (probably) non-fatal Kernel Panic has occured! (MNFCC: {})", to_string(osData.maxNonFatalCrashCount), Colors.bred);
        //         crashWindow->renderer->Println("-------------------------------------------------------", Colors.bred);
        //         crashWindow->renderer->Println();

        //         Serial::Writeln("-------------------------------------------------------");
        //         Serial::Writeln("A (probably) non-fatal Kernel Panic has occured! (MNFCC: {})", to_string(osData.maxNonFatalCrashCount));
        //         Serial::Writeln("-------------------------------------------------------");
        //         Serial::Writeln();
        //         //GlobalRenderer->Println("BRUH 5.3", Colors.yellow);
        //         crashWindow->renderer->Println("Panic Message:", Colors.yellow);
        //         crashWindow->renderer->Print(panicMessage, var, Colors.bred);
        //         crashWindow->renderer->Println("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), Colors.white);

        //         Serial::Writeln("Panic Message: {}  (MNFCC: {})", panicMessage, to_string(osData.maxNonFatalCrashCount));


        //         crashWindow->renderer->Println();
        //         crashWindow->renderer->Println();

        //         Serial::Writeln();
        //         Serial::Writeln();
                
        //         //GlobalRenderer->Println("BRUH 5.4", Colors.yellow);
        //         PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer, crashWindow->renderer, Colors.yellow);
        //         //GlobalRenderer->Println("BRUH 5.5", Colors.yellow);


        //         if (!usingBackupHeap)
        //             CreateWindowWithBenchmarkData();
        //     }
        //     //GlobalRenderer->Println("BRUH 6", Colors.yellow);

        //     osData.tempCrash = false;    
        //     RemoveFromStack();
        // }
        //GlobalRenderer->Println("<BRUH END>", Colors.yellow);
    }
    else
    {
        kernelPanicCount++;
        //osData.serialManager = NULL;
        GlobalRenderer->ClearDotted(Colors.red);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        GlobalRenderer->Println("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA", Colors.white);
        Serial::Writeln();
        Serial::Writeln();
        Serial::Writeln();
        Serial::TWriteln("KERNEL PANIC AAAAAAAAAAAAAAAAAAAAAAAAAAA");
        for (int i = 0; i < kernelPanicCount; i++)
            GlobalRenderer->Println();
        GlobalRenderer->Print(panicMessage, var, Colors.white);
        Serial::Write(panicMessage, var, true);
        GlobalRenderer->Println("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), Colors.white);
        Serial::Writeln("  (MNFCC: {})",  to_string(osData.maxNonFatalCrashCount), true);

        GlobalRenderer->Println();
        GlobalRenderer->Println();
        Serial::Writeln();

        osData.crashCount++;
        // if (osData.crashCount <= 2 && osData.debugTerminalWindow != NULL)//!osData.booting)
        // {
        //     osData.debugTerminalWindow->position.x = GlobalRenderer->framebuffer->Width - 500;
        //     osData.debugTerminalWindow->position.y = 23;
        //     osData.debugTerminalWindow->parentFrameBuffer = GlobalRenderer->framebuffer;
        //     osData.debugTerminalWindow->Render(osData.debugTerminalWindow->framebuffer, GlobalRenderer->framebuffer, osData.debugTerminalWindow->position, osData.debugTerminalWindow->size, osData.debugTerminalWindow);
        //     Serial::Writeln("<INSERT DEBUG TERMINAL DATA HERE>");
        // }
        // else
        {
            GlobalRenderer->Println("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");
            GlobalRenderer->Println();
            Serial::Writeln("(BTW the rendering of the debug terminal is causing issues so no debug terminal)");

            PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
            GlobalRenderer->Println();
            GlobalRenderer->Println();
            Serial::Writeln();
            Serial::Writeln();
            PrintRegisterDump(GlobalRenderer);

            //LockLoop();
            while (true) ;
        }
        
        PrintMStackTrace(MStackData::stackArr, MStackData::stackPointer);
        GlobalRenderer->Println();
        GlobalRenderer->Println();
        Serial::Writeln();
        Serial::Writeln();
        PrintRegisterDump(GlobalRenderer);
        
        //if (lock)

        //LockLoop();
        while (true) ;
    }

    osData.tempCrash = false;   

    if (!lock)
        RemoveFromStack(); 
} 

void Panic(const char* panicMessage, const char* var)
{
    Panic(panicMessage, var, true);
}

void Panic(const char* panicMessage)
{
    Panic(panicMessage, NULL, false);
}

void Panic(const char* panicMessage, bool lock)
{
    Panic(panicMessage, NULL, lock);
}

bool CheckKernelSpaceAddr(void* addr)
{
    return (uint64_t)addr >= 0xFFFF800000000000;
}


