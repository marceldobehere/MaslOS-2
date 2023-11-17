#include "windowAttr.h"
#include <libm/cstrTools.h>
#include <libm/stubs.h>

bool SetWindowAttribute(Window* window, WindowAttributeType type, uint64_t val)
{
    if (window == NULL)
        return false;

    if (type == WindowAttributeType::POSITION_X)
    {
        window->Dimensions.x = *((int32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::POSITION_Y)
    {
        window->Dimensions.y = *((int32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SIZE_X)
    {
        window->Dimensions.width = *((uint32_t*)&val);
        if (window->Dimensions.width < 50)
            window->Dimensions.width = 50;
        return true;
    }
    if (type == WindowAttributeType::SIZE_Y)
    {
        window->Dimensions.height = *((uint32_t*)&val);
        if (window->Dimensions.height < 50)
            window->Dimensions.height = 50;
        return true;
    }
    if (type == WindowAttributeType::ID)
    {
        window->ID = *((int64_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
    {
        window->DefaultBorderColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
    {
        window->SelectedBorderColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
    {
        window->DefaultTitleColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
    {
        window->SelectedTitleColor = *((uint32_t*)&val);
        return true;
    }
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
    {
        window->DefaultTitleBackgroundColor = *((uint32_t*)&val);
        return true;
    }

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
    {
        window->ShowTitleBar = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
    {
        window->ShowBorder = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
    {
        window->Moveable = *((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
    {
        window->Hidden = !*((bool*)&val);
        return true;
    }
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
    {
        window->Resizeable = *((bool*)&val);
        return true;
    }

    if (type == WindowAttributeType::TITLE_TEXT)
    {
        const char* nT = StrCopy(*((const char**)&val));
        _Free((void*)window->Title);
        window->Title = nT;
        
        return true;
    }

    return false;
}

int GetWindowAttributeSize(WindowAttributeType type)
{
    if (type == WindowAttributeType::POSITION_X)
        return 4;
    if (type == WindowAttributeType::POSITION_Y)
        return 4;
    if (type == WindowAttributeType::SIZE_X)
        return 4;
    if (type == WindowAttributeType::SIZE_Y)
        return 4;

    if (type == WindowAttributeType::ID)
        return 8;

    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
        return 4;
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
        return 4;
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
        return 4;
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
        return 4;
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
        return 4;

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
        return 1;
    if (type == WindowAttributeType::IS_WINDOW_SELECTED)
        return 1;

    if (type == WindowAttributeType::TITLE_TEXT)
        return 8;

    return 0;
}

uint64_t GetWindowAttribute(Window* window, WindowAttributeType type)
{
    if (window == NULL)
        return false;

    uint8_t temp[8];
    for (int i = 0; i < 8; i++)
        temp[i] = 0;

    if (type == WindowAttributeType::POSITION_X)
        *((int32_t*)temp) = window->Dimensions.x;
    if (type == WindowAttributeType::POSITION_Y)
        *((int32_t*)temp) = window->Dimensions.y;
    if (type == WindowAttributeType::SIZE_X)
        *((uint32_t*)temp) = window->Dimensions.width;
    if (type == WindowAttributeType::SIZE_Y)
        *((uint32_t*)temp) = window->Dimensions.height;

    if (type == WindowAttributeType::ID)
        *((int64_t*)temp) = window->ID;

    if (type == WindowAttributeType::DEFAULT_BORDER_COLOR)
        *((uint32_t*)temp) = window->DefaultBorderColor;
    if (type == WindowAttributeType::SELECTED_BORDER_COLOR)
        *((uint32_t*)temp) = window->SelectedBorderColor;
    if (type == WindowAttributeType::DEFAULT_TITLE_COLOR)
        *((uint32_t*)temp) = window->DefaultTitleColor;
    if (type == WindowAttributeType::SELECTED_TITLE_COLOR)
        *((uint32_t*)temp) = window->SelectedTitleColor;
    if (type == WindowAttributeType::DEFAULT_TITLE_BACKGROUND_COLOR)
        *((uint32_t*)temp) = window->DefaultTitleBackgroundColor;

    if (type == WindowAttributeType::IS_TITLEBAR_SHOWN)
        *((bool*)temp) = window->ShowTitleBar;
    if (type == WindowAttributeType::IS_BORDER_SHOWN)
        *((bool*)temp) = window->ShowBorder;
    if (type == WindowAttributeType::IS_WINDOW_MOVEABLE)
        *((bool*)temp) = window->Moveable;
    if (type == WindowAttributeType::IS_WINDOW_SHOWN)
        *((bool*)temp) = !window->Hidden;
    if (type == WindowAttributeType::IS_WINDOW_RESIZEABLE)
        *((bool*)temp) = window->Resizeable;
    if (type == WindowAttributeType::IS_WINDOW_SELECTED)
        *((bool*)temp) = window->IsActive;

    if (type == WindowAttributeType::TITLE_TEXT)
        *((uint64_t*)temp) = (uint64_t)window->Title;


    return *((uint64_t*)temp);
}