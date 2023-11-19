#include "guiInstance.h"
#include "guiStuff/components/box/boxComponent.h"
#include "guiStuff/components/button/buttonComponent.h"
#include "guiStuff/components/screenComponent/screenComponent.h"
#include "guiStuff/components/imageRect/imageRectangleComponent.h"
#include "guiStuff/components/textField/textFieldComponent.h"
#include "guiStuff/components/canvas/canvasComponent.h"
#include "guiStuff/components/advancedText/advancedTextComponent.h"
#include <libm/rendering/Cols.h>
#include <libm/wmStuff/wmStuff.h>


// #include "../../../memory/heap.h"
// #include "../../../cStdLib/cstrTools.h"
// #include "../../..//devices/serial/serial.h"

GuiInstance::GuiInstance(Window* window)
{
    // waitTask = NULL;
    // waitTask2 = NULL;
    // waitingForTask = false;
    // OnWaitTaskDoneCallback = NULL;
    // OnWaitTaskDoneHelp = NULL;
    this->window = window;
    oldResizeable = false;
    instanceType = InstanceType::GUI;
    FreeFunc = (void(*)(void*))&Free;
}

#include <libm/memStuff.h>

void GuiInstance::Init()
{
    //window->renderer->Clear(Colors.white);
    allComponents = new List<GuiComponentStuff::BaseComponent*>(10);
    screen = new GuiComponentStuff::ScreenComponent(window, this);
    allComponents->Add(screen);
    screen->id = 1234;
    //window->renderer->Clear(Colors.white);
    _memset(window->Buffer->BaseAddress, 0, window->Buffer->BufferSize);
    
    screen->CheckUpdates();

    GuiComponentStuff::ComponentFramebuffer bruhus = GuiComponentStuff::ComponentFramebuffer
        (
            window->Buffer->Width,
            window->Buffer->Height,
            (uint32_t*)window->Buffer->BaseAddress
        );

    screen->renderer->Render(
        screen->position, 
        GuiComponentStuff::Field(
            GuiComponentStuff::Position(), 
            screen->GetActualComponentSize()
        ), 
        &bruhus
    );

    window->DefaultBackgroundColor = screen->backgroundColor;
    setWindow(window);
}

int GetBaseComponentAttributeSize(GuiInstanceBaseAttributeType type)
{
    if (type == GuiInstanceBaseAttributeType::POSITION_X)
        return 4;
    if (type == GuiInstanceBaseAttributeType::POSITION_Y)
        return 4;
    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_X)
        return 4;
    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_Y)
        return 4;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_X)
        return 8;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_Y)
        return 8;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_X)
        return 1;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y)
        return 1;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_X)
        return 4;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_Y)
        return 4;

    if (type == GuiInstanceBaseAttributeType::ID)
        return 8;
    if (type == GuiInstanceBaseAttributeType::PARENT_ID)
        return 8;
    if (type == GuiInstanceBaseAttributeType::IS_HIDDEN)
        return 1;

    return 0;
}

#ifndef _KERNEL_SRC
#include <libm/heap/heap.h>
#endif
#ifdef _KERNEL_SRC
#include "../../kernel/memory/heap.h"
#endif

#include <libm/syscallManager.h>
#include <libm/cstr.h>
#include <libm/msgPackets/mousePacket/mousePacket.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>

void GuiInstance::Update()
{
    if (screen == NULL)
        return;

    // Window Updates
    if (true)
    {
        bool updateEverHappened = false;
        for (int i = 0; i < 500; i++)
        {
            GenericMessagePacket* wPacket = msgGetConv(window->CONVO_ID_WM_WINDOW_UPDATE);
            if (wPacket != NULL)
            {
                if (wPacket->FromPID == desktopPID)
                {
                    WindowObjectPacket* gotObj = new WindowObjectPacket(wPacket);
                    Window* partialWindow = gotObj->PartialWindow;
                    gotObj->Free();
                    _Free(gotObj);

                    if (partialWindow != NULL)
                    {
                        window->UpdateUsingPartialWindow(partialWindow, true, true, true);
                        updateEverHappened = true;
                        partialWindow->Free();
                        _Free(partialWindow);
                    }
                }
                
                wPacket->Free();
                _Free(wPacket);
            }
            else
                break;
        }
        if (updateEverHappened)
        {
            window->UpdateCheck();
            window->Updates->Clear();
        }
    }
    else
    {
        updateWindow(window);
    }

    // Update Mouse Position
    {
        MouseState* temp = envGetMouseState();
        if (temp != NULL)
        {
            mouseState = *temp;
            mouseState.MouseX -= window->Dimensions.x;
            mouseState.MouseY -= window->Dimensions.y;
            _Free(temp);
        }
    }
    
    // Keyboard Events
    for (int i = 0; i < 500; i++)
    {
        GenericMessagePacket* mPacket = msgGetConv(window->CONVO_ID_WM_KB_STUFF);
        if (mPacket != NULL)
        {
            if (mPacket->Size >= sizeof(KeyMessagePacket))
            {
                KeyMessagePacket* kbMsg = (KeyMessagePacket*)mPacket->Data;
                
                if (kbMsg->Type == KeyMessagePacketType::KEY_PRESSED)
                    screen->KeyHit(GuiComponentStuff::KeyHitEventInfo(kbMsg->Scancode, kbMsg->KeyChar));
            }

            mPacket->Free();
            _Free(mPacket);
        }
        else
            break;
    }

    // Mouse Events
    for (int i = 0; i < 500; i++)
    {
        GenericMessagePacket* mPacket = msgGetConv(window->CONVO_ID_WM_MOUSE_STUFF);
        if (mPacket != NULL)
        {
            if (mPacket->Size >= sizeof(MouseMessagePacket))
            {
                MouseMessagePacket* mouseMsg = (MouseMessagePacket*)mPacket->Data;
                if (mouseMsg->Type == MouseMessagePacketType::MOUSE_CLICK)
                {
                    GuiComponentStuff::MouseClickEventInfo info = GuiComponentStuff::MouseClickEventInfo(
                        GuiComponentStuff::Position(
                            mouseMsg->MouseX - window->Dimensions.x, 
                            mouseMsg->MouseY - window->Dimensions.y
                        ),
                        mouseMsg->Left, mouseMsg->Right, mouseMsg->Middle
                    );
                    screen->MouseClicked(info);
                }
            }

            mPacket->Free();
            _Free(mPacket);
        }
        else
            break;
    }
}

void GuiInstance::Render(bool update)
{
    if (screen == NULL)
        return;
    
    if (update)
        Update();

    screen->CheckUpdates();

    //serialPrint("> Updates: ");
    //serialPrintLn(to_string(screen->finalUpdatedFields->GetCount()));

    int x1, y1, x2, y2;
    int ox1, oy1, ox2, oy2;
    bool set = false;
    int counter = 0;
    while (screen->finalUpdatedFields->GetCount() > 0)
    {
        GuiComponentStuff::Field bruh = screen->finalUpdatedFields->LastElement();
        screen->finalUpdatedFields->RemoveLast();

        GuiComponentStuff::ComponentFramebuffer bruhus = GuiComponentStuff::ComponentFramebuffer
            (
                window->Buffer->Width,
                window->Buffer->Height,
                (uint32_t*)window->Buffer->BaseAddress
            );

        screen->renderer->Render(screen->position, bruh, &bruhus);

        if (true)
        {
            if (!set)
            {
                x1 = bruh.TL.x;
                y1 = bruh.TL.y;
                x2 = bruh.BR.x;
                y2 = bruh.BR.y;

                ox1 = x1;
                oy1 = y1;
                ox2 = x2;
                oy2 = y2;

                set = true;
            }

            if (x1 > bruh.TL.x)
                x1 = bruh.TL.x;
            if (y1 > bruh.TL.y)
                y1 = bruh.TL.y;
            if (x2 < bruh.BR.x)
                x2 = bruh.BR.x;
            if (y2 < bruh.BR.y)
                y2 = bruh.BR.y;

            int oSize = (ox2 - ox1) * (oy2 - oy1);
            int nSize = (x2 - x1) * (y2 - y1);

            if (nSize < oSize * 4)
            {
                ox1 = x1;
                oy1 = y1;
                ox2 = x2;
                oy2 = y2;
            }
            else
            {
                SendWindowFrameBufferUpdate(window, ox1, oy1, ox2, oy2);
                SendWindowFrameBufferUpdate(window, bruh.TL.x, bruh.TL.y, bruh.BR.x, bruh.BR.y);

                set = false;
                counter = 0;
            }
        }
        else
        {
            //send update
            SendWindowFrameBufferUpdate(window, bruh.TL.x, bruh.TL.y, bruh.BR.x, bruh.BR.y);
        }
        
        if (++counter > 20)
        {
            counter = 0;
            if (set)
            {
                SendWindowFrameBufferUpdate(window, x1, y1, x2, y2);
                set = false;
            }
        }
    }

    if (set)
    {
        SendWindowFrameBufferUpdate(window, x1, y1, x2, y2);
        set = false;
    }
}


GuiComponentStuff::BaseComponent* GuiInstance::GetComponentFromId(uint64_t id)
{
    if (screen == NULL)
        return NULL;

    if (screen->id == id)
        return screen;

    for (int i = 0; i < allComponents->GetCount(); i++)
    {
        GuiComponentStuff::BaseComponent* comp = allComponents->ElementAt(i);
        if (comp == NULL)
            continue;
        if (comp->id == id)
            return comp;
    }

    return NULL;
}

GuiComponentStuff::BaseComponent* GuiInstance::GetChildFromComponentWithId(uint64_t id, int index)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return NULL;
    if (index < 0)
        return NULL;

    if (base->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)base;
        if (index >= box->children->GetCount())
            return NULL;
        return box->children->ElementAt(index);
    }

    if (base->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)base;
        if (index >= scr->children->GetCount())
            return NULL;
        return scr->children->ElementAt(index);
    }

    if (base->componentType == GuiComponentStuff::ComponentType::BUTTON)
    {
        GuiComponentStuff::ButtonComponent* btn = (GuiComponentStuff::ButtonComponent*)base;
        if (index >= 1)
            return NULL;
        return btn->actualButtonStuff;
    }

    if (base->componentType == GuiComponentStuff::ComponentType::TEXTFIELD)
    {
        GuiComponentStuff::TextFieldComponent* txt = (GuiComponentStuff::TextFieldComponent*)base;
        if (index >= 1)
            return NULL;
        return txt->actualTextFieldStuff;
    }

    return NULL;
}

int GuiInstance::GetIndexOfChildFromComponentWithId(uint64_t id, uint64_t childId)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    GuiComponentStuff::BaseComponent* child = GetComponentFromId(childId);
    if (base == NULL)
        return -1;
    if (child == NULL)
        return -1;

    

    if (base->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)base;
        return box->children->GetIndexOf(child);
    }

    if (base->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)base;
        return scr->children->GetIndexOf(child);
    }

    return -1;
}

bool GuiInstance::RemoveChildFromComponentWithId(uint64_t id, int index)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    
    if (base == NULL)
        return false;
    if (index < 0)
        return false;
        

    if (base->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)base;
        if (index >= box->children->GetCount() || index < 0)
            return false;
        box->children->RemoveAt(index);
        return true;
    }

    if (base->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)base;
        if (index >= scr->children->GetCount())
            return false;
        scr->children->RemoveAt(index);
        return true;
    }

    return false;
}

//, void (*callBackFunc)(BaseComponent* comp)

GuiInstance* currentInst;

void RemoveThingFromList(GuiComponentStuff::BaseComponent* comp)
{
    int indx = currentInst->allComponents->GetIndexOf(comp);
    if (indx != -1)
        currentInst->allComponents->RemoveAt(indx);
}

//#include "../../../osData/MStack/MStackM.h"
#include <libm/stubs.h>

void GuiInstance::Free() 
{
    AddToStack();
    GuiComponentStuff::ScreenComponent* tScreen = screen;
    screen = NULL;
    currentInst = this;
    AddToStack();
    RemoveThingFromList(tScreen);
    RemoveFromStack();

    AddToStack();
    tScreen->Destroy(true, RemoveThingFromList);
    RemoveFromStack();
    _Free(tScreen);
    
    AddToStack();
    for (int i = 0; i < allComponents->GetCount(); i++)
    {
        if (allComponents->ElementAt(i)->componentType != GuiComponentStuff::ComponentType::SCREEN)
            continue;
        GuiComponentStuff::ScreenComponent* bruh = (GuiComponentStuff::ScreenComponent*)allComponents->ElementAt(i);
        AddToStack();
        RemoveThingFromList(bruh);
        RemoveFromStack();
        AddToStack();
        bruh->Destroy(true, RemoveThingFromList);
        RemoveFromStack();
        AddToStack();
        _Free(bruh);
        RemoveFromStack();
        i = -1;
    }
    RemoveFromStack();

    allComponents->Free();
    _Free(allComponents);


    RemoveFromStack();
}

bool GuiInstance::DeleteComponentWithId(int64_t id, bool destroyChildren)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;
    if (screen == base)
        return false;
    currentInst = this;

    if (screen->tempSelectedComponent == base)
        screen->tempSelectedComponent = NULL;
    if (screen->selectedComponent == base)
        screen->selectedComponent = NULL;
  
    bool res = base->Destroy(destroyChildren, RemoveThingFromList);
    RemoveThingFromList(base);
    _Free(base);
    return res;
}


bool GuiInstance::SetActiveScreenFromId(int64_t id)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;
    if (base->componentType != GuiComponentStuff::ComponentType::SCREEN)
        return false;

    screen = (GuiComponentStuff::ScreenComponent*)base;
    return true;
}

bool GuiInstance::SetSpecificComponentAttribute(int64_t id, int32_t type, uint64_t val)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;

    return base->SetAttribute(type, val);
}

uint64_t GuiInstance::GetSpecificComponentAttribute(int64_t id, int32_t type)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return 0;

    return base->GetAttribute(type);
}
int GuiInstance::GetSpecificComponentAttributeSize(int64_t id, int32_t type)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return 0;

    return base->GetAttributeSize(type);
}

bool GuiInstance::CreateComponentWithId(int64_t id, GuiComponentStuff::ComponentType type)
{
    if (screen == NULL)
        return false;
    return CreateComponentWithIdAndParent(id, type, screen->id);
}




bool GuiInstance::SetBaseComponentAttribute(int64_t id, GuiInstanceBaseAttributeType type, uint64_t val)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return false;

    if (type == GuiInstanceBaseAttributeType::POSITION_X)
    {
        base->position.x = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::POSITION_Y)
    {
        base->position.y = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_X)
    {
        base->size.FixedX = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_Y)
    {
        base->size.FixedY = *((int32_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_X)
    {
        //base->GetActualComponentSize().FixedX = *((int32_t*)&val);
        return false;
    }

    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_Y)
    {
        //base->GetActualComponentSize().FixedY = *((int32_t*)&val);
        return false;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_X)
    {
        base->size.ScaledX = *((double*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_Y)
    {
        base->size.ScaledY = *((double*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_X)
    {
        base->size.IsXFixed = *((bool*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y)
    {
        base->size.IsYFixed = *((bool*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::ID)
    {
        base->id = *((int64_t*)&val);
        return true;
    }

    if (type == GuiInstanceBaseAttributeType::PARENT_ID)
    {
        return false;
        // if (base->parent == NULL)
        //     return false;

        // base->parent->id = *((int64_t*)&val);
        // return true;
    }

    if (type == GuiInstanceBaseAttributeType::IS_HIDDEN)
    {
        base->hidden= *((bool*)&val);
        return true;
    }

    return false;
}

uint64_t GuiInstance::GetBaseComponentAttribute(int64_t id, GuiInstanceBaseAttributeType type)
{
    GuiComponentStuff::BaseComponent* base = GetComponentFromId(id);
    if (base == NULL)
        return 0;

    uint8_t temp[8];
    for (int i = 0; i < 8; i++)
        temp[i] = 0;

    if (type == GuiInstanceBaseAttributeType::POSITION_X)
        *((int32_t*)temp) = base->position.x;
    if (type == GuiInstanceBaseAttributeType::POSITION_Y)
        *((int32_t*)temp) = base->position.y;

    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_X)
        *((int32_t*)temp) = base->size.FixedX;
    if (type == GuiInstanceBaseAttributeType::SIZE_FIXED_Y)
        *((int32_t*)temp) = base->size.FixedY;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_X)
            *((int32_t*)temp) = base->GetActualComponentSize().FixedX;
    if (type == GuiInstanceBaseAttributeType::ACTUAL_SIZE_Y)
            *((int32_t*)temp) = base->GetActualComponentSize().FixedY;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_X)
        *((double*)temp) = base->size.ScaledX;
    if (type == GuiInstanceBaseAttributeType::SIZE_SCALED_Y)
        *((double*)temp) = base->size.ScaledY;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_X)
        *((bool*)temp) = base->size.IsXFixed;
    if (type == GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y)
        *((bool*)temp) = base->size.IsYFixed;

    if (type == GuiInstanceBaseAttributeType::ID)
        *((int64_t*)temp) = base->id;
    if (type == GuiInstanceBaseAttributeType::PARENT_ID)
        if (base->parent != NULL)
                *((int64_t*)temp) =  base->parent->id;
    if (type == GuiInstanceBaseAttributeType::IS_HIDDEN)
        *((bool*)temp) = base->hidden;

    return *((uint64_t*)temp);
}


bool GuiInstance::ComponentAddChild(int64_t id, GuiComponentStuff::BaseComponent* childComp)
{
    GuiComponentStuff::BaseComponent* parentComp = GetComponentFromId(id);
    //GuiComponentStuff::BaseComponent* childComp = GetComponentFromId(childId);
    if (parentComp == NULL || childComp == NULL)
        return false;
    
    if (parentComp->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)parentComp;
        box->children->Add(childComp);
        return true;
    }
    if (parentComp->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)parentComp;
        scr->children->Add(childComp);
        return true;
    }

    return false;
}

bool GuiInstance::ComponentRemoveChild(int64_t id, int64_t childId)
{
    GuiComponentStuff::BaseComponent* parentComp = GetComponentFromId(id);
    GuiComponentStuff::BaseComponent* childComp = GetComponentFromId(childId);
    if (parentComp == NULL || childComp == NULL)
        return false;
    
    if (parentComp->componentType == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* box = (GuiComponentStuff::BoxComponent*)parentComp;
        int indx = box->children->GetIndexOf(childComp);
        if (indx == -1)
            return false;
        box->children->RemoveAt(indx);
        return true;
    }
    if (parentComp->componentType == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = (GuiComponentStuff::ScreenComponent*)parentComp;
        int indx = scr->children->GetIndexOf(childComp);
        if (indx == -1)
            return false;
        scr->children->RemoveAt(indx);
        return true;
    }


    return false;
}



bool GuiInstance::CreateComponentWithIdAndParent(int64_t id, GuiComponentStuff::ComponentType type, int64_t parentId)
{
    if (GetComponentFromId(id) != NULL)
        return false;
    GuiComponentStuff::BaseComponent* parentComp = GetComponentFromId(parentId);
    if (parentComp == NULL)
    {
        if (screen == NULL)
            return false;
        parentId = screen->id;
        parentComp = screen;
    }
       

    if (type == GuiComponentStuff::ComponentType::NONE)
    {
        return true;
    }

    if (type == GuiComponentStuff::ComponentType::SCREEN)
    {
        GuiComponentStuff::ScreenComponent* scr = new GuiComponentStuff::ScreenComponent(window, this);
        scr->parent = parentComp;

        allComponents->Add(scr);
        return ComponentAddChild(parentId, scr);
    }

    if (type == GuiComponentStuff::ComponentType::BOX)
    {
        GuiComponentStuff::BoxComponent* comp =
        new GuiComponentStuff::BoxComponent(
            parentComp, 
            GuiComponentStuff::ComponentSize(50, 50),
            Colors.tblack
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::BUTTON)
    {
        GuiComponentStuff::ButtonComponent* comp =
        new GuiComponentStuff::ButtonComponent("", 
            Colors.black, Colors.dgray, Colors.white,
            Colors.white, Colors.bgray, Colors.black,
            GuiComponentStuff::ComponentSize(50, 50),
            GuiComponentStuff::Position(0, 0),
            parentComp
        );
        comp->id = id;
        allComponents->Add(comp->actualButtonStuff);
        allComponents->Add(comp->rectComp);
        allComponents->Add(comp->textComp);
        

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::RECT)
    {
        GuiComponentStuff::RectangleComponent* comp =
        new GuiComponentStuff::RectangleComponent(
            Colors.black,
            GuiComponentStuff::ComponentSize(50, 50),
            parentComp 
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::ADVANCED_TEXT)
    {
        GuiComponentStuff::AdvancedTextComponent* comp =
        new GuiComponentStuff::AdvancedTextComponent(
            Colors.white,
            Colors.black,
            GuiComponentStuff::ComponentSize(50, 50),
            parentComp 
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::IMAGE_RECT)
    {
        GuiComponentStuff::ImageRectangleComponent* comp =
        new GuiComponentStuff::ImageRectangleComponent(
            "",
            GuiComponentStuff::ComponentSize(50, 50),
            parentComp 
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::TEXT)
    {
        GuiComponentStuff::TextComponent* comp =
        new GuiComponentStuff::TextComponent(
            parentComp,
            Colors.white,
            Colors.black,
            "",
            GuiComponentStuff::Position(0, 0)
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::TEXTFIELD)
    {
        GuiComponentStuff::TextFieldComponent* comp =
        new GuiComponentStuff::TextFieldComponent(
            Colors.black,
            Colors.white,
            GuiComponentStuff::ComponentSize(50, 50),
            GuiComponentStuff::Position(0, 0),
            parentComp
        );
        comp->id = id;

        allComponents->Add(comp->actualTextFieldStuff);
        allComponents->Add(comp->rectComp);
        allComponents->Add(comp->textComp);

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }
    if (type == GuiComponentStuff::ComponentType::CANVAS)
    {
        GuiComponentStuff::CanvasComponent* comp =
        new GuiComponentStuff::CanvasComponent(
            Colors.black,
            GuiComponentStuff::ComponentSize(50, 50),
            parentComp 
        );
        comp->id = id;

        allComponents->Add(comp);
        return ComponentAddChild(parentId, comp);
    }

    return false;
}