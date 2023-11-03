#include "wmStuff.h"
#include "../stubs.h"
#include "../syscallManager.h"
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>

List<GenericMessagePacket*>* nonWindowPackets;
List<GenericMessagePacket*>* yesWindowPackets;
uint64_t desktopPID;

void initWindowManagerStuff()
{
    nonWindowPackets = new List<GenericMessagePacket*>();
    yesWindowPackets = new List<GenericMessagePacket*>();
    desktopPID = envGetDesktopPid();
}

GenericMessagePacket* getWindowCreatePacket()
{
    for (int i = 0; i < yesWindowPackets->GetCount(); i++)
    {
        GenericMessagePacket* msg = yesWindowPackets->ElementAt(i);
        if (msg->Type == MessagePacketType::WINDOW_CREATE_EVENT)
        {
            yesWindowPackets->RemoveAt(i);
            return msg;
        }   
    }
    return NULL;
}

GenericMessagePacket* getWindowGetPacket()
{
    for (int i = 0; i < yesWindowPackets->GetCount(); i++)
    {
        GenericMessagePacket* msg = yesWindowPackets->ElementAt(i);
        if (msg->Type == MessagePacketType::WINDOW_GET_EVENT)
        {
            yesWindowPackets->RemoveAt(i);
            return msg;
        }   
    }
    return NULL;
}

Window* getPartialWindow(uint64_t id)
{
    {
        Window* tWin = new Window(0, 0, 0, 0, "", id, 0);
        WindowObjectPacket* winObj = new WindowObjectPacket(tWin, false);
        GenericMessagePacket* msg = winObj->ToGenericMessagePacket();
        msgSendMessage(msg, desktopPID);
        tWin->Free();
        _Free(tWin);
        msg->Free();
        _Free(msg);
        winObj->Free();
        _Free(winObj);
    }

    GenericMessagePacket* winGet = NULL;
    while (true)
    {
        programYield();
        handleWindowPackets();
        winGet = getWindowGetPacket();
        if (winGet != NULL)
            break;
    }

    WindowObjectPacket* gotObj = new WindowObjectPacket(winGet);
    Window* window = gotObj->PartialWindow;
    gotObj->Free();
    _Free(gotObj);

    winGet->Free();
    _Free(winGet);
    return window;
}

void setWindow(Window* window)
{
    setWindow(window->ID, window);
}

void setWindow(uint64_t id, Window* window)
{
    {
        WindowObjectPacket* winObj = new WindowObjectPacket(window, true);
        GenericMessagePacket* msg = winObj->ToGenericMessagePacket();
        msgSendMessage(msg, desktopPID);
        msg->Free();
        _Free(msg);
        winObj->Free();
        _Free(winObj);
    }

    // after sending the window set, we need to update the window object 
    // to avoid changes that were not permitted by the wm (keep it synchronised)
    Window* partialWindow = getPartialWindow(id);
    window->UpdateUsingPartialWindow(partialWindow, true);
    window->UpdateCheck();
    window->Updates->Clear();
    partialWindow->Free();
    _Free(partialWindow);
}

void updateWindow(Window* window)
{
    Window* partialWindow = getPartialWindow(window->ID);
    window->UpdateUsingPartialWindow(partialWindow, true);
    window->UpdateCheck();
    window->Updates->Clear();
    partialWindow->Free();
    _Free(partialWindow);
}

void deleteWindow(Window* window)
{
    deleteWindow(window->ID);
}

void deleteWindow(uint64_t id)
{
    {
        uint64_t data = id;
        GenericMessagePacket* winReq = new GenericMessagePacket(MessagePacketType::WINDOW_DELETE_EVENT, (uint8_t*)&data, 8);
        msgSendMessage(winReq, desktopPID);
        winReq->Free();
    }

    // TODO: maybe make it blocking and wait for a result
}

Window* requestWindow()
{
    {
        GenericMessagePacket* winReq = new GenericMessagePacket(MessagePacketType::WINDOW_CREATE_EVENT, NULL, 0);
        msgSendMessage(winReq, desktopPID);
        winReq->Free();
    }

    GenericMessagePacket* winCreate = NULL;
    while (true)
    {
        programYield();
        handleWindowPackets();
        winCreate = getWindowCreatePacket();
        if (winCreate != NULL)
            break;
    }
    
    uint64_t windowId = 0; // <GET WINDOW ID FROM PACKET>
    if (winCreate->Size >= 8)
        windowId = *((uint64_t*)winCreate->Data);


    Window* partial = getPartialWindow(windowId);

    Window* newWindow = new Window(0, 0, 0, 0, "", windowId, 0);
    newWindow->UpdateUsingPartialWindow(partial, true);
    newWindow->UpdateCheck();
    newWindow->Updates->Clear();

    winCreate->Free();
    _Free(winCreate);
    return newWindow;
}

void handleWindowPackets()
{
    while (true)
    {
        if (msgGetCount() == 0)
            return;
        
        GenericMessagePacket* msg = msgGetMessage();
        if (msg == NULL)
            return;

        if (
            msg->Type == MessagePacketType::WINDOW_BUFFER_EVENT ||
            msg->Type == MessagePacketType::WINDOW_CREATE_EVENT ||
            msg->Type == MessagePacketType::WINDOW_GET_EVENT ||
            msg->Type == MessagePacketType::WINDOW_SET_EVENT
        )
            yesWindowPackets->Add(msg);
        else 
            nonWindowPackets->Add(msg);
    }
}

#include "../msgPackets/windowBufferUpdatePacket/windowBufferUpdatePacket.h"

bool SendWindowFrameBufferUpdate(Window* window, int x1, int y1, int x2, int y2)
{
    if (window == NULL)
        return false;
    if (window->Buffer == NULL)
        return false;
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 > window->Buffer->Width - 1)
        x2 = window->Buffer->Width - 1;
    if (y2 > window->Buffer->Height - 1)
        y2 = window->Buffer->Height - 1;
    if (x1 > x2)
        return false;
    if (y1 > y2)
        return false;

    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;

    uint32_t* buffer = (uint32_t*)_Malloc(width * height * 4);

    for (int i = 0, y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++, i++)
            buffer[i] = ((uint32_t*)window->Buffer->BaseAddress)[x + y * window->Buffer->Width];
    

    WindowBufferUpdatePacket* packet = new WindowBufferUpdatePacket(
        x1, y1, width, height,
        window->ID,
        buffer
    );

    GenericMessagePacket* msg = packet->ToGenericMessagePacket();

    bool res = msgSendMessage(msg, desktopPID);

    msg->Free();
    _Free(msg);

    packet->Free();
    _Free(packet); 

    _Free(buffer);

    return res;
}

bool SendWindowFrameBufferUpdate(Window* window)
{
    if (window == NULL)
        return false;
    if (window->Buffer == NULL)
        return false;
    
    WindowBufferUpdatePacket* packet = new WindowBufferUpdatePacket(
        0, 0, window->Dimensions.width, window->Dimensions.height,
        window->ID,
        (uint32_t*)window->Buffer->BaseAddress
    );

    GenericMessagePacket* msg = packet->ToGenericMessagePacket();

    bool res = msgSendMessage(msg, desktopPID);

    msg->Free();
    _Free(msg);

    packet->Free();
    _Free(packet); 

    return res;
}