#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../elf/elf.h"
#include <libm/interrupt_frame.h>
#include <libm/lock/lock_list_task.h>

namespace Scheduler
{
    extern Lockable<List<osTask*>*> osTasks;
    extern osTask* CurrentRunningTask;
    extern bool SchedulerEnabled;
    extern int CurrentTaskIndex; 

    void InitScheduler();

    // maybe save more registers yes
    interrupt_frame* SchedulerInterrupt(interrupt_frame* frame);

    void AddElf(Elf::LoadedElfFile module, int argc, char** argv, bool isUserMode);

    void AddTask(osTask* task);

    void RemoveTask(osTask* task);
};

