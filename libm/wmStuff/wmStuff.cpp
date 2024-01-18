#include "wmStuff.h"
#include "../stubs.h"
#include "../syscallManager.h"
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>
#include <libm/cstr.h>
#include <libm/memStuff.h>

uint64_t desktopPID;

void initWindowManagerStuff()
{
    desktopPID = envGetDesktopPid();

    if (desktopPID == 0)
    {
        programCrash();
    }
}

void checkWindowManagerStuff()
{
    uint64_t tempDesktop = envGetDesktopPid();
    if (tempDesktop != desktopPID)
        programCrash();
}

// GenericMessagePacket* getWindowCreatePacket()
// {
//     for (int i = 0; i < yesWindowPackets->GetCount(); i++)
//     {
//         GenericMessagePacket* msg = yesWindowPackets->ElementAt(i);
//         if (msg->Type == MessagePacketType::WINDOW_CREATE_EVENT)
//         {
//             yesWindowPackets->RemoveAt(i);
//             return msg;
//         }   
//     }
//     return NULL;
// }

// GenericMessagePacket* getWindowGetPacket()
// {
//     for (int i = 0; i < yesWindowPackets->GetCount(); i++)
//     {
//         GenericMessagePacket* msg = yesWindowPackets->ElementAt(i);
//         if (msg->Type == MessagePacketType::WINDOW_GET_EVENT)
//         {
//             yesWindowPackets->RemoveAt(i);
//             return msg;
//         }   
//     }
//     return NULL;
// }

Window* getPartialWindow(uint64_t id)
{
    uint64_t convoId;
    {
        Window* tWin = new Window(0, 0, 0, 0, "", id, 0);
        WindowObjectPacket* winObj = new WindowObjectPacket(tWin, false);
        GenericMessagePacket* msg = winObj->ToGenericMessagePacket();
        convoId = msgSendConv(msg, desktopPID);
        tWin->Free();
        _Free(tWin);
        msg->Free();
        _Free(msg);
        winObj->Free();
        _Free(winObj);
    }

    GenericMessagePacket* winGet = msgWaitConv(convoId, 3000);

    if (winGet == NULL)
        return NULL;

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
    if (partialWindow != NULL)
    {
        window->UpdateUsingPartialWindow(partialWindow, true, true, true);
        partialWindow->Free();
        _Free(partialWindow);
    }
    
    window->UpdateCheck();
    window->Updates->Clear();

    // swallow all update packets
    for (int i = 0; i < 50; i++)
    {
        GenericMessagePacket* msg = msgGetConv(window->CONVO_ID_WM_WINDOW_UPDATE);
        if (msg == NULL)
            break;
        msg->Free();
        _Free(msg);
    }
}

void updateWindow(Window* window)
{
    Window* partialWindow = getPartialWindow(window->ID);
    if (partialWindow == NULL)
        return;
    window->UpdateUsingPartialWindow(partialWindow, true, true, true);
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
        _Free(winReq);
    }

    // TODO: maybe make it blocking and wait for a result
}
Window* requestWindow()
{
    return requestWindow(0);
}

Window* requestWindow(uint64_t id)
{
    uint64_t convoId;
    {
        GenericMessagePacket* winReq = new GenericMessagePacket(MessagePacketType::WINDOW_CREATE_EVENT, (uint8_t*)(&id), 8);
        convoId = msgSendConv(winReq, desktopPID);
        winReq->Free();
        _Free(winReq);
    }

    GenericMessagePacket* winCreate = msgWaitConv(convoId, 3000);
    if (winCreate == NULL)
        return NULL;
    
    uint64_t windowId = 0; // <GET WINDOW ID FROM PACKET>
    if (winCreate->Size >= 8)
        windowId = *((uint64_t*)winCreate->Data);


    Window* partial = getPartialWindow(windowId);
    if (partial == NULL)
    {
        winCreate->Free();
        _Free(winCreate);
        return NULL;
    }

    Window* newWindow = new Window(windowId);
    newWindow->UpdateUsingPartialWindow(partial, true, true, true);
    partial->Free();
    _Free(partial);

    newWindow->UpdateCheck();
    newWindow->Updates->Clear();

    winCreate->Free();
    _Free(winCreate);
    return newWindow;
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

    // for (int i = 0, y = y1; y <= y2; y++)
    //     for (int x = x1; x <= x2; x++, i++)
    //         buffer[i] = ((uint32_t*)window->Buffer->BaseAddress)[x + y * window->Buffer->Width];
    
    uint32_t* tBuff = (uint32_t*)window->Buffer->BaseAddress;
    for (int y = y1; y <= y2; y++)
        _memcpy(tBuff + window->Buffer->Width * y + x1, buffer + (y - y1) * width, width * 4);


    WindowBufferUpdatePacket* packet = new WindowBufferUpdatePacket(
        x1, y1, width, height,
        window->ID,
        buffer,
        true
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

bool CheckForWindowClosed(Window* window)
{
    if (window == NULL)
        return true;
    if (window->ID == 0)
        return true;
    if (window->CONVO_ID_WM_WINDOW_CLOSED == 0)
        return true;
    
    GenericMessagePacket* msg = msgGetConv(window->CONVO_ID_WM_WINDOW_CLOSED);
    if (msg == NULL)
        return false;
    
    msg->Free();
    _Free(msg);
    return true;
}