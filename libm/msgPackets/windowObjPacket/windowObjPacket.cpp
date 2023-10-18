#include "windowObjPacket.h"
#include <libm/memStuff.h>
#include <libm/stubs.h>


WindowObjectPacket::WindowObjectPacket(Window* window, bool set)
{
    this->PartialWindow = window;
    this->Set = set;
}
WindowObjectPacket::WindowObjectPacket(GenericMessagePacket* genericMessagePacket)
{
    PartialWindow = new Window(0, 0, 0, 0, "", 0, 0);
    Set = false;
    
    // TODO: load the window data lol
}
GenericMessagePacket* WindowObjectPacket::ToGenericMessagePacket()
{
    // TODO: actually like save the window data in the buffer and put that into the msg


    GenericMessagePacket* msg = NULL;
    if (Set)
        msg = new GenericMessagePacket(MessagePacketType::WINDOW_SET_EVENT, NULL, 0);
    else
        msg = new GenericMessagePacket(MessagePacketType::WINDOW_GET_EVENT, NULL, 0);

    return msg;
}  

void WindowObjectPacket::Free()
{
    
}