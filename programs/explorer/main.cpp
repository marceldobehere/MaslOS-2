#include "main.h"

#include <libm/list/list_basics.h>
#include <libm/fsStuff/extra/fsExtra.h>

Window* window;
GuiInstance* guiInstance;
GuiComponentStuff::BoxComponent* fileListComp;
GuiComponentStuff::TextFieldComponent* pathComp;
GuiComponentStuff::ButtonComponent* goUpBtn;
List<GuiComponentStuff::ButtonComponent*>* folderCompsYes;
List<const char*>* folderPathsYes;
List<GuiComponentStuff::ButtonComponent*>* driveCompsYes;
List<const char*>* drivePathsYes;
List<GuiComponentStuff::ButtonComponent*>* fileCompsYes;
List<const char*>* filePathsYes;


const char* thisPath;
int ScrollY;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Explorer");
    setWindow(window);

    thisPath = StrCopy("");

    folderCompsYes = new List<GuiComponentStuff::ButtonComponent*>(4);
    driveCompsYes = new List<GuiComponentStuff::ButtonComponent*>(4);
    fileCompsYes = new List<GuiComponentStuff::ButtonComponent*>(4);

    folderPathsYes = new List<const char*>(4);
    drivePathsYes = new List<const char*>(4);
    filePathsYes = new List<const char*>(4);

    GuiInstance* gui = new GuiInstance(window);
    gui->Init();
    // gui->screen->backgroundColor = Colors.white;
    // window->DefaultBackgroundColor = Colors.white;
    // setWindow(window);

    guiInstance = gui;

    guiInstance->CreateComponentWithId(1021, GuiComponentStuff::ComponentType::TEXTFIELD);
    pathComp = (GuiComponentStuff::TextFieldComponent*)guiInstance->GetComponentFromId(1021);
    pathComp->position.x = 5*8 + 4;
    pathComp->position.y = 0;

    pathComp->AdvancedKeyHitCallBackHelp = (void*)NULL;
    pathComp->AdvancedKeyHitCallBack = PathTypeCallBack;
    _Free(pathComp->textComp->text);
    pathComp->textComp->text = StrCopy(thisPath);

    guiInstance->CreateComponentWithId(1023, GuiComponentStuff::ComponentType::BUTTON);
    goUpBtn = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(1023);
    goUpBtn->position.x = 0;
    goUpBtn->position.y = 0;
    _Free(goUpBtn->textComp->text);
    goUpBtn->textComp->text = StrCopy("Go Up");
    goUpBtn->size.FixedY = 16;
    goUpBtn->size.FixedX = 5*8;
    goUpBtn->OnMouseClickedCallBack = OnGoUpClick;
    goUpBtn->OnMouseClickHelp = NULL;


    guiInstance->CreateComponentWithId(1022, GuiComponentStuff::ComponentType::BOX);
    fileListComp = (GuiComponentStuff::BoxComponent*)guiInstance->GetComponentFromId(1022);
    fileListComp->position.x = 0;
    fileListComp->position.y = 20;

    

    UpdateSizes();

    Reload();
    
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
    int w = window->Dimensions.width;
    int h = window->Dimensions.height;

    if (w < 50)
        w = 50;
    if (h < 50)
        h = 50;

    fileListComp->size.FixedX = w;
    fileListComp->size.FixedY = h - 25;
    fileListComp->backgroundColor = Colors.white;
    pathComp->size.FixedX = w - pathComp->position.x;
    pathComp->size.FixedY = 16;
}

const char* GetPath()
{
    return StrCopy(thisPath);
}
void SetPath(const char* path)
{
    _Free(thisPath);
    thisPath = StrCopy(path);
}

bool PathTypeCallBack(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo event)
{
    if (event.Chr == '\n')
    {
        //GlobalRenderer->Clear(Colors.orange);
        SetPath(pathComp->textComp->text);
        Reload();
        return false;
    }
    return true;
}

void OnExternalWindowClose(Window* window)
{
    Free();
}

void OnExternalWindowResize(Window* window)
{
    UpdateSizes();
}

void Free()
{
    
    _Free(thisPath);
    
    ClearLists();
    folderCompsYes->Free();
    folderPathsYes->Free();
    driveCompsYes->Free();
    drivePathsYes->Free();
    fileCompsYes->Free();
    filePathsYes->Free();

    
}

void ClearLists()
{
    for (int i = 0; i < folderPathsYes->GetCount(); i++)
        _Free((void*)folderPathsYes->ElementAt(i)); 
    for (int i = 0; i < drivePathsYes->GetCount(); i++)
        _Free((void*)drivePathsYes->ElementAt(i)); 
    for (int i = 0; i < filePathsYes->GetCount(); i++)
        _Free((void*)filePathsYes->ElementAt(i));

    folderCompsYes->Clear();
    folderPathsYes->Clear();
    driveCompsYes->Clear();
    drivePathsYes->Clear();
    fileCompsYes->Clear();
    filePathsYes->Clear();
}

void OnFolderClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    int indx = folderCompsYes->GetIndexOf(btn);
    if (indx == -1)
        return;
    const char* pathThing = folderPathsYes->ElementAt(indx);

    const char* temp2 = StrCombine(thisPath, pathThing);
    _Free(thisPath);
    thisPath = StrCombine(temp2, "/");
    _Free(temp2);

    _Free(pathComp->textComp->text);
    pathComp->textComp->text = StrCopy(thisPath);
    Reload();
}

void OnFileClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    int indx = fileCompsYes->GetIndexOf(btn);
    if (indx == -1)
        return;
    const char* pathThing = filePathsYes->ElementAt(indx);

    serialPrint("Trying to open \"");
    serialPrint(pathThing);
    serialPrintLn("\"");

    startFile(pathThing, thisPath);
}

void OnDriveClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    int indx = driveCompsYes->GetIndexOf(btn);
    if (indx == -1)
        return;
    const char* pathThing = drivePathsYes->ElementAt(indx);
    _Free(thisPath);
    thisPath = StrCopy(pathThing);

    _Free(pathComp->textComp->text);
    pathComp->textComp->text = StrCopy(thisPath);
    Reload();
}

void OnGoUpClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info)
{
    const char* drive = FS_EXTRA::GetDriveNameFromFullPath(thisPath);
    const char* dir = FS_EXTRA::GetFolderPathFromFullPath(thisPath);

    if (drive != NULL && dir != NULL)
    {
        const char* cool1 = StrCombine(drive, ":", dir, "/");
        //GlobalRenderer->Println("COOL1:  \"{}\"", cool1, Colors.yellow);
        const char* cool2 = StrCombine(drive, ":");
        //GlobalRenderer->Println("COOL2:  \"{}\"", cool2, Colors.yellow);
        if (StrEquals(cool2, thisPath))
        {
            _Free(thisPath);
            thisPath = StrCopy("");
        }
        else if (StrEquals(cool1, thisPath))
        {
            // GO UP
            int lstIndex = StrLastIndexOf(thisPath, '/', 1);
            // test:abc/def/ 
            //          ^

            if (lstIndex != -1)
            {
                const char* nPath = StrSubstr(thisPath, 0, lstIndex + 1);
                _Free(thisPath);
                thisPath = nPath;
            }
            else
            {
                lstIndex = StrLastIndexOf(thisPath, ':');
                if (lstIndex != -1)
                {
                    const char* nPath = StrSubstr(thisPath, 0, lstIndex + 1);
                    _Free(thisPath);
                    thisPath = nPath;
                }
                else
                {
                    const char* nPath = StrCopy("");
                    _Free(thisPath);
                    thisPath = nPath;
                }
            }
            // GlobalRenderer->Println("NPATH:  \"{}\"", path, Colors.yellow);

            // while (true);
        
        }
        else
        {
            _Free(thisPath);
            thisPath = StrCopy(cool1);
        }
        _Free(cool1);
        _Free(cool2);
    }
    
    if (drive != NULL)
        _Free(drive);
    if (dir != NULL)
        _Free(dir); 

    _Free(pathComp->textComp->text);
    pathComp->textComp->text = StrCopy(thisPath);
    Reload();
}
















void Reload()
{
    
    UpdateSizes();
    ClearLists();

    while (fileListComp->children->GetCount() > 0)
    {
        GuiComponentStuff::BaseComponent* comp = fileListComp->children->ElementAt(0);
        uint64_t coolId = RND::RandomInt();
        comp->id = coolId;
        guiInstance->DeleteComponentWithId(coolId, true);
    }
    

    const char* fullDir = StrCopy(thisPath);
    const char* dir = FS_EXTRA::GetFolderPathFromFullPath(fullDir);

    int cutOff = 0;
    if (dir != NULL && StrLen(dir) != 0)
        cutOff = StrLen(dir) + 1;
    
    if (dir != NULL)
    {
        uint64_t tempCount = 0;
        const char** dataList;
        int _y = 0;

        dataList = fsGetFoldersInPath(fullDir, &tempCount);
        if (dataList != NULL)
        {
            for (int i = 0; i < (int64_t)tempCount; i++)
            {
                uint64_t coolId = RND::RandomInt();
                guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                btnComp->OnMouseClickHelp = NULL;
                btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFolderClick;

                GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                _Free(textComp->text);
                const char* tempo = StrSubstr(dataList[i], cutOff);
                textComp->text = StrCombine("Folder: ", tempo);
                btnComp->size.FixedY = 16;
                btnComp->size.FixedX = StrLen(textComp->text) * 8;
                
                folderCompsYes->Add(btnComp);
                folderPathsYes->Add(tempo);

                //_Free(tempo);
                btnComp->position.x = 0;
                btnComp->position.y = _y;
                _y += 16;
                _Free(dataList[i]);
            }   
            _Free(dataList);
        }

        dataList = fsGetFilesInPath(fullDir, &tempCount);
        if (dataList != NULL)
        {
            for (int i = 0; i < (int64_t)tempCount; i++)
            {
                uint64_t coolId = RND::RandomInt();
                guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                btnComp->OnMouseClickHelp = NULL;
                btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnFileClick;

                GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                _Free(textComp->text);
                const char* tempo = StrSubstr(dataList[i], cutOff);
                textComp->text = StrCombine("File: ", tempo);
                btnComp->size.FixedY = 16;
                btnComp->size.FixedX = StrLen(textComp->text) * 8;
                
                fileCompsYes->Add(btnComp);
                filePathsYes->Add(StrCombine(thisPath, tempo));

                _Free(tempo);
                btnComp->position.x = 0;
                btnComp->position.y = _y;
                _y += 16;
                _Free(dataList[i]);
            }
            _Free(dataList);
        }
    }
    else
    {
        uint64_t tempCount = 0;
        const char** dataList;

        dataList = fsGetDrivesInRoot(&tempCount);

        if (dataList != NULL)
        {
            int _y = 0;
            for (int i = 0; i < (int64_t)tempCount; i++)
            {
               uint64_t coolId = RND::RandomInt();

                guiInstance->CreateComponentWithIdAndParent(coolId, GuiComponentStuff::ComponentType::BUTTON, 1022);
                GuiComponentStuff::ButtonComponent* btnComp = (GuiComponentStuff::ButtonComponent*)guiInstance->GetComponentFromId(coolId);

                btnComp->OnMouseClickHelp = NULL;
                btnComp->OnMouseClickedCallBack = (void(*)(void*, GuiComponentStuff::BaseComponent*, GuiComponentStuff::MouseClickEventInfo))(void*)&OnDriveClick;

                GuiComponentStuff::TextComponent* textComp = btnComp->textComp;
                driveCompsYes->Add(btnComp);
                drivePathsYes->Add(StrCombine(dataList[i], ":"));

                _Free(textComp->text);
                textComp->text = StrCombine("Drive: ", dataList[i]);

                btnComp->size.FixedY = 16;
                btnComp->size.FixedX = StrLen(textComp->text) * 8;
                
                btnComp->position.x = 0;
                btnComp->position.y = _y;



                _y += 16;
            }

            _Free(dataList);
        }
    }

    guiInstance->Render(false);
    fileListComp->Render(GuiComponentStuff::Field(GuiComponentStuff::Position(), GuiComponentStuff::Position(fileListComp->size.FixedX, fileListComp->size.FixedY)));


    if (fullDir != NULL)
        _Free(fullDir);
    if (dir != NULL)
        _Free(dir);
    
}