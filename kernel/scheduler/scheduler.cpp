#include "scheduler.h"
#include "../osData/osData.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../paging/PageTableManager.h"
#include "../interrupts/interrupts.h"
#include "../devices/serial/serial.h"
#include "../interrupts/panic.h"
#include "../devices/pit/pit.h"

namespace Scheduler
{
    Lockable<List<osTask*>*> osTasks;
    osTask* CurrentRunningTask;
    osTask* NothingDoerTask;
    bool SchedulerEnabled = false;
    int CurrentTaskIndex = 0;

    void InitScheduler()
    {
        CurrentRunningTask = NULL;
        CurrentTaskIndex = 0;
        NothingDoerTask = NULL;

        osTasks = Lockable<List<osTask*>*>(new List<osTask*>());

        osTasks.Lock();

        Serial::Writelnf("SCHEDULER> INITIALIZING SCHEDULER");

        osTasks.Unlock();

        SchedulerEnabled = false;
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
        
        int64_t time = PIT::TimeSinceBootMS();

        //Serial::Writelnf("SCHEDULER> INTERRUPT %d", osTasks.obj->GetCount());


        osTask* currentTask = NULL;
        if (osTasks.obj->GetCount() > 0 && CurrentTaskIndex < osTasks.obj->GetCount())
        {
            currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
            if (currentTask->doExit)
            {
                Serial::Writelnf("SCHEDULER> TASK EXITED");
                osTasks.obj->RemoveAt(CurrentTaskIndex);
                CurrentRunningTask = NULL;
                currentTask = NULL;
            }
        }

        //Serial::Writelnf("SCHEDULER> SWITCHING FROM TASK %d", CurrentTaskIndex);

        if (CurrentRunningTask == currentTask && currentTask != NULL)
        {
            //Serial::Writelnf("SCHEDULER> SAVING PREV DATA");
            *currentTask->frame = *frame;
        }

        bool cycleDone = false;
        for (int i = 0; i < osTasks.obj->GetCount(); i++)
        {
            osTask* bruhTask = osTasks.obj->ElementAt(i);

            if (bruhTask->taskTimeoutDone != 0)
                continue;

            if (!bruhTask->active)
                continue;
            
            if (bruhTask->priority == 0)
                continue;

            if (bruhTask->justYielded)
            {
                bruhTask->justYielded = false;
                continue;
            }

            bruhTask->priorityStep++;
            if (bruhTask->priorityStep >= bruhTask->priority)
            {
                bruhTask->priorityStep = 0;
                CurrentTaskIndex = i;
                cycleDone = true;
                break;
            }
        }

        if (cycleDone)
        {
            cycleDone = false;
            goto skip2ndLoop;
        }
            

        cycleDone = false;
        while (true)
        {
            CurrentTaskIndex++;
            if (CurrentTaskIndex >= osTasks.obj->GetCount())
            {
                if (!cycleDone)
                {
                    CurrentTaskIndex = 0;
                    cycleDone = true;
                }
                else
                    break;
            }

            currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
            if (currentTask->taskTimeoutDone != 0)
                if (currentTask->taskTimeoutDone < time)
                    currentTask->taskTimeoutDone = 0;
            
            if (currentTask->taskTimeoutDone != 0)
                continue;

            if (!currentTask->active)
                continue;

            if (currentTask->priority != 0)
                continue;

            if (currentTask->justYielded)
            {
                currentTask->justYielded = false;
                continue;
            }


            cycleDone = false;
            break;
        }

        skip2ndLoop:

        if (cycleDone)
        {
            if (CurrentRunningTask != NothingDoerTask)
                Serial::Writelnf("SCHEDULER> NO TASKS TO RUN");
            currentTask = NothingDoerTask;

            if (currentTask == NULL)
            {
                Serial::Writelnf("SCHEDULER> NO NOTHING DOER TASK!");
                return frame;
            }
        }
        else
            currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
        
        if (currentTask->doExit)
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
            if (currentTask != NothingDoerTask)
                Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);
            //Serial::Writelnf("SCHEDULER> RIP %X", frame->rip);
        }

        //Serial::Writelnf("> CS 2: %D", frame->cs);

        // set cr3
        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);
        //frame->cr3 = (uint64_t)((PageTable*)currentTask->pageTableContext)->entries;
        //frame->cr3 = (uint64_t)GlobalPageTableManager.PML4->entries;

        //Serial::Writelnf("SCHEDULER> EXITING INTERRUPT");
        return frame;      
    }

    #define KERNEL_STACK_PAGE_SIZE 8
    #define USER_STACK_PAGE_SIZE 4

    osTask* CreateTaskFromElf(Elf::LoadedElfFile module, int argc, char** argv, bool isUserMode)
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

        
        task->taskTimeoutDone = 0;
        task->requestedPages = new List<void*>();
        task->doExit = false;
        task->active = true;
        task->priority = 0;
        task->priorityStep = 0;
        task->isKernelModule = !isUserMode;
        task->justYielded = false;

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



        SchedulerEnabled = tempEnabled;
        return task;
    }

    void AddTask(osTask* task)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;
        
        osTasks.Lock();
        osTasks.obj->Add(task);
        osTasks.Unlock();

        SchedulerEnabled = tempEnabled;
    }

    void RemoveTask(osTask* task)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;

        if (task == NULL)
        {
            Panic("Trying to remove non-existant Task!", true);
            return;
        }

        task->active = false;

        osTasks.Lock();

        Serial::Writelnf("> Trying to remove task at %X", (uint64_t)task);

        int index = osTasks.obj->GetIndexOf(task);
        if (index != -1)
        {
            Serial::Writelnf("> Removing task at index %d", index);
            osTasks.obj->RemoveAt(index);
            //Serial::Writelnf("> Removed task at index %d", index);

            // free task
            
            if (task->requestedPages != NULL)
            {
                for (int i = 0; i < task->requestedPages->GetCount(); i++)
                    GlobalAllocator->FreePage((void*)task->requestedPages->ElementAt(i));
                
                task->requestedPages->Free();
                _Free(task->requestedPages);
                task->requestedPages = NULL;
            }

            if (task->kernelStack != NULL)
            {
                GlobalAllocator->FreePages(task->kernelStack, KERNEL_STACK_PAGE_SIZE);
                task->kernelStack = NULL;
            }

            if (task->userStack != NULL)
            {
                GlobalAllocator->FreePages(task->userStack, USER_STACK_PAGE_SIZE);
                task->userStack = NULL;
            }

            if (task->pageTableContext != NULL)
            {
                GlobalPageTableManager.FreePageTable((PageTable*)task->pageTableContext);
                task->pageTableContext = NULL;
            }
        }

        osTasks.Unlock();

        SchedulerEnabled = tempEnabled;
    }

}

