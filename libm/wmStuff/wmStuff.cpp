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
