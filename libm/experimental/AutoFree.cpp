#include "AutoFree.h"
#include <libm/experimental/RelocatableAllocator.h>

void ReferenceCounter::Increment()
{
    ++RefCount;
}
int ReferenceCounter::Decrement()
{
    --RefCount;
    return RefCount;
}
int ReferenceCounter::GetCount()
{
    return RefCount;
}

static char buf[1024 * 500]; // enough for now will change to allow for other stuffs?
RelocatableAllocator Allocator;

template <class T>
AutoFree<T>::AutoFree(T value)
{
    if (Allocator.Size == 0)
    {
        Allocator = RelocatableAllocator(buf, 128 * sizeof(ReferenceCounter));
    }
    T *ptr = (T *)Allocator.alloc(sizeof(T));
    for (int i = 0; i < sizeof(value); ++i)
    {
        ((char *)ptr)[i] = ((char *)&value)[i];
    }
    Obj = ptr;
    RefCount = (ReferenceCounter *)Allocator.alloc(sizeof(ReferenceCounter));
    RefCount->Increment();
}

template <class T>
AutoFree<T>::~AutoFree()
{
    if (RefCount)
    {
        int dec = RefCount->Decrement();
        if (dec <= 0)
        {
            Allocator.free((char *)RefCount);
            Allocator.free((char *)Obj);
            RefCount = nullptr;
            Obj = nullptr;
        }
    }
}
template <class T>
AutoFree<T>::AutoFree(AutoFree<T> &other)
{
    RefCount = other.RefCount;
    Obj = other.Obj;
    RefCount->Increment();
}

// template <class T>
// AutoFree<T>::AutoFree(uint64_t size, T *value)
// {
//     if (Allocator.Size == 0)
//     {
//         Allocator = RelocatableAllocator(buf, 128 * sizeof(ReferenceCounter));
//     }
//     T *ptr = (T *)Allocator.alloc(sizeof(T));
//     for (int i = 0; i < sizeof(value); ++i)
//     {
//         ((char *)ptr)[i] = ((char *)value)[i];
//     }
//     Obj = ptr;
//     RefCount = (ReferenceCounter *)Allocator.alloc(sizeof(ReferenceCounter));
//     RefCount->Increment();
// }

template <class T>
AutoFree<T> AutoFree<T>::operator=(AutoFree<T> &other)
{
    if (this != &other)
    {
        if (RefCount && RefCount->Decrement() == 0)
        {
            Allocator.free((char *)RefCount);
            Allocator.free((char *)Obj);
        }
        Obj = other.Obj;
        RefCount = other.RefCount;
        RefCount->Increment();
    }
    return *this;
}

template <class T>
T &AutoFree<T>::operator*()
{
    return *Obj;
}

template <class T>
T *AutoFree<T>::operator->()
{
    return Obj;
}

template class AutoFree<uint8_t>;
template class AutoFree<uint16_t>;
template class AutoFree<uint32_t>;
template class AutoFree<uint64_t>;
template class AutoFree<int8_t>;
template class AutoFree<int16_t>;
template class AutoFree<int32_t>;
template class AutoFree<int64_t>;