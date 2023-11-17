#include "main.h"
#include <libm/memStuff.h>


const char* lastSavePath;
int ScrollY;
Window* window;
GuiInstance* guiInstance;
GuiComponentStuff::ButtonComponent* loadBtn;
GuiComponentStuff::ButtonComponent* saveBtn;
GuiComponentStuff::ButtonComponent* saveAsBtn;
GuiComponentStuff::TextFieldComponent* textComp;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Notepad");
    setWindow(window);

    lastSavePath = NULL;
    //btnTaskState = NotePadButtonTaskState::None;

    GuiInstance* gui = new GuiInstance(window);
    gui->Init();


    guiInstance = gui;

    guiInstance->CreateComponentWithId(1010, GuiComponentStuff::ComponentType::TEXTFIELD);
    textComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1010);
    textComp->position.x = 0;
    textComp->position.y = 22;

    guiInstance->CreateComponentWithId(1020, GuiComponentStuff::ComponentType::BUTTON);
    loadBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1020);
    loadBtn->position.x = 0;
    loadBtn->position.y = 0;
    _Free(loadBtn->textComp->text);
    loadBtn->textComp->text = StrCopy("Load");
    loadBtn->size.FixedX = 50;
    loadBtn->size.FixedY = 20;
    loadBtn->OnMouseClickHelp = NULL;
    loadBtn->OnMouseClickedCallBack = OnLoadClick;

    guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::BUTTON);
    saveBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1021);
    saveBtn->position.x = 52;
    saveBtn->position.y = 0;
    _Free(saveBtn->textComp->text);
    saveBtn->textComp->text = StrCopy("Save");
    saveBtn->size.FixedX = 50;
    saveBtn->size.FixedY = 20;
    saveBtn->OnMouseClickHelp = NULL;
    saveBtn->OnMouseClickedCallBack = OnSaveClick;

    guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BUTTON);
    saveAsBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1022);
    saveAsBtn->position.x = 104;
    saveAsBtn->position.y = 0;
    _Free(saveAsBtn->textComp->text);
    saveAsBtn->textComp->text = StrCopy("Save As");
    saveAsBtn->size.FixedX = 64;
    saveAsBtn->size.FixedY = 20;
    saveAsBtn->OnMouseClickHelp = NULL;
    saveAsBtn->OnMouseClickedCallBack = OnSaveAsClick;



    UpdateSizes();

    //Reload();


    if (argc != 0 && argv != NULL)
    {
        LoadFrom(argv[0]);
    }


    int inputHeight = 32;
    int pathHeight = 16;
    while (!CheckForWindowClosed(window))
    {
        // Update Positions and Sizes
        guiInstance->Update();
        UpdateSizes();
        guiInstance->Render(false);
        
        programWaitMsg();
    }

    return 0;
}


void UpdateSizes()
{
    int w = window->Dimensions.width;
    int h = window->Dimensions.height;
    if (w < 50)
        w = 50;
    if (h < 50)
        h = 50;

    textComp->size.FixedX = w;
    textComp->size.FixedY = h - 22;
}

void OnSaveClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    // if (lastSavePath == NULL)
    // {
    //     SaveFileExplorer* exp = new SaveFileExplorer();
        
    //     guiInstance->OnWaitTaskDoneHelp = (void*)this;
    //     guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
    //     guiInstance->waitTask = exp->dataTask;

    //     btnTaskState = NotePadButtonTaskState::Save;
    // }
    // else
    // {
    //     SaveInto(StrCopy(lastSavePath));
    // }
}

void OnSaveAsClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    // SaveFileExplorer* exp = new SaveFileExplorer();
    
    // guiInstance->OnWaitTaskDoneHelp = (void*)this;
    // guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
    // guiInstance->waitTask = exp->dataTask;

    // btnTaskState = NotePadButtonTaskState::Save;
}

void OnLoadClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    // OpenFileExplorer* exp = new OpenFileExplorer();
        
    // guiInstance->OnWaitTaskDoneHelp = (void*)this;
    // guiInstance->OnWaitTaskDoneCallback = (void(*)(void*, Task*))(void*)&OnTaskDone;
    // guiInstance->waitTask = exp->dataTask;

    // btnTaskState = NotePadButtonTaskState::Load;
}


void SaveInto(const char* path)
{
    if (path == NULL)
        return;

    if (lastSavePath != NULL)
    {
        _Free(lastSavePath);
        lastSavePath = NULL;
    }
    lastSavePath = StrCopy(path);

    fsWriteFileFromBuffer(path, (char*)textComp->textComp->text, StrLen(textComp->textComp->text));
}

void LoadFrom(const char* path)
{
    if (lastSavePath != NULL)
    {
        _Free(lastSavePath);
        lastSavePath = NULL;
    }
    lastSavePath = StrCopy(path);


    // LOAD
    char* fData = NULL;
    uint64_t fDataLen = 0;

    if (fsReadFile(path, (void**)(&fData), &fDataLen))
    {
        char* nData = (char*)_Malloc(fDataLen + 1);
        nData[fDataLen] = 0;
        _memcpy(fData, nData, fDataLen);
        _Free(fData);

        _Free(textComp->textComp->text);
        textComp->textComp->text = (const char*)nData;
    }
    else
    {
        if (lastSavePath != NULL)
        {
            _Free(lastSavePath);
            lastSavePath = NULL;
        }
    }
}
