#include "heap.h"
#include "../stubs.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/rendering/Cols.h>
#include <libm/syscallManager.h>


namespace Heap
{
    const uint64_t HeapMagicNum = 0xABCD12DEAD9654AA;//0;// 0xFFFFFFFFFFFFFFFF;//0xABCD12DEAD9654AA;
    
    HeapManager* GlobalHeapManager;
    
    void _HeapSegHdr::CombineForward(HeapManager* manager)
    {
        AddToStack();
        if (next == NULL)
        {
            RemoveFromStack();
            return;   
        }
        
        if (!next->free)
        {
            RemoveFromStack();
            return;   
        }

        if (next->last != this)
        {
            #ifdef _KERNEL_SRC
            return;
            #else
            Panic("COMBINING FORWARD BUT ACTUALLY NOT???", true);
            #endif
        }

        if (!free)
        {
            #ifdef _KERNEL_SRC
            return;
            #else
            Panic("COMBINING FORWARD BUT THE HDR IS NOT FREE???", true);
            #endif
        }
        
        if (next == manager->_lastHdr) 
            manager->_lastHdr = this;

        if (next->next != NULL)
            next->next->last = this;
        
        length = length + next->length + sizeof(_HeapSegHdr);
        next = next->next;
        text = "<FREE>";
        manager->_heapCount--;
        RemoveFromStack();
    }

    void _HeapSegHdr::CombineBackward(HeapManager* manager)
    {
        AddToStack();
        if (last != NULL && last->free)
            last->CombineForward(manager);
        RemoveFromStack();
    }

    _HeapSegHdr* _HeapSegHdr::Split(HeapManager* manager, size_t splitLength)
    {
        AddToStack();
        if (splitLength < 0x10)
        {
            RemoveFromStack();
            return NULL;
        }

        int64_t splitSegLength = ((int64_t)length - (int64_t)splitLength) - (int64_t)sizeof(_HeapSegHdr);
       if (splitSegLength < 0x10)
        {
            RemoveFromStack();
            return NULL;
        }

        _HeapSegHdr* newSplitHdr = (_HeapSegHdr*)((uint64_t)this + splitLength + sizeof(_HeapSegHdr));
        *newSplitHdr = _HeapSegHdr();

        if (next != NULL)
            next->last = newSplitHdr;

        newSplitHdr->next = next;
        next = newSplitHdr;

        newSplitHdr->last = this;
        newSplitHdr->length = splitSegLength;
        newSplitHdr->free = free;
        newSplitHdr->magicNum = HeapMagicNum;
        length = splitLength;
        newSplitHdr->text = "<FREE>";

        if (manager->_lastHdr == this) 
            manager->_lastHdr = newSplitHdr;

        manager->_heapCount++;
        RemoveFromStack();
        return newSplitHdr;
    }

    void HeapManager::SubInitHeap(void* heapAddress, size_t pageCount)
    {
        AddToStack();
        _activeMemFlagVal = 0;

        uint64_t heapLength = pageCount * 0x1000;
        _heapStart = heapAddress;
        _heapEnd = (void*)((uint64_t)_heapStart + heapLength);

        _HeapSegHdr* startSeg = (_HeapSegHdr*)heapAddress;

        //serialPrint("Start Seg: 0x");
        //serialPrintLn(ConvertHexToString((uint64_t)startSeg));

        startSeg->length = heapLength - sizeof(_HeapSegHdr);
        
        //return;
        startSeg->next = NULL;
        startSeg->last = NULL;
        startSeg->free = true;
        startSeg->text = "<FREE>";
        startSeg->magicNum = HeapMagicNum;
        startSeg->activeMemFlagVal = _activeMemFlagVal;
        _lastHdr = startSeg;
        _heapCount = 1;
        _usedHeapCount = 0;
        _usedHeapAmount = 0;
        RemoveFromStack();
    }

    void HeapManager::InitializeHeap(int pageCount)
    {
        AddToStack();
        void* pos = requestNextPages(pageCount);
        
        SubInitHeap(pos, pageCount);
        RemoveFromStack();
    }

    void* HeapManager::_Xmalloc(int64_t size,  const char* text)
    {
        return _Xmalloc(size, text, "<NO FUNC GIVEN>", "<NO FILE GIVEN>", 0);
    }

    void* HeapManager::_Xmalloc(int64_t size, const char* func, const char* file, int line)
    {
        return _Xmalloc(size, "<NO TEXT GIVEN>", func, file, line);
    }

    bool HeapManager::HeapCheck(bool wait)
    {
        AddToStack();
        if (wait)
        {
            #ifdef _KERNEL_SRC
            //return;
            #else
            programWait(100);
            #endif
            
        }
        bool foundError = false;

        _HeapSegHdr* current = (_HeapSegHdr*) _heapStart;
        int counter = 0;
        while(true)
        {
            counter++;

            if ((uint64_t)current < 10000)
            {
                foundError = true;
                break;
            }

            if (current->magicNum != HeapMagicNum)
            {
                foundError = true;
                break;
            }

            if (current->next == NULL)
            {
                foundError = true;
                break;
            }
            
            if (((uint64_t)(current->length + sizeof(_HeapSegHdr) + (uint64_t)current)) != (uint64_t)current->next)
            {
                foundError = true;
                break;
            }

            if (current->last != NULL)
            {
                if (current->last->next != current)
                {
                    foundError = true;
                    break;
                }
            }


            current = current->next;
        }

        if (foundError)
        {
            RemoveFromStack();
            return false;
        }

        if (wait)
        {
            #ifdef _KERNEL_SRC
            //return;
            #else
            programWait(500);
            #endif
        }
        
        RemoveFromStack();
        return true;
    }


    void* HeapManager::_Xmalloc(int64_t size, const char* text, const char* func, const char* file, int line)
    {
        AddToStack();
        
        if (size <= 0)
            size = 0x10;

        if (size % 0x10 > 0)
        {
            size -= (size % 0x10);
            size += 0x10;
        }

        _HeapSegHdr* current = (_HeapSegHdr*) _heapStart;
        while(true)
        {

            if ((int64_t)current < 10000)
            {
                #ifdef _KERNEL_SRC
                return NULL;
                #else
                Panic("CURRENT IS NULL BRO", true); 
                #endif
            }

            if (current->magicNum != HeapMagicNum)
            {
                HeapCheck(false);
                
                #ifdef _KERNEL_SRC
                return NULL;
                #else
                Panic("Trying to access invalid HeapSegment Header!", true);
                #endif
                RemoveFromStack();
                return NULL;
            }

            if (current->free)
            {
                if (current->length > (size + sizeof(_HeapSegHdr) + 0x10))
                {
                    if (current->Split(this, size) == NULL)
                    {
                        if (current->next == NULL)
                            break;

                        current = current->next;
                        continue;
                    }
                    current->free = false;
                    current->text = text;
                    current->activeMemFlagVal = _activeMemFlagVal;
                    current->file = file;
                    current->func = func;
                    current->line = line;
                    current->time = PIT::TimeSinceBootMS();
                    _usedHeapCount++;
                    _usedHeapAmount += size;
                    RemoveFromStack();
                    return (void*)((uint64_t)current + sizeof(_HeapSegHdr));
                }
                if (current->length == size)
                {
                    current->free = false;
                    current->text = text;
                    current->activeMemFlagVal = _activeMemFlagVal;
                    current->file = file;
                    current->func = func;
                    current->line = line;
                    current->time = PIT::TimeSinceBootMS();
                    _usedHeapCount++;
                    _usedHeapAmount += size;
                    RemoveFromStack();
                    return (void*)((uint64_t)current + sizeof(_HeapSegHdr));
                }
            }

            if (current->next == NULL)
                break;
            current = current->next;
        }
        
        if (ExpandHeap(size))
        {
            AddToStack();
            void* res = _Xmalloc(size, text);//_Malloc(size, text);
            RemoveFromStack();
            RemoveFromStack();

            return res;
        }

        
        #ifdef _KERNEL_SRC
        return NULL;
        #else
        Panic("MALLOC FAILED!!!", true);
        #endif

        RemoveFromStack();
        return NULL;
    }


    void HeapManager::_Xfree(void* address)
    {
        _Xfree(address, "<NO FUNC GIVEN>", "<NO FILE GIVEN>", 0);
    }

    void HeapManager::_Xfree(void* address, const char* func, const char* file, int line)
    {
        AddToStack();  
        if (address < (void*)1000)
        {
            #ifdef _KERNEL_SRC
            return;
            #else
            Panic("Tried to free NULL address!", true);
            #endif
        }
        
        _HeapSegHdr* segment = ((_HeapSegHdr*)address) - 1;
        if (segment->magicNum == HeapMagicNum)
        {
            if (!segment->free)
            {
                segment->free = true;
                segment->text = "<FREE>";
                _usedHeapAmount -= segment->length;
                segment->CombineForward(this);
                segment->CombineBackward(this);
                _usedHeapCount--;
                RemoveFromStack();
                return;
            }
            else
            {
                #ifdef _KERNEL_SRC
                return;
                #else
                Panic("Tried to free already free Segment!");
                #endif
                RemoveFromStack();
                return;
            }
        }
        else
        {
            #ifdef _KERNEL_SRC
            return;
            #else
            Panic("Tried to free invalid Segment!", true);
            #endif
            RemoveFromStack();
            return;
        }
        RemoveFromStack();
    }

    bool HeapManager::ExpandHeap(size_t length)
    {
        AddToStack();
        if (_lastHdr->next != NULL)
        {
            #ifdef _KERNEL_SRC
            return false;
            #else
            Panic("LAST HDR NEXT IS NOT NULL!", true);
            #endif
        }

        length += sizeof(_HeapSegHdr) + 0x100;

        if (length % 0x1000)
        {
            length -= (length % 0x1000);
            length += 0x1000;
        }

        size_t pageCount = length / 0x1000;
        void* tHeapEnd = _heapEnd;

        {
            void* tempAddr = requestNextPages(pageCount);
            if (tempAddr == NULL)
            {
                #ifdef _KERNEL_SRC
                return false;
                #else
                Panic("NO MORE RAM!!!!!!!", true);
                #endif
            }

            tHeapEnd = (void*)((uint64_t)tHeapEnd + pageCount * 0x1000);
        }

        _HeapSegHdr* newSegment = (_HeapSegHdr*) _heapEnd;

        
        newSegment->last = _lastHdr;
        _lastHdr->next = newSegment;
        _lastHdr = newSegment;
        
        newSegment->next = NULL;
        newSegment->length = length - sizeof(_HeapSegHdr);
        newSegment->free = true;

        newSegment->magicNum = HeapMagicNum;
        newSegment->activeMemFlagVal = _activeMemFlagVal;
        newSegment->time = 0;

        newSegment->text = "<FREE>";
        newSegment->file = "<NO FILE GIVEN>";
        newSegment->func = "<NO FUNC GIVEN>";
        newSegment->line = 0;
        
        _heapCount++;
        
        _heapEnd = tHeapEnd;

        if (newSegment == NULL)
        {
            #ifdef _KERNEL_SRC
            return false;
            #else
            Panic("NEW SEGMENT IS NULL!", true);
            #endif
        }

        newSegment->CombineBackward(this);
        RemoveFromStack();
        return true;
    }

    bool HeapManager::_XtryFree(void* address, const char* func, const char* file, int line)
    {
        if (address  < (void*)1000)
            return false;

        AddToStack();

        _HeapSegHdr* segment = ((_HeapSegHdr*)address) - 1;

        if (segment->magicNum == HeapMagicNum)
        {
            if (!segment->free)
            {
                segment->free = true;
                segment->text = "<FREE>";
                _usedHeapAmount -= segment->length;
                segment->CombineForward(this);
                segment->CombineBackward(this);
                _usedHeapCount--;
                
                RemoveFromStack();
                return true;
            }
            else
            {
                RemoveFromStack();
                return false;
            }
        }
        else
        {
            RemoveFromStack();
            return false;
        }
        RemoveFromStack();
        return true;
    }

}