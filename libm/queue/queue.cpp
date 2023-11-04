#include "queue.h"
#include "./queue_all.h"
#include "../stubs.h"


#define DEF_SIZE 10

template <typename T> Queue<T>::Queue(int64_t yesSize)
{
    if (yesSize < 2)
        yesSize = 2;
    arrSize = yesSize;
    count = 0;
    arr = (T*)_Malloc(sizeof(T) * arrSize, "Queue Constructor");
    head = 0;
    tail = 0;
    freed = false;
}

template <typename T> Queue<T>::Queue()
{
    arrSize = DEF_SIZE;
    count = 0;
    arr = (T*)_Malloc(sizeof(T) * arrSize, "Queue Constructor");
    head = 0;
    tail = 0;
    freed = false;
}

template <typename T> void Queue<T>::Free()
{
    if (freed)
        return;
    
    freed = true;
    _Free(arr);
    arr = NULL;
}

template <typename T> void Queue<T>::Resize(uint64_t newSize)
{
    if (freed)
        return;
    if (newSize <= count)
        Panic("Queue Resize: New size is smaller than count!", true);
    
    T* newArr = (T*)_Malloc(sizeof(T) * newSize, "Queue Resize");
    for (int64_t i = 0; i < count; i++)
        newArr[i] = arr[(head + i) % arrSize];
    
    _Free(arr);
    arr = newArr;
    arrSize = newSize;
    head = 0;
    tail = count;
}

template <typename T> int64_t Queue<T>::GetCount()
{
    return count;
}


template <typename T> T Queue<T>::Dequeue()
{
    if (freed)
    {
        Panic("Dequeueing from freed queue!", true);
        return arr[0];
    }

    if (count == 0)
    {
        Panic("Dequeueing from empty queue!", true);
        return arr[0];
    }
    
    T t = arr[head];
    head = (head + 1) % arrSize;
    count--;
    return t;
}

template <typename T> void Queue<T>::Enqueue(T item)
{
    if (freed)
    {
        Panic("Enqueueing to freed queue!", true);
        return;
    }

    if (count == arrSize)
        Resize(arrSize * 2);
    
    arr[tail] = item;
    tail = (tail + 1) % arrSize;
    count++;
}

template <typename T> void Queue<T>::Clear()
{
    if (freed)
        return;
    
    head = 0;
    tail = 0;
    count = 0;
}

template <typename T> T* Queue<T>::First(bool(*condFunction)(T))
{
    if (freed)
        return NULL;
    if (condFunction == NULL)
        return NULL;
    
    for (int64_t i = 0; i < count; i++)
    {
        T* item = arr + ((head + i) % arrSize);
        if (condFunction(*item))
            return item;
    }
    
    return NULL;
}

template <typename T> void Queue<T>:: Remove(T* item)
{
    if (freed)
        return;
    
    // get index
    int64_t index = -1;
    for (int64_t i = 0; i < count; i++)
    {
        T* item2 = arr + ((head + i) % arrSize);
        if (item2 == item)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;
    
    // remove
    for (int64_t i = index; i < count - 1; i++)
        arr[(head + i) % arrSize] = arr[(head + i + 1) % arrSize];
    
    count--;
    tail = (arrSize + tail - 1) % arrSize;
}