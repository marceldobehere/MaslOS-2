#pragma once
#include "lock.h"
#include <libm/list/list_task.h>

template class Lockable<List<osTask*>*>;

#include <libm/list/list_basics.h>

template class Lockable<List<void*>*>;