#include "main.h"

static const uint32_t DefaultMSColors[28]
{ //0xAARRGGBB
    0xFF000000, // Black
    0xFF7F7F7F, // Dark Gray
    0xFF7E0102, // Dark Red
    0xFF7E8000, // Dark Yellow
    0xFF027F03, // Dark Green
    0xFF00807E, // Dark Light Blue
    0xFF040083, // Dark Blue
    0xFF7D0284, // Dark Magenta
    0xFF807E41,// Dark Lime
    0xFF024241, // Dark Cyan
    0xFF0380FE, // Dark Light Blue (2)
    0xFF013F83, // Dark Light Purple Blue
    0xFF4000FB, // Middle Dark Blue
    0xFF813D00, // Brown

    0xFFFFFFFF, // White
    0xFFBFBFBF, // Gray
    0xFFFA0201, // Red
    0xFFFBFF04, // Yellow
    0xFF04FF0B, // Green
    0xFF03FDFB, // Light Blue
    0xFF0403F7, // Blue
    0xFFFD01FC, // Magenta
    0xFFFFFB84, // Lime
    0xFF05FD7E, // Cyan
    0xFF7CFFFF, // Light Blue (2)
    0xFF7F7EFE, // Light Purple Blue
    0xFFFC0181, // Pink
    0xFFFE8040, // Orange
};

GuiInstance* guiInstance;
Window* window;

BoxComponent* menuBox;
ButtonComponent* openBtn;
ButtonComponent* saveBtn;
ButtonComponent* saveAsBtn;
ButtonComponent* clearBtn;
BoxComponent* colorBox;
ButtonComponent* lastCol;
// Colors will be added into the colorbox
CanvasComponent* canvas;
Position canvasPosition;

MPoint oldMousePos;
bool oldMouseLeftState;
bool oldMouseRightState;
bool currMouseLeftState;
bool currMouseRightState;
uint32_t currentColor;
int curSize;

int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Paint");

    window->Dimensions.width = 300;
    window->Dimensions.height = 320;

    setWindow(window);

    guiInstance = new GuiInstance(window);
    guiInstance->Init();

    guiInstance->screen->backgroundColor = Colors.white;

    oldMouseLeftState = false;
    oldMouseRightState = false;
    currMouseLeftState = false;
    currMouseRightState = false;
    currentColor = Colors.black;
    curSize = 2;


    guiInstance->CreateComponentWithId(1100, ComponentType::BOX);
    menuBox = (BoxComponent*)guiInstance->GetComponentFromId(1100);
    menuBox->position.x = 0;
    menuBox->position.y = 0;
    menuBox->size.IsXFixed = false;
    menuBox->size.ScaledX = 1;
    menuBox->size.IsYFixed = true;
    menuBox->size.FixedY = 20;
    menuBox->backgroundColor = Colors.bgray;

    guiInstance->CreateComponentWithIdAndParent(1101, ComponentType::BUTTON, menuBox->id);
    openBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1101);
    openBtn->position.x = 0;
    openBtn->position.y = 0;
    openBtn->size.IsXFixed = true;
    openBtn->size.FixedX = 50;
    openBtn->size.IsYFixed = true;
    openBtn->size.FixedY = 20;
    
    _Free(openBtn->textComp->text);
    openBtn->textComp->text = StrCopy("Open");

    openBtn->OnMouseClickHelp = NULL;
    openBtn->OnMouseClickedCallBack = OnOpenClick;


    guiInstance->CreateComponentWithIdAndParent(1102, ComponentType::BUTTON, menuBox->id);
    saveBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1102);
    saveBtn->position.x = 50;
    saveBtn->position.y = 0;
    saveBtn->size.IsXFixed = true;
    saveBtn->size.FixedX = 50;
    saveBtn->size.IsYFixed = true;
    saveBtn->size.FixedY = 20;

    _Free(saveBtn->textComp->text);
    saveBtn->textComp->text = StrCopy("Save");

    saveBtn->OnMouseClickHelp = NULL;
    saveBtn->OnMouseClickedCallBack = OnSaveClick;


    guiInstance->CreateComponentWithIdAndParent(1103, ComponentType::BUTTON, menuBox->id);
    saveAsBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1103);
    saveAsBtn->position.x = 100;
    saveAsBtn->position.y = 0;
    saveAsBtn->size.IsXFixed = true;
    saveAsBtn->size.FixedX = 74;
    saveAsBtn->size.IsYFixed = true;
    saveAsBtn->size.FixedY = 20;

    _Free(saveAsBtn->textComp->text);
    saveAsBtn->textComp->text = StrCopy("Save As");

    saveAsBtn->OnMouseClickHelp = NULL;
    saveAsBtn->OnMouseClickedCallBack = OnSaveAsClick;


    guiInstance->CreateComponentWithIdAndParent(1104, ComponentType::BUTTON, menuBox->id);
    clearBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1104);
    clearBtn->position.x = 174;
    clearBtn->position.y = 0;
    clearBtn->size.IsXFixed = true;
    clearBtn->size.FixedX = 50;
    clearBtn->size.IsYFixed = true;
    clearBtn->size.FixedY = 20;

    _Free(clearBtn->textComp->text);
    clearBtn->textComp->text = StrCopy("Clear");

    clearBtn->OnMouseClickHelp = NULL;
    clearBtn->OnMouseClickedCallBack = OnClearClick;


    guiInstance->CreateComponentWithId(1200, ComponentType::BOX);
    colorBox = (BoxComponent*)guiInstance->GetComponentFromId(1200);
    colorBox->position.x = 0;
    colorBox->position.y = 20;
    // colorBox->size.IsXFixed = true;
    // colorBox->size.FixedX = 320;
    colorBox->size.IsXFixed = false;
    colorBox->size.ScaledX = 1;
    colorBox->size.IsYFixed = true;
    colorBox->size.FixedY = 40;
    colorBox->backgroundColor = Colors.bgray;

    
    // add colors here
    int startIdForColorButtons = 1300;
    lastCol = NULL;
    for (int y = 0; y < 2; y++)
    {
        for (int x = 0; x < 14; x++)
        {
            //Serial::Writeln("Adding color button: {}", to_string(startIdForColorButtons));
            if (!guiInstance->CreateComponentWithIdAndParent(startIdForColorButtons, ComponentType::BUTTON, colorBox->id))
            {
                x--;
                continue;
            }

            ButtonComponent* colorBtn = (ButtonComponent*)guiInstance->GetComponentFromId(startIdForColorButtons);
            

        
            colorBtn->position.x = x * 20;
            colorBtn->position.y = y * 20;
            colorBtn->size.IsXFixed = true;
            colorBtn->size.FixedX = 20;
            colorBtn->size.IsYFixed = true;
            colorBtn->size.FixedY = 20;

            if (lastCol == NULL)
            {
                lastCol = colorBtn;
                lastCol->position.x += 2;
                lastCol->position.y += 2;
                lastCol->size.FixedX -= 4;
                lastCol->size.FixedY -= 4;
            }

            colorBtn->bgColDef    = DefaultMSColors[x + y * 14];
            colorBtn->bgColHover = colorBtn->bgColDef | 0xFF000000;
            colorBtn->bgColClick  = colorBtn->bgColDef | 0xFF000000;

            colorBtn->OnMouseClickHelp = NULL;
            colorBtn->OnMouseClickedCallBack = OnColorClick;

            startIdForColorButtons++;
        }
    }
    

    guiInstance->CreateComponentWithId(1400, ComponentType::CANVAS);
    canvas = (CanvasComponent*)guiInstance->GetComponentFromId(1400);
    canvas->position.x = 0;
    canvas->position.y = 60;
    canvas->size.IsXFixed = false;
    canvas->size.ScaledX = 1;
    canvas->size.IsYFixed = true;
    canvas->size.FixedY = window->Dimensions.height - canvas->position.y;
    canvas->bgColor = Colors.white;
    canvasPosition = canvas->GetAbsoluteComponentPosition();
    canvas->OnMouseClickHelp = NULL;
    canvas->OnMouseClickedCallBack = OnCanvasClick;

    canvas->Clear();
    {
        MouseState pos = guiInstance->mouseState;

        MPoint aPos = MPoint(
            pos.MouseX - canvasPosition.x,
            pos.MouseY - canvasPosition.y
        ); 
        oldMousePos = aPos;
    }
    canvas->CheckUpdates();

    MainLoop();

    return 0;
}


void MainLoop()
{
    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        UpdateSizes();
        HandleFrame();
        guiInstance->Render(false);
    }
}

void HandleFrame()
{
    MouseState stat = guiInstance->mouseState;


    MPoint aPos = MPoint(
        stat.MouseX - canvasPosition.x,
        stat.MouseY - canvasPosition.y
    ); 


    GuiComponentStuff::ComponentSize tSize = canvas->GetActualComponentSize();
    bool inRange = aPos.x >= 0 && aPos.y >= 0 &&
        aPos.x < tSize.FixedX && 
        aPos.y < tSize.FixedY;

    if (window->IsActive && inRange)
        guiInstance->screen->tempSelectedComponent = canvas;

    if (!window->IsActive ||
        canvas != guiInstance->screen->selectedComponent)
        return;


    //serialPrintLn("> F2");

    currMouseLeftState = currMouseLeftState && stat.Left;
    
    if (currMouseLeftState)
    {
        if (!oldMouseLeftState)
            oldMousePos = aPos;

        if (inRange)
        {
            if (oldMouseLeftState && oldMousePos != aPos)
                canvas->DrawLine(oldMousePos.x, oldMousePos.y, aPos.x, aPos.y, currentColor, curSize);
            else if (!oldMouseLeftState)
                canvas->DrawBlob(aPos.x, aPos.y, currentColor, curSize);
        }
        oldMousePos = aPos;
    }
    oldMouseLeftState = currMouseLeftState;
}

void UpdateSizes()
{
    int h = window->Dimensions.height;
    if (h < 40)
        h = 40;
    int w = window->Dimensions.width;
    if (w < 100)
        w = 100;
    window->Dimensions.width = w;
    window->Dimensions.height = h;

    canvas->size.FixedY = h - canvas->position.y;
}

void OnOpenClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{

}

void OnSaveClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{

}

void OnSaveAsClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{

}

void OnClearClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    canvas->Clear();
}

void OnCanvasClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    currMouseLeftState = true;
}

void OnColorClick(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    if (lastCol != NULL)
    {
        lastCol->position.x -= 2;
        lastCol->position.y -= 2;
        lastCol->size.FixedX += 4;
        lastCol->size.FixedY += 4;
    }

    {
        lastCol = (ButtonComponent*)btn;
        lastCol->position.x += 2;
        lastCol->position.y += 2;
        lastCol->size.FixedX -= 4;
        lastCol->size.FixedY -= 4;
    }

    currentColor = ((ButtonComponent*)btn)->bgColDef;
}