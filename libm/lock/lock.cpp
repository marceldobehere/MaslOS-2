#include "lock.h"
#include "lock_all.h"
#include "../stubs.h"


template <typename T> Lockable<T>::Lockable(T obj)
{
    this->obj = obj;
    this->locked = false;
    this->hasItem = true;
}

template <typename T> Lockable<T>::Lockable()
{
    this->locked = false;
    this->hasItem = false;
}

template <typename T> void Lockable<T>::Lock()
{
    if (this->locked)
        Panic("LOCKING LOCKED OBJECT", true);
    if (!this->hasItem)
        Panic("LOCKING UNINITIALIZED OBJECT", true);
    
    this->locked = true;
}

template <typename T> void Lockable<T>::Unlock()
{
    if (!this->locked)
        Panic("UNLOCKING UNLOCKED OBJECT", true);
    if (!this->hasItem)
        Panic("UNLOCKING UNINITIALIZED OBJECT", true);

    this->locked = false;
}

template <typename T> bool Lockable<T>::IsLocked()
{
    return this->locked;
}