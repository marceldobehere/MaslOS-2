#include "syncedList.h"
#include <libm/stubs.h>

template <typename T> void SyncedList<T>::Init(uint64_t capacity)
{
    AddToStack();
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.ReInit(capacity);
    RemoveFromStack();
}

template <typename T> SyncedList<T>::SyncedList(List<T>* list) : arr(Array<T>(0))
{
    AddToStack();
    syncList = list;
    Init(list->GetCapacity());
    for (int i = 0; i < list->GetCount(); i++)
        Add(list->ElementAt(i));
    RemoveFromStack();
}



template <typename T> void SyncedList<T>::Free()
{
    AddToStack();
    arr.Free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveFromStack();
}

template <typename T>void SyncedList<T>::expandArr()
{
    AddToStack();
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.Copy(newArr);
    arr.Free();
    arr = newArr;
    RemoveFromStack();
}

// template <typename T> SyncedList<T> SyncedList<T>::clone()
// {
//     AddToStack();
//     List<T> newList = List<T>(cap);

//     for (uint64_t i = 0; i < count; i++)
//         newList.add(arr[i]);

//     RemoveFromStack();
//     return newList;
// }

template <typename T> uint64_t SyncedList<T>::GetCount()
{
    return count;
}

template <typename T> uint64_t SyncedList<T>::GetCapacity()
{
    return cap;
}


template <typename T> int64_t SyncedList<T>::GetIndexOf(T item)
{
    AddToStack();
    for (uint64_t index = 0; index < count; index++)
        if (arr[index] == item)
        {
            RemoveFromStack();
            return index;
        }
    RemoveFromStack();
    return -1;
}

template <typename T> void SyncedList<T>::Add(T item)
{
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
    RemoveFromStack();
}

template <typename T> void SyncedList<T>::InsertAt(T item, uint64_t index)
{
    if (index > count)
        return;
    
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    
    for (uint64_t i = count - 1; i >= index; i--)
        arr[i+1] = arr[i];
    
    arr[index] = item;

    count++;
    RemoveFromStack();
}

template <typename T> void SyncedList<T>::RemoveAt(uint64_t index)
{
    AddToStack();
    if (index < 0 || index >= count)
    {
        RemoveFromStack();
        return;
    }
    for (uint64_t i = index + 1; i < count; i++)
    {
        arr[i - 1] = arr[i];
    }

    //arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> void SyncedList<T>::RemoveFirst()
{
    AddToStack();
    if (count == 0)
    {
        RemoveFromStack();
        return;
    }

    RemoveAt(0);

    RemoveFromStack();
}

template <typename T> void SyncedList<T>::RemoveLast()
{
    AddToStack();
    if (count == 0)
    {
        RemoveFromStack();
        return;
    }

    //arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> T& SyncedList<T>::operator[](uint64_t index)
{
    AddToStack();
    T& data = arr[index];
    RemoveFromStack();
    return data;
}

template <typename T> T SyncedList<T>::ElementAt(uint64_t index)
{
    return arr[index];
}


template <typename T> void SyncedList<T>::Sync()
{
    for (int i = 0; i < syncList->GetCount(); i++)
        if (this->GetIndexOf(syncList->ElementAt(i)) == -1)
            Add(syncList->ElementAt(i));
        
    for (int i = 0; i < count; i++)
        if (syncList->GetIndexOf(this->arr[i]) == -1)
        {
            RemoveAt(i);
            i--;
        }  
}

template <typename T> void SyncedList<T>::Move(int64_t from, int64_t to)
{
    if (from < 0 || from >= count)
        return;
    if (to < 0 || to >= count)
        return;
    if (from == to)
        return;

    if (from < to)
        to--;

    T temp = arr[from];
    RemoveAt(from);
    InsertAt(temp, to);
    

}

template <typename T> void SyncedList<T>::Move(T item, int64_t to)
{
    Move(GetIndexOf(item), to);
}





#include "syncedList_all.h"