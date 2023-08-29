#include "scheduler.h"
#include "../osData/osData.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../paging/PageTableManager.h"
#include "../interrupts/interrupts.h"

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
            return;
        }

        //Serial::Writelnf("SCHEDULER> SWITCHING FROM TASK %d", CurrentTaskIndex);

        //GlobalPageTableManager.SwitchPageTable((PageTable*)currentTask->pageTableContext);

        //Serial::Writelnf("1> CURR RUNNING TASK: %X, CURR TASK: %X", (uint64_t)currentRunningTask, (uint64_t)currentTask);

        if (CurrentRunningTask == currentTask)
        {
            //Serial::Writelnf("SCHEDULER> SAVING PREV DATA");
            currentTask->frame->rax = frame->rax;
            currentTask->frame->rbx = frame->rbx;
            currentTask->frame->rcx = frame->rcx;
            currentTask->frame->rdx = frame->rdx;
            currentTask->frame->r8 = frame->r8;
            currentTask->frame->r9 = frame->r9;
            currentTask->frame->r10 = frame->r10;
            currentTask->frame->r11 = frame->r11;
            currentTask->frame->r12 = frame->r12;
            currentTask->frame->r13 = frame->r13;
            currentTask->frame->r14 = frame->r14;
            currentTask->frame->r15 = frame->r15;
            currentTask->frame->rip = frame->rip;
            currentTask->frame->rsp = frame->rsp;
            currentTask->frame->rbp = frame->rbp;
            currentTask->frame->rsi = frame->rsi;
            currentTask->frame->rdi = frame->rdi;
            currentTask->frame->rflags = frame->rflags;
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
            return;
        }

        //Serial::Writelnf("2> CURR RUNNING TASK: %X, CURR TASK: %X", (uint64_t)currentRunningTask, (uint64_t)currentTask);

        //Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);

        //Serial::Writelnf("SCHEDULER> LOADING NEXT DATA");
        frame->rax = currentTask->frame->rax;
        frame->rbx = currentTask->frame->rbx;
        frame->rcx = currentTask->frame->rcx;
        frame->rdx = currentTask->frame->rdx;
        frame->r8 = currentTask->frame->r8;
        frame->r9 = currentTask->frame->r9;
        frame->r10 = currentTask->frame->r10;
        frame->r11 = currentTask->frame->r11;
        frame->r12 = currentTask->frame->r12;
        frame->r13 = currentTask->frame->r13;
        frame->r14 = currentTask->frame->r14;
        frame->r15 = currentTask->frame->r15;
        frame->rip = currentTask->frame->rip;
        frame->rsp = currentTask->frame->rsp;
        frame->rbp = currentTask->frame->rbp;
        frame->rsi = currentTask->frame->rsi;
        frame->rdi = currentTask->frame->rdi;
        frame->rflags = currentTask->frame->rflags;
        if (CurrentRunningTask != currentTask)
        {
            CurrentRunningTask = currentTask;
            Serial::Writelnf("SCHEDULER> SWITCHING TO TASK %d", CurrentTaskIndex);
        }

        //Serial::Writelnf("SCHEDULER> EXITING INTERRUPT");
        return frame;      
    }

    #define KERNEL_STACK_PAGE_SIZE 8
    #define USER_STACK_PAGE_SIZE 4

    void AddModule(Elf::LoadedElfFile module, int argc, char** argv)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;

        osTask* task = new osTask();

        uint8_t* kernelStack = (uint8_t*)GlobalAllocator->RequestPages(KERNEL_STACK_PAGE_SIZE);
        GlobalPageTableManager.MapMemories(kernelStack, kernelStack, KERNEL_STACK_PAGE_SIZE);
        uint8_t* userStack = (uint8_t*)GlobalAllocator->RequestPages(USER_STACK_PAGE_SIZE);
        GlobalPageTableManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);

        task->kernelStack = kernelStack;
        task->userStack = userStack;

        
        task->exited = false;

        task->pageTableContext = GlobalPageTableManager.CreatePageTableContext();
        GlobalPageTableManager.CopyPageTable(GlobalPageTableManager.PML4, (PageTable*)task->pageTableContext);





        uint8_t* kernelStackEnd = (uint8_t*)(uint64_t)kernelStack + KERNEL_STACK_PAGE_SIZE * 0x1000;
        


        {
            ENV_DATA env;
            env.globalFrameBuffer = GlobalRenderer->framebuffer;
            env.globalFont = GlobalRenderer->psf1_font;

            //write argc, argv, env to stack
            task->kernelEnvStack = kernelStackEnd;

            kernelStackEnd -= 4;
            *((int*)kernelStackEnd) = argc;
            
            kernelStackEnd -= 8;
            *((uint64_t*)kernelStackEnd) = (uint64_t)argv;
            
            kernelStackEnd -= sizeof(ENV_DATA);
            *((ENV_DATA*)kernelStackEnd) = env;
        }


        {
            kernelStackEnd -= sizeof(interrupt_frame);
            interrupt_frame* frame = (interrupt_frame*)kernelStackEnd;
            _memset(frame, 0, sizeof(interrupt_frame));
            frame->rip = (uint64_t)task_entry;
            //frame->cr3 = (uint64_t)GlobalPageTableManager.PML4->entries;//(uint64_t)((PageTable*)task->pageTableContext)->entries;
            frame->rsp = (uint64_t)kernelStackEnd;
            frame->rax = (uint64_t)module.entryPoint;


            task->frame = frame;
        }
        


        osTasks.Lock();

        osTasks.obj->Add(task);

        osTasks.Unlock();

        

        // //Elf::RunElfHere(module, 0, NULL, &env);
        // if (module.works)
        // {
        //     void (*entry)(int, char**, ENV_DATA*) = (void (*)(int, char**, ENV_DATA*)) module.entryPoint;
        //     Serial::Writelnf("ELF> ENTRY POINT: %x", entry);
        //     entry(argc, argv, &env);
        // }

        SchedulerEnabled = tempEnabled;
    }

    void AddTask(osTask* task)
    {
        Panic("UHMMMM", true);
    }

    void RemoveTask(osTask* task)
    {
        if (task == NULL)
            return;

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

