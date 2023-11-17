#include "main.h"

Window* window;
GuiInstance* guiInstance;
GuiComponentStuff::ButtonComponent* openBtn;
GuiComponentStuff::ImageRectangleComponent* testImg1;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Image Viewer");
    setWindow(window);


    const char* path = "";
    if (argc != 0 && argv != NULL)
        path = argv[0];
    

    GuiInstance* gui = new GuiInstance(window);
    gui->Init();


    guiInstance = gui;

    guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::IMAGE_RECT);
    testImg1 = (GuiComponentStuff::ImageRectangleComponent*)guiInstance->GetComponentFromId(1010);

    testImg1->position.x = 0;
    testImg1->position.y = 20;

    testImg1->size.FixedX = 100;
    testImg1->size.FixedY = 100;
    window->Dimensions.x = 100;
    window->Dimensions.y = 120;
    
    _Free(testImg1->imagePath);
    testImg1->imagePath = StrCopy(path);

    testImg1->GetImageFromPath(testImg1->imagePath);

    if (testImg1->image != NULL)
    {
        testImg1->size.FixedX = testImg1->image->width;
        testImg1->size.FixedY = testImg1->image->height;
        testImg1->size.IsXFixed = true;
        testImg1->size.IsYFixed = true;
        window->Dimensions.width = testImg1->image->width;
        window->Dimensions.height = testImg1->image->height + 20;
    }
    setWindow(window);

    guiInstance->CreateComponentWithId(1011, GuiComponentStuff::ComponentType::BUTTON);
    openBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1011);
    openBtn->position.x = 0;
    openBtn->position.y = 0;
    _Free(openBtn->textComp->text);
    openBtn->textComp->text = StrCopy("Open");
    openBtn->size.FixedX = 50;
    openBtn->size.FixedY = 20;
    openBtn->OnMouseClickHelp = NULL;
    openBtn->OnMouseClickedCallBack = OnOpenClick;


    UpdateSizes();


    int inputHeight = 32;
    int pathHeight = 16;
    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        UpdateSizes();
        guiInstance->Render(false);
        
        programWaitMsg();
    }

    return 0;
}

void UpdateSizes()
{
    int h = window->Dimensions.height;
    if (h < 20)
        h = 20;
    testImg1->size.FixedY = h - 20;
    testImg1->size.FixedX = window->Dimensions.width;
}

void OnOpenClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    const char* path = "bruh:images/rocc.mbif";

    _Free(testImg1->imagePath);
    testImg1->imagePath = StrCopy(path);

    testImg1->GetImageFromPath(testImg1->imagePath);

    testImg1->size.FixedX = 100;
    testImg1->size.FixedY = 100;
    window->Dimensions.width = 100;
    window->Dimensions.height = 120;

    if (testImg1->image != NULL)
    {
        testImg1->size.FixedX = testImg1->image->width;
        testImg1->size.FixedY = testImg1->image->height;
        testImg1->size.IsXFixed = true;
        testImg1->size.IsYFixed = true;
        window->Dimensions.width = testImg1->image->width;
        window->Dimensions.height = testImg1->image->height + 20;
    }
    setWindow(window);
}

