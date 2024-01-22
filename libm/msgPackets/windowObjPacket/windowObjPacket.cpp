#include "windowObjPacket.h"
#include <libm/memStuff.h>
#include <libm/stubs.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/syscallManager.h>


WindowObjectPacket::WindowObjectPacket(Window* window, bool set)
{
    this->PartialWindow = window;
    this->Set = set;
}

/*
    const char* Title;
    WindowDimension Dimensions = WindowDimension();
    {
        int x;
        int y;
        int width;
        int height;
    }

    bool ShowTitleBar;
    bool ShowBorder;
    bool Hidden;
    bool Moveable;
    bool Resizeable;
    bool Closeable;

    uint32_t DefaultBorderColor;
    uint32_t SelectedBorderColor;
    uint32_t DefaultTitleColor;
    uint32_t SelectedTitleColor;
    uint32_t DefaultTitleBackgroundColor;

    uint64_t ID;
    uint64_t PID;
*/

WindowObjectPacket::WindowObjectPacket(GenericMessagePacket* genericMessagePacket)
{
    PartialWindow = new Window(0, 0, 0, 0, "", 0, 0);
    
    uint8_t* buffer = genericMessagePacket->Data;
    uint8_t* tBuffer = buffer;

    // Set
    Set = *(uint8_t*)tBuffer;
    tBuffer += 1;

    // const char* Title;
    uint32_t titleLen = *(uint32_t*)tBuffer;
    tBuffer += 4;
    _Free((void*)PartialWindow->Title);
    PartialWindow->Title = (char*)_Malloc(titleLen + 1, "Window Title");
    _memcpy(tBuffer, (uint8_t*)PartialWindow->Title, titleLen);
    ((char*)PartialWindow->Title)[titleLen] = 0;
    tBuffer += titleLen;

    // WindowDimension Dimensions = WindowDimension();
    PartialWindow->Dimensions.x = *(uint32_t*)tBuffer;
    tBuffer += 4;
    PartialWindow->Dimensions.y = *(uint32_t*)tBuffer;
    tBuffer += 4;
    PartialWindow->Dimensions.width = *(uint32_t*)tBuffer;
    tBuffer += 4;
    PartialWindow->Dimensions.height = *(uint32_t*)tBuffer;
    tBuffer += 4;

    // bool ShowTitleBar;
    PartialWindow->ShowTitleBar = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool ShowBorder;
    PartialWindow->ShowBorder = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool Hidden;
    PartialWindow->Hidden = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool Moveable;
    PartialWindow->Moveable = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool Resizeable;
    PartialWindow->Resizeable = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool Closeable;
    PartialWindow->Closeable = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool IsActive
    PartialWindow->IsActive = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool IsFrozen
    PartialWindow->IsFrozen = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool CaptureMouse
    PartialWindow->CaptureMouse = *(uint8_t*)tBuffer;
    tBuffer += 1;
    // bool IsCapturing
    PartialWindow->IsCapturing = *(uint8_t*)tBuffer;
    tBuffer += 1;

    // uint32_t DefaultBorderColor;
    PartialWindow->DefaultBorderColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t SelectedBorderColor;
    PartialWindow->SelectedBorderColor = *(uint32_t*)tBuffer;   
    tBuffer += 4;
    // uint32_t DefaultTitleColor;
    PartialWindow->DefaultTitleColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t SelectedTitleColor;
    PartialWindow->SelectedTitleColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t DefaultTitleBackgroundColor;
    PartialWindow->DefaultTitleBackgroundColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t CurrentBorderColor
    PartialWindow->CurrentBorderColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t CurrentTitleColor
    PartialWindow->CurrentTitleColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t CurrentTitleBackgroundColor
    PartialWindow->CurrentTitleBackgroundColor = *(uint32_t*)tBuffer;
    tBuffer += 4;
    // uint32_t DefaultBackgroundColor
    PartialWindow->DefaultBackgroundColor = *(uint32_t*)tBuffer;
    tBuffer += 4;

    // uint64_t ID;
    PartialWindow->ID = *(uint64_t*)tBuffer;
    tBuffer += 8;
    // uint64_t PID;
    PartialWindow->PID = *(uint64_t*)tBuffer;
    tBuffer += 8;

    // uint64_t CONVO_ID_WM_MOUSE_STUFF
    PartialWindow->CONVO_ID_WM_MOUSE_STUFF = *(uint64_t*)tBuffer;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_KB_STUFF
    PartialWindow->CONVO_ID_WM_KB_STUFF = *(uint64_t*)tBuffer;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_WINDOW_UPDATE
    PartialWindow->CONVO_ID_WM_WINDOW_UPDATE = *(uint64_t*)tBuffer;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_WINDOW_CLOSED
    PartialWindow->CONVO_ID_WM_WINDOW_CLOSED = *(uint64_t*)tBuffer;
    tBuffer += 8;
}
GenericMessagePacket* WindowObjectPacket::ToGenericMessagePacket()
{
    if (PartialWindow == NULL)
        Panic("WAAA WINDOW IS NULL", true);
    int count = 0;

    // Set
    count += 1;

    // const char* Title;
    count += 4;
    count += StrLen(PartialWindow->Title);
    
    // WindowDimension Dimensions = WindowDimension();
    count += 4 * 4;
    // bool ShowTitleBar;
    count += 1;
    // bool ShowBorder;
    count += 1;
    // bool Hidden;
    count += 1;
    // bool Moveable;
    count += 1;
    // bool Resizeable;
    count += 1;
    // bool Closeable;
    count += 1;
    // bool IsActive
    count += 1;
    // bool IsFrozen
    count += 1;
    // bool CaptureMouse
    count += 1;
    // bool IsCapturing
    count += 1;
    // uint32_t DefaultBorderColor;
    count += 4;
    // uint32_t SelectedBorderColor;
    count += 4;
    // uint32_t DefaultTitleColor;
    count += 4;
    // uint32_t SelectedTitleColor;
    count += 4;
    // uint32_t DefaultTitleBackgroundColor;
    count += 4;
    // uint32_t CurrentBorderColor
    count += 4;
    // uint32_t CurrentTitleColor
    count += 4;
    // uint32_t CurrentTitleBackgroundColor
    count += 4;
    // uint32_t DefaultBackgroundColor
    count += 4;


    // uint64_t ID;
    count += 8;
    // uint64_t PID;
    count += 8;

    // uint64_t CONVO_ID_WM_MOUSE_STUFF
    count += 8;
    // uint64_t CONVO_ID_WM_KB_STUFF
    count += 8;
    // uint64_t CONVO_ID_WM_WINDOW_UPDATE
    count += 8;
    // uint64_t CONVO_ID_WM_WINDOW_CLOSED
    count += 8;

    GenericMessagePacket* msg = NULL;
    if (Set)
        msg = new GenericMessagePacket(count, MessagePacketType::WINDOW_SET_EVENT);
    else
        msg = new GenericMessagePacket(count, MessagePacketType::WINDOW_GET_EVENT);
    
    uint8_t* tBuffer = msg->Data;

    // Set
    *(uint8_t*)tBuffer = Set;
    tBuffer += 1;
    
    // const char* Title;
    *(uint32_t*)tBuffer = StrLen(PartialWindow->Title);
    tBuffer += 4;
    _memcpy((uint8_t*)PartialWindow->Title, tBuffer, StrLen(PartialWindow->Title));
    tBuffer += StrLen(PartialWindow->Title);

    // WindowDimension Dimensions = WindowDimension();
    *(uint32_t*)tBuffer = PartialWindow->Dimensions.x;
    tBuffer += 4;
    *(uint32_t*)tBuffer = PartialWindow->Dimensions.y;
    tBuffer += 4;
    *(uint32_t*)tBuffer = PartialWindow->Dimensions.width;
    tBuffer += 4;
    *(uint32_t*)tBuffer = PartialWindow->Dimensions.height;
    tBuffer += 4;

    // bool ShowTitleBar;
    *(uint8_t*)tBuffer = PartialWindow->ShowTitleBar;
    tBuffer += 1;
    // bool ShowBorder;
    *(uint8_t*)tBuffer = PartialWindow->ShowBorder;
    tBuffer += 1;
    // bool Hidden;
    *(uint8_t*)tBuffer = PartialWindow->Hidden;
    tBuffer += 1;
    // bool Moveable;
    *(uint8_t*)tBuffer = PartialWindow->Moveable;
    tBuffer += 1;
    // bool Resizeable;
    *(uint8_t*)tBuffer = PartialWindow->Resizeable;
    tBuffer += 1;
    // bool Closeable;
    *(uint8_t*)tBuffer = PartialWindow->Closeable;
    tBuffer += 1;
    // bool IsActive
    *(uint8_t*)tBuffer = PartialWindow->IsActive;
    tBuffer += 1;
    // bool IsFrozen
    *(uint8_t*)tBuffer = PartialWindow->IsFrozen;
    tBuffer += 1;
    // bool CaptureMouse
    *(uint8_t*)tBuffer = PartialWindow->CaptureMouse;
    tBuffer += 1;
    // bool IsCapturing
    *(uint8_t*)tBuffer = PartialWindow->IsCapturing;
    tBuffer += 1;

    // uint32_t DefaultBorderColor;
    *(uint32_t*)tBuffer = PartialWindow->DefaultBorderColor;
    tBuffer += 4;
    // uint32_t SelectedBorderColor;
    *(uint32_t*)tBuffer = PartialWindow->SelectedBorderColor;
    tBuffer += 4;
    // uint32_t DefaultTitleColor;
    *(uint32_t*)tBuffer = PartialWindow->DefaultTitleColor;
    tBuffer += 4;
    // uint32_t SelectedTitleColor;
    *(uint32_t*)tBuffer = PartialWindow->SelectedTitleColor;
    tBuffer += 4;
    // uint32_t DefaultTitleBackgroundColor;
    *(uint32_t*)tBuffer = PartialWindow->DefaultTitleBackgroundColor;
    tBuffer += 4;
    // uint32_t CurrentBorderColor
    *(uint32_t*)tBuffer = PartialWindow->CurrentBorderColor;
    tBuffer += 4;
    // uint32_t CurrentTitleColor
    *(uint32_t*)tBuffer = PartialWindow->CurrentTitleColor;
    tBuffer += 4;
    // uint32_t CurrentTitleBackgroundColor
    *(uint32_t*)tBuffer = PartialWindow->CurrentTitleBackgroundColor;
    tBuffer += 4;
    // uint32_t DefaultBackgroundColor
    *(uint32_t*)tBuffer = PartialWindow->DefaultBackgroundColor;
    tBuffer += 4;

    // uint64_t ID;
    *(uint64_t*)tBuffer = PartialWindow->ID;
    tBuffer += 8;
    // uint64_t PID;
    *(uint64_t*)tBuffer = PartialWindow->PID;
    tBuffer += 8;
    
    // uint64_t CONVO_ID_WM_MOUSE_STUFF
    *(uint64_t*)tBuffer = PartialWindow->CONVO_ID_WM_MOUSE_STUFF;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_KB_STUFF
    *(uint64_t*)tBuffer = PartialWindow->CONVO_ID_WM_KB_STUFF;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_WINDOW_UPDATE
    *(uint64_t*)tBuffer = PartialWindow->CONVO_ID_WM_WINDOW_UPDATE;
    tBuffer += 8;
    // uint64_t CONVO_ID_WM_WINDOW_CLOSED
    *(uint64_t*)tBuffer = PartialWindow->CONVO_ID_WM_WINDOW_CLOSED;
    tBuffer += 8;

    return msg;
}

void WindowObjectPacket::Free()
{
    
}