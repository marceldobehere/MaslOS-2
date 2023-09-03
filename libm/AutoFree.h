#pragma once
#include <stdint.h>

class ReferenceCounter
{
private:
    int RefCount = 0;

public:
    void Increment();
    int Decrement();
    int GetCount();
};

template <class T>
class AutoFree
{
private:
    T *Obj;
    ReferenceCounter *RefCount;

public:
    AutoFree()
    {
    }
    AutoFree(T value);
    AutoFree(AutoFree<T> &other);
    AutoFree(AutoFree<T> &other);
    AutoFree<T> operator=(AutoFree<T> &other);
    AutoFree<T>::AutoFree(uint64_t size, T *value);
    T &operator*();
    T *operator->();
    ~AutoFree();
};