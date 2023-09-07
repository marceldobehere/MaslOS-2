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
    Elf::LoadedElfFile testElfFile;

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
    interrupt_frame* SchedulerInterrupt(interrupt_frame* currFrame)
    {
        if (!SchedulerEnabled)
            return currFrame;
        if (osTasks.IsLocked())
        {
            CurrentRunningTask = NULL;
            CurrentTaskIndex = 0;
            Serial::Writelnf("SCHEDULER> LOCKED");
            osTasks.Unlock();
            //return currFrame;
        }

        interrupt_frame outFrame = *currFrame;
        
        int64_t time = PIT::TimeSinceBootMS();

        //Serial::Writelnf("SCHEDULER> INTERRUPT %d", osTasks.obj->GetCount());

        osTasks.Lock();

        {
            osTask* currentTask = NULL;
            if (CurrentRunningTask != NULL && (int64_t)osTasks.obj->GetCount() > 0 && CurrentTaskIndex <  (int64_t)osTasks.obj->GetCount())
            {
                currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
                if (currentTask == NULL)
                    Panic("CURRENT TASK IS NULL!", true);
                
                if (currentTask->removeMe || currentTask->doExit)
                {
                    Serial::Writelnf("SCHEDULER> TASK WANTS TO EXIT");
                    //osTasks.obj->RemoveAt(CurrentTaskIndex);
                    CurrentRunningTask = NULL;
                    currentTask = NULL;
                }
                else if (CurrentRunningTask == currentTask)
                {
                    Serial::Writelnf("SCHEDULER> SAVING PREV DATA");
                    *currentTask->frame = *currFrame;
                }
            }
        }

        for (int i = 0; i < osTasks.obj->GetCount(); i++)
        {
            osTask* bruhTask = osTasks.obj->ElementAt(i);

            if (bruhTask == CurrentRunningTask)
                continue;

            if (bruhTask->removeMe || bruhTask->doExit)
            {
                Serial::Writelnf("SCHEDULER> AUTO REMOVING STOPPED TASK AT %d", i);

                osTasks.Unlock();
                RemoveTask(bruhTask);
                osTasks.Lock();
                i--;
            }
        }

        bool cycleDone = false;
        for (int i = 0; i < osTasks.obj->GetCount(); i++)
        {
            osTask* bruhTask = osTasks.obj->ElementAt(i);

            if (bruhTask->removeMe)
                continue;
            

            if (bruhTask->doExit)
                continue;
            
                
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

            if (osTasks.obj->GetCount() == 0)
                break;

            osTask* currentTask = osTasks.obj->ElementAt(CurrentTaskIndex);
            if (currentTask->taskTimeoutDone != 0)
                if (currentTask->taskTimeoutDone < time)
                    currentTask->taskTimeoutDone = 0;
            
            if (currentTask->taskTimeoutDone != 0)
                continue;

            if (!currentTask->active)
                continue;

            if (currentTask->doExit)
                continue;

            if (currentTask->removeMe)
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

        osTask* nowTask;
        if (cycleDone)
        {
            if (CurrentRunningTask != NothingDoerTask)
                Serial::Writelnf("SCHEDULER> NO TASKS TO RUN");
            nowTask = NothingDoerTask;

            if (nowTask == NULL)
            {
                osTasks.Unlock();
                Serial::Writelnf("SCHEDULER> NO NOTHING DOER TASK!");
                return currFrame;
            }
        }
        else
            nowTask = osTasks.obj->ElementAt(CurrentTaskIndex);
        

        if (nowTask->doExit)
        {
            Panic("YOU SHOULD NOT BE HERE!!!", true);
            // osTasks.Unlock();
            // Serial::Writelnf("SCHEDULER> TASK EXITED");
            // RemoveTask(currentTask);
            // // free task
            return currFrame;
        }

        //Serial::Writelnf("2> CURR RUNNING TASK: %X, CURR TASK: %X", (uint64_t)currentRunningTask, (uint64_t)currentTask);

        //Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);

        //Serial::Writelnf("SCHEDULER> LOADING NEXT DATA");
        outFrame = *nowTask->frame;


        if (CurrentRunningTask != nowTask)
        {
            CurrentRunningTask = nowTask;
            if (nowTask != NothingDoerTask)
                Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d / %d", CurrentTaskIndex, osTasks.obj->GetCount());
            //Serial::Writelnf("SCHEDULER> RIP %X", frame->rip);
        }

        //Serial::Writelnf("> CS 2: %D", frame->cs);

        // set cr3
        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);
        //frame->cr3 = (uint64_t)((PageTable*)currentTask->pageTableContext)->entries;
        //frame->cr3 = (uint64_t)GlobalPageTableManager.PML4->entries;

        //Serial::Writelnf("SCHEDULER> EXITING INTERRUPT");
        osTasks.Unlock();
        *currFrame = outFrame;
        return currFrame;      
    }

    #define KERNEL_STACK_PAGE_SIZE 8
    #define USER_STACK_PAGE_SIZE 4

    osTask* CreateTaskFromElf(Elf::LoadedElfFile module, int argc, char** argv, bool isUserMode)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;

        AddToStack();
        osTask* task = new osTask();
        RemoveFromStack();

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
        AddToStack();
        task->requestedPages = new List<void*>(4);
        RemoveFromStack();
        task->doExit = false;
        task->active = true;
        task->priority = 0;
        task->priorityStep = 0;
        task->isKernelModule = !isUserMode;
        task->justYielded = false;
        task->removeMe = false;

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
        AddToStack();

        if (task == NULL)
        {
            Panic("Trying to remove non-existant Task!", true);
            RemoveFromStack();
            return;
        }

        AddToStack();
        task->active = false;
        RemoveFromStack();

        AddToStack();
        osTasks.Lock();
        RemoveFromStack();

        Serial::Writelnf("> Trying to remove task at %X", (uint64_t)task);

        AddToStack();
        int index = osTasks.obj->GetIndexOf(task);
        RemoveFromStack();
        if (index != -1)
        {
            AddToStack();
            Serial::Writelnf("> Removing task at index %d", index);
            osTasks.obj->RemoveAt(index);
            Serial::Writelnf("> Removed task at index %d", index);
            RemoveFromStack();

            // free task
            AddToStack();
            if (task->requestedPages != NULL)
            {
                AddToStack();
                for (int i = 0; i < task->requestedPages->GetCount(); i++)
                {
                    Serial::Writelnf("> Freeing page %X", (uint64_t)task->requestedPages->ElementAt(i));
                    GlobalAllocator->FreePage((void*)task->requestedPages->ElementAt(i));
                }
                RemoveFromStack();

                AddToStack();
                Serial::Writelnf("> Freeing requested pages");
                task->requestedPages->Free();
                _Free(task->requestedPages);
                task->requestedPages = NULL;
                RemoveFromStack();
            }
            RemoveFromStack();

            AddToStack();
            if (task->kernelStack != NULL)
            {
                Serial::Writelnf("> Freeing kernel stack");
                GlobalAllocator->FreePages(task->kernelStack, KERNEL_STACK_PAGE_SIZE);
                task->kernelStack = NULL;
            }
            RemoveFromStack();

            AddToStack();
            if (task->userStack != NULL)
            {
                Serial::Writelnf("> Freeing user stack");
                GlobalAllocator->FreePages(task->userStack, USER_STACK_PAGE_SIZE);
                task->userStack = NULL;
            }
            RemoveFromStack();

            AddToStack();
            if (task->pageTableContext != NULL)
            {
                Serial::Writelnf("> Freeing page table");
                GlobalPageTableManager.FreePageTable((PageTable*)task->pageTableContext);
                task->pageTableContext = NULL;
            }
            RemoveFromStack();
        }

        AddToStack();
        osTasks.Unlock();
        RemoveFromStack();

        AddToStack();
        Serial::Writelnf("> Freeing task");
        _Free(task);
        RemoveFromStack();

        if (task == CurrentRunningTask)
            CurrentRunningTask = NULL;

        RemoveFromStack();
        SchedulerEnabled = tempEnabled;
        Serial::Writelnf("> Done");
    }

}

