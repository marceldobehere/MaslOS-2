#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../elf/elf.h"
#include <libm/interrupt_frame.h>
#include <libm/lock/lock_list_task.h>

namespace Scheduler
{
    extern Lockable<List<osTask*>*> osTasks;
    extern bool SchedulerEnabled;

    void InitScheduler();

    // maybe save more registers yes
    void SchedulerInterrupt(interrupt_frame* frame);

    void AddModule(Elf::LoadedElfFile module, int argc, char** argv);

    void AddTask(osTask* task);

    void RemoveTask(osTask* task);
};

