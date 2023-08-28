#pragma once
#include <stdint.h>
#include <stddef.h>

template <typename T> class Lockable
{
    private:
    bool locked;
    bool hasItem;

    public:
    T obj;

    Lockable<T>(T obj);
    Lockable<T>();

    void Lock();
    void Unlock();
    bool IsLocked();
};