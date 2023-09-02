#pragma once
#include "lock.h"
#include <libm/queue/queue_basics.h>


template class Lockable<Queue<int>>;
template class Lockable<Queue<char>>;
template class Lockable<Queue<bool>>;
template class Lockable<Queue<uint8_t>>;
template class Lockable<Queue<uint64_t>>;
template class Lockable<Queue<int64_t>>;
template class Lockable<Queue<const char*>>;
template class Lockable<Queue<void*>>;

template class Lockable<Queue<int>*>;
template class Lockable<Queue<char>*>;
template class Lockable<Queue<bool>*>;
template class Lockable<Queue<uint8_t>*>;
template class Lockable<Queue<uint64_t>*>;
template class Lockable<Queue<int64_t>*>;
template class Lockable<Queue<const char*>*>;
template class Lockable<Queue<void*>*>;