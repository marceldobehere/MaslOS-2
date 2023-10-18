#include "msgPacket.h"
#include "../memStuff.h"
#include "../stubs.h"

GenericMessagePacket::GenericMessagePacket(MessagePacketType type, uint8_t* data, uint64_t size)
{
    Type = type;
    Size = size;
    Data = (uint8_t*)_Malloc(size, "Generic Message Data");
    FromPID = 0;
    _memcpy(data, Data, size);
}
GenericMessagePacket* GenericMessagePacket::Copy()
{
    GenericMessagePacket* packet = (GenericMessagePacket*)_Malloc(sizeof(GenericMessagePacket), "Generic Message Packet");
    *packet = GenericMessagePacket(Type, Data, Size);
    packet->FromPID = FromPID;
    return packet;   
}    
void GenericMessagePacket::Free()
{
    if (Data != NULL)
    {
        _Free(Data);    
        Data = NULL;
    }
}

GenericMessagePacket::GenericMessagePacket(MessagePacketType type, uint8_t* data, uint64_t size, Heap::HeapManager* manager)
{
    Type = type;
    Size = size;
    Data = (uint8_t*)manager->_Xmalloc(size, "Generic Message Data");
    FromPID = 0;
    _memcpy(data, Data, size);
}

GenericMessagePacket* GenericMessagePacket::Copy(Heap::HeapManager* manager)
{
    GenericMessagePacket* packet = (GenericMessagePacket*)manager->_Xmalloc(sizeof(GenericMessagePacket), "Generic Message Packet");
    *packet = GenericMessagePacket(Type, Data, Size, manager);
    packet->FromPID = FromPID;
    return packet;
}

void GenericMessagePacket::Free(Heap::HeapManager* manager)
{
    if (Data != NULL)
    {
        manager->_Xfree((void*)Data);
        Data = NULL;
    }
}