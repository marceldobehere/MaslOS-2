#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../elf/elf.h"
#include <libm/interrupt_frame.h>
#include <libm/lock/lock_list_task.h>

#define KERNEL_STACK_PAGE_SIZE 4
#define USER_STACK_PAGE_SIZE 4

namespace Scheduler
{
    extern Lockable<List<osTask*>*> osTasks;
    extern osTask* CurrentRunningTask;
    extern osTask* NothingDoerTask;
    extern osTask* DesktopTask;
    extern bool SchedulerEnabled;
    extern int CurrentTaskIndex; 
    extern void* TestElfFile;
    extern void* DesktopElfFile;
    extern Lockable<List<void*>*> UsedPageRegions;

    void InitScheduler();

    // maybe save more registers yes
    interrupt_frame* SchedulerInterrupt(interrupt_frame* frame);

    osTask* CreateTaskFromElf(Elf::LoadedElfFile module, int argc, const char** argv, bool isUserMode, const char* elfPath, const char* startedAtPath);

    void* RequestNextFreePageRegion();
    void FreePageRegion(void* addr);

    void AddTask(osTask* task);

    void RemoveTask(osTask* task);

    osTask* GetTask(uint64_t pid);
};

