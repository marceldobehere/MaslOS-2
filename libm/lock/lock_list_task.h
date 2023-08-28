#pragma once
#include "lock.h"
#include <libm/list/list_task.h>

template class Lockable<List<osTask*>*>;

