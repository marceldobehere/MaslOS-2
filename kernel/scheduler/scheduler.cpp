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

    // TODO
    // Handle SMP (Symmetric Multi-Processing)
    // Handle FPU Register States on Context Switch
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

        if (CurrentRunningTask == currentTask)
        {
            //Serial::Writelnf("SCHEDULER> SAVING PREV DATA");
            *currentTask->frame = *frame;
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


        if (CurrentRunningTask != currentTask)
        {
            CurrentRunningTask = currentTask;
            Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);
            //Serial::Writelnf("SCHEDULER> RIP %X", frame->rip);
        }

        //Serial::Writelnf("> CS 2: %D", frame->cs);

        // set cr3
        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);
        frame->cr3 = (uint64_t)((PageTable*)currentTask->pageTableContext)->entries;
        frame->cr3 = (uint64_t)GlobalPageTableManager.PML4->entries;

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
        GlobalPageTableManager.MapMemories(kernelStack, kernelStack, KERNEL_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite);

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
            frame->rip = (uint64_t)module.entryPoint;
            frame->cr3 = (uint64_t)tempManager.PML4->entries;
            frame->rsp = (uint64_t)userStackEnd;
            frame->rax = (uint64_t)module.entryPoint;
            frame->cs = 0x28 | 0x03;
            frame->ss = 0x20 | 0x03;

            frame->rflags = 0x202;
        }
        else
        {
            frame->rip = (uint64_t)module.entryPoint;
            frame->cr3 = (uint64_t)tempManager.PML4->entries;
            frame->rsp = (uint64_t)userStackEnd;
            frame->rax = (uint64_t)module.entryPoint;
            frame->cs = 0x8;
            frame->ss = 0x10;

            frame->rflags = 0x202;
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

