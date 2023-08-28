#include "scheduler.h"
#include "../osData/osData.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../paging/PageTableManager.h"

namespace Scheduler
{
    Lockable<List<osTask*>*> osTasks;
    bool SchedulerEnabled = false;

    void InitScheduler()
    {
        osTasks = Lockable<List<osTask*>*>(new List<osTask*>());

        osTasks.Lock();

        osTasks.Unlock();

        SchedulerEnabled = true;
    }

    void SchedulerInterrupt(interrupt_frame* frame)
    {
        if (!SchedulerEnabled)
            return;

        
    }

    #define KERNEL_STACK_PAGE_SIZE 8
    #define USER_STACK_PAGE_SIZE 4

    void AddModule(Elf::LoadedElfFile module, int argc, char** argv)
    {
        bool tempEnabled = SchedulerEnabled;
        SchedulerEnabled = false;

        osTask* task = new osTask();

        uint8_t* kernelStack = (uint8_t*)GlobalAllocator->RequestPages(KERNEL_STACK_PAGE_SIZE);
        GlobalPageTableManager.MapMemories(kernelStack, kernelStack, KERNEL_STACK_PAGE_SIZE, true);
        uint8_t* userStack = (uint8_t*)GlobalAllocator->RequestPages(USER_STACK_PAGE_SIZE);
        GlobalPageTableManager.MapMemories(userStack, userStack, USER_STACK_PAGE_SIZE, true);

        interrupt_frame* frame = (interrupt_frame*) ((uint64_t)kernelStack + KERNEL_STACK_PAGE_SIZE * 0x1000 - sizeof(interrupt_frame));
        frame->base_frame.rip = (uint64_t)module.entryPoint;
        frame->base_frame.rsp = (uint64_t)userStack + USER_STACK_PAGE_SIZE * 0x1000 - 8;
        frame->base_frame.rbp = frame->base_frame.rsp;

        task->kernelStack = kernelStack;
        task->userStack = userStack;

        task->frame = frame;
        task->exited = false;

        




        
        ENV_DATA env;
        env.globalFrameBuffer = GlobalRenderer->framebuffer;
        env.globalFont = GlobalRenderer->psf1_font;

        //Elf::RunElfHere(module, 0, NULL, &env);
        if (module.works)
        {
            void (*entry)(int, char**, ENV_DATA*) = (void (*)(int, char**, ENV_DATA*)) module.entryPoint;
            Serial::Writelnf("ELF> ENTRY POINT: %x", entry);
            entry(argc, argv, &env);
        }



        SchedulerEnabled = tempEnabled;
    }

    void AddTask(osTask* task)
    {

    }

    void RemoveTask(osTask* task)
    {

    }

}

