#include "scheduler.h"
#include "../osData/osData.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../paging/PageTableManager.h"
#include "../interrupts/interrupts.h"
#include "../devices/serial/serial.h"
#include "../interrupts/panic.h"

namespace Scheduler
{
    Lockable<List<osTask*>*> osTasks;
    osTask* CurrentRunningTask;
    bool SchedulerEnabled = false;
    int CurrentTaskIndex = 0;

    void InitScheduler()
    {
        CurrentRunningTask = NULL;
        CurrentTaskIndex = 0;

        osTasks = Lockable<List<osTask*>*>(new List<osTask*>());

        osTasks.Lock();

        Serial::Writelnf("SCHEDULER> INITIALIZING SCHEDULER");

        osTasks.Unlock();

        SchedulerEnabled = true;
    }

    interrupt_frame* SchedulerInterrupt(interrupt_frame* frame)
    {
        if (!SchedulerEnabled)
            return frame;
        if (osTasks.IsLocked())
            return frame;
        
        //Serial::Writelnf("SCHEDULER> INTERRUPT %d", osTasks.obj->GetCount());

        if (osTasks.obj->GetCount() == 0)
            return frame;
        

        if (CurrentTaskIndex >= osTasks.obj->GetCount())
            CurrentTaskIndex = 0;

        
        
        osTask* currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
        if (currentTask->exited)
        {
            Serial::Writelnf("SCHEDULER> TASK EXITED");
            osTasks.obj->RemoveAt(CurrentTaskIndex);
            // free task
            return frame;
        }

        //Serial::Writelnf("SCHEDULER> SWITCHING FROM TASK %d", CurrentTaskIndex);

        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);

        //Serial::Writelnf("1> CURR RUNNING TASK: %X, CURR TASK: %X", (uint64_t)currentRunningTask, (uint64_t)currentTask);

        // Serial::Writelnf("> CS 1: %D", frame->cs);
        // Serial::Writelnf("> SS 1: %D", frame->ss);


        if (CurrentRunningTask == currentTask)
        {
            //Serial::Writelnf("SCHEDULER> SAVING PREV DATA");
            *currentTask->frame = *frame;

            // currentTask->frame->rax = frame->rax;
            // currentTask->frame->rbx = frame->rbx;
            // currentTask->frame->rcx = frame->rcx;
            // currentTask->frame->rdx = frame->rdx;
            // currentTask->frame->r8 = frame->r8;
            // currentTask->frame->r9 = frame->r9;
            // currentTask->frame->r10 = frame->r10;
            // currentTask->frame->r11 = frame->r11;
            // currentTask->frame->r12 = frame->r12;
            // currentTask->frame->r13 = frame->r13;
            // currentTask->frame->r14 = frame->r14;
            // currentTask->frame->r15 = frame->r15;
            // currentTask->frame->rip = frame->rip;
            // currentTask->frame->rsp = frame->rsp;
            // currentTask->frame->rbp = frame->rbp;
            // currentTask->frame->rsi = frame->rsi;
            // currentTask->frame->rdi = frame->rdi;
            // currentTask->frame->rflags = frame->rflags;

            //currentTask->frame->cs = frame->cs;
        }

        CurrentTaskIndex++;  
        if (CurrentTaskIndex >= osTasks.obj->GetCount())
            CurrentTaskIndex = 0;

        currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
        if (currentTask->exited)
        {
            Serial::Writelnf("SCHEDULER> TASK EXITED");
            RemoveTask(currentTask);
            // free task
            return frame;
        }

        //Serial::Writelnf("2> CURR RUNNING TASK: %X, CURR TASK: %X", (uint64_t)currentRunningTask, (uint64_t)currentTask);

        //Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);

        //Serial::Writelnf("SCHEDULER> LOADING NEXT DATA");
        *frame = *currentTask->frame;
        // frame->rax = currentTask->frame->rax;
        // frame->rbx = currentTask->frame->rbx;
        // frame->rcx = currentTask->frame->rcx;
        // frame->rdx = currentTask->frame->rdx;
        // frame->r8 = currentTask->frame->r8;
        // frame->r9 = currentTask->frame->r9;
        // frame->r10 = currentTask->frame->r10;
        // frame->r11 = currentTask->frame->r11;
        // frame->r12 = currentTask->frame->r12;
        // frame->r13 = currentTask->frame->r13;
        // frame->r14 = currentTask->frame->r14;
        // frame->r15 = currentTask->frame->r15;
        // frame->rip = currentTask->frame->rip;
        // frame->rsp = currentTask->frame->rsp;
        // frame->rbp = currentTask->frame->rbp;
        // frame->rsi = currentTask->frame->rsi;
        // frame->rdi = currentTask->frame->rdi;
        // frame->rflags = currentTask->frame->rflags;

        //frame->cs = currentTask->frame->cs;
        if (CurrentRunningTask != currentTask)
        {
            CurrentRunningTask = currentTask;
            Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);
            Serial::Writelnf("SCHEDULER> RIP %X", frame->rip);
        }

        //Serial::Writelnf("> CS 2: %D", frame->cs);

        // set cr3
        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);
        frame->cr3 = (uint64_t)((PageTable*)currentTask->pageTableContext)->entries;

        //Serial::Writelnf("SCHEDULER> EXITING INTERRUPT");
        return frame;      
    }

    #define KERNEL_STACK_PAGE_SIZE 8
    #define USER_STACK_PAGE_SIZE 4

    void AddElf(Elf::LoadedElfFile module, int argc, char** argv, bool isUserMode)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;

        osTask* task = new osTask();

        uint8_t* kernelStack = (uint8_t*)GlobalAllocator->RequestPages(KERNEL_STACK_PAGE_SIZE);
        GlobalPageTableManager.MapMemories(kernelStack, kernelStack, KERNEL_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);

        uint8_t* userStack = (uint8_t*)GlobalAllocator->RequestPages(USER_STACK_PAGE_SIZE);
        if (isUserMode)
            GlobalPageTableManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
        else
            GlobalPageTableManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite);

        task->kernelStack = kernelStack;
        task->userStack = userStack;

        
        task->exited = false;

        task->pageTableContext = GlobalPageTableManager.CreatePageTableContext();
        PageTableManager tempManager = PageTableManager((PageTable*)task->pageTableContext);
        
        CopyPageTable(GlobalPageTableManager.PML4, tempManager.PML4);

        if (isUserMode)
            tempManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
        else
            tempManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite);


        uint8_t* kernelStackEnd = (uint8_t*)kernelStack + KERNEL_STACK_PAGE_SIZE * 0x1000;
        uint8_t* userStackEnd = (uint8_t*)userStack + USER_STACK_PAGE_SIZE * 0x1000;

        {
            task->kernelEnvStack = kernelStackEnd;     
            //write argc, argv, env to stack
        
            ENV_DATA env;
            env.globalFrameBuffer = GlobalRenderer->framebuffer;
            env.globalFont = GlobalRenderer->psf1_font;

            kernelStackEnd -= 4;
            *((int*)kernelStackEnd) = argc;
            
            kernelStackEnd -= 8;
            *((uint64_t*)kernelStackEnd) = (uint64_t)argv;
            
            kernelStackEnd -= sizeof(ENV_DATA);
            *((ENV_DATA*)kernelStackEnd) = env;
        }

        kernelStackEnd -= sizeof(interrupt_frame);
        interrupt_frame* frame = (interrupt_frame*)kernelStackEnd;
        _memset(frame, 0, sizeof(interrupt_frame));
        task->frame = frame;

        userStackEnd -= - sizeof(uint64_t);
        kernelStackEnd -= sizeof(uint64_t);

        if (isUserMode)
        {
            frame->rip = (uint64_t)module.entryPoint;//task_entry;
            frame->cr3 = (uint64_t)tempManager.PML4->entries; // (uint64_t)GlobalPageTableManager.PML4->entries;//
            frame->rsp = (uint64_t)userStackEnd;
            frame->rax = (uint64_t)module.entryPoint;
            frame->cs = 0x28 | 0x03; // 0x28;//
            frame->ss = 0x20 | 0x03; // 0x20;//

            frame->rflags = 0x202;//(1 << 9) | (1 << 1);
        }
        else
        {
            frame->rip = (uint64_t)task_entry;
            frame->cr3 = (uint64_t)tempManager.PML4->entries; // (uint64_t)GlobalPageTableManager.PML4->entries;//
            frame->rsp = (uint64_t)userStackEnd;
            frame->rax = (uint64_t)module.entryPoint;
            frame->cs = 0x8;// 0x18 | 0x03;
            frame->ss = 0x10;

            frame->rflags = 0x202;//(1 << 9) | (1 << 1);
        }
        


        osTasks.Lock();
        osTasks.obj->Add(task);
        osTasks.Unlock();

        SchedulerEnabled = tempEnabled;
    }

    void AddTask(osTask* task)
    {
        Panic("UHMMMM", true);
    }

    void RemoveTask(osTask* task)
    {
        if (task == NULL)
        {
            Panic("Trying to remove non-existant Task!", true);
            return;
        }

        osTasks.Lock();

        Serial::Writelnf("> Trying to remove task at %X", (uint64_t)task);

        int index = osTasks.obj->GetIndexOf(task);
        if (index != -1)
        {
            Serial::Writelnf("> Removing task at index %d", index);
            osTasks.obj->RemoveAt(index);
            // free task
        }

        osTasks.Unlock();
    }

}

