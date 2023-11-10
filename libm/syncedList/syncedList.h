#pragma once
#include <stdint.h>

#include <libm/list/list.h>


template <typename T> class SyncedList
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    List<T>* syncList;
    bool freed;
    void Init(uint64_t capacity);
    void expandArr();
    void Add(T item);
    void InsertAt(T item, uint64_t index);
    void RemoveAt(uint64_t index);
    void RemoveLast();
    void RemoveFirst();

    public:
    //SyncedList<T>(uint64_t capacity);
    SyncedList<T>(List<T>* list);
    void Free();
    //List<T> clone(); 
    uint64_t GetCount();
    uint64_t GetCapacity();
    int64_t GetIndexOf(T item);

    void Sync();
    void Move(int64_t from, int64_t to);
    void Move(T item, int64_t to);
    T& operator[](uint64_t index);
    T ElementAt(uint64_t index);

};