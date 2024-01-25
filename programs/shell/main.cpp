#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>
#include <libm/keyboard.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>
#include <libm/gui/guiStuff/components/advancedText/advancedTextComponent.h>

#include <libm/stdio/stdio.h>


void Cls();
bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info);
void HandleCommand(const char* input);
void SplitCommand(const char* input, const char*** args, int* argCount, bool removeQuotes);

const char* currentPath = NULL;
GuiInstance* guiInst;
GuiComponentStuff::TextFieldComponent* inTxt;
GuiComponentStuff::TextComponent* pathTxt;
GuiComponentStuff::AdvancedTextComponent* outTxt;

using namespace STDIO;

bool echoInput = true;
bool echoExit = true;
StdioInst* currStdio = NULL;

void progClosed()
{
    if (echoExit)   
        outTxt->Println("\n> Process exited.", Colors.yellow);

    currStdio->Free();
    _Free(currStdio);
    currStdio = NULL;
}

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    Window* window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Terminal");
    setWindow(window);

    currentPath = StrCopy("bruh:");
    
    {
        guiInst = new GuiInstance(window);
        guiInst->Init();
        guiInst->screen->backgroundColor = Colors.black;
        window->DefaultBackgroundColor = Colors.black;
        setWindow(window);

        // Output
        {
            outTxt = new GuiComponentStuff::AdvancedTextComponent(
                Colors.white,
                Colors.black,
                GuiComponentStuff::ComponentSize(10,10),
                guiInst->screen
            );
            guiInst->screen->children->Add(outTxt);
        }

        // Input
        {
            inTxt = new GuiComponentStuff::TextFieldComponent(
                Colors.white, Colors.dgray,
                GuiComponentStuff::ComponentSize(100, 32),
                GuiComponentStuff::Position(0, 0), 
                guiInst->screen
            );

            inTxt->AdvancedKeyHitCallBack = SpecialKeyHandler;
            inTxt->AdvancedKeyHitCallBackHelp = NULL;

            guiInst->screen->children->Add(inTxt);
        }
    
        // Path
        {
            pathTxt = new GuiComponentStuff::TextComponent(
                guiInst->screen, 
                Colors.dblue, Colors.bblue | Colors.bgreen, 
                currentPath, 
                GuiComponentStuff::Position(0, 0)
            );
            pathTxt->useFixedSize = true;
            guiInst->screen->children->Add(pathTxt);
        }
    
    }

    Cls();
    outTxt->Println();

    int inputHeight = 32;
    int pathHeight = 16;
    while (!CheckForWindowClosed(window))
    {
        // Update Positions and Sizes
        guiInst->Update();
        {
            GuiComponentStuff::ComponentSize scrSize =  guiInst->screen->size;
            outTxt->size.FixedX = scrSize.FixedX;
            outTxt->size.FixedY = scrSize.FixedY - inputHeight - pathHeight;
            
            pathTxt->size.FixedX = scrSize.FixedX;
            pathTxt->size.FixedY = pathHeight;
            pathTxt->position.y = scrSize.FixedY - inputHeight - pathHeight;

            inTxt->size.FixedX = scrSize.FixedX;
            inTxt->size.FixedY = inputHeight;
            inTxt->position.y = scrSize.FixedY - inputHeight;

            if (!StrEquals(pathTxt->text, currentPath))
            {
                _Free(pathTxt->text);
                pathTxt->text = StrCopy(currentPath);
            }
        }

        if (currStdio != NULL && !available(currStdio) && !pidExists(currStdio->pid))
            progClosed();
        if (currStdio != NULL)
        {
            int c = bytesAvailable(currStdio);
            if (c != 0)
            {
                char* bruh = (char*)_Malloc(c + 1);
                int ac = readBytes((uint8_t*)bruh, c, currStdio);
                bruh[ac] = 0;
                outTxt->Print(bruh, Colors.white);
                _Free(bruh);
            }
        }

        guiInst->Render(false);
        
        programWaitMsg();
    }

    return 0;
}

void Cls()
{
    outTxt->Clear();
    outTxt->Println("MaslOS v2", Colors.bgreen);
    outTxt->scrollX = 0;
    outTxt->scrollY = 0;
}

bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info)
{
    if (currStdio != NULL && !available(currStdio) && !pidExists(currStdio->pid))
        progClosed();
        
    if (currStdio != NULL)
    {
        char temp[2];
        temp[1] = 0;
        const char* send = NULL;

        if (envGetKeyState(Key_LeftShift) && info.Scancode != Key_LeftShift)
        {
            if (info.Scancode == Key_ArrowUp)
            {
                outTxt->scrollY -= 16;
                return false;
            }
            else if (info.Scancode == Key_ArrowDown)
            {
                outTxt->scrollY += 16;
                return false;
            }
            else if (info.Scancode == Key_ArrowLeft)
            {
                if (outTxt->scrollX >= 8)
                    outTxt->scrollX -= 8;
                return false;
            }
            else if (info.Scancode == Key_ArrowRight)
            {
                outTxt->scrollX += 8;
                return false;
            }
            else
            {
                temp[0] = info.Chr;
                send = (const char*)temp;
            }
        }
        else if (info.Scancode == Key_ArrowLeft)
            send = "\x1b[D";
        else if (info.Scancode == Key_ArrowRight)
            send = "\x1b[C";
        else if (info.Scancode == Key_ArrowUp)
            send = "\x1b[A";
        else if (info.Scancode == Key_ArrowDown)
            send = "\x1b[B";
        else if (info.Scancode == Key_Backspace)
            send = "\b";
        else if (info.Chr == 'c' && envGetKeyState(Key_LeftControl))
        {
            closeProcess(currStdio->pid);
            progClosed();
        }
        else
        {
            temp[0] = info.Chr;
            send = (const char*)temp;
        }

        if (send != NULL)
        {
            print(send, currStdio);
            if (echoInput)
                outTxt->Print(send, Colors.white);
        }

        return false;
    }   
    else
    {
        if (info.Chr == '\n')
        {
            // Write command
            outTxt->Print("> ", Colors.bgray);
            outTxt->Println(inTxt->textComp->text, Colors.bgray);

            // Get cmd str
            const char* cmd = StrCopy(inTxt->textComp->text);

            // Reset input
            _Free(inTxt->textComp->text);
            inTxt->textComp->text = StrCopy("");

            HandleCommand(cmd);
            outTxt->Println();

            // Free
            _Free(cmd);

            return false;
        }
        else if (info.Scancode == Key_ArrowUp)
        {
            outTxt->scrollY -= 16;
            return false;
        }
        else if (info.Scancode == Key_ArrowDown)
        {
            outTxt->scrollY += 16;
            return false;
        }
        else if (info.Scancode == Key_ArrowLeft)
        {
            if (outTxt->scrollX >= 8)
                outTxt->scrollX -= 8;
            return false;
        }
        else if (info.Scancode == Key_ArrowRight)
        {
            outTxt->scrollX += 8;
            return false;
        }

        return true;
    }

    return true;
}


// "maab/test.maab", "bruh:maab/"
// -> "test.maab"
const char* RemoveCurrentPathFromPath(const char* path, const char* current)
{
    if (StrEquals(currentPath, ""))
        return StrCopy(path);
    
    int pathLen = StrLen(path);
    int currentLen = StrLen(current);

    // Remove the drive part from the current path
    int currentStart = StrIndexOf(current, ':') + 1;
    if (currentStart == 0)
        return StrCopy(path);
    

    if (!StrStartsWith(path, current + currentStart))
        return StrCopy(path);

    // Remove the current path from the path
    int currLen2 = currentLen - currentStart;
    return StrSubstr(path, currLen2);
}

void HandleCommand(const char* inputStr)
{
    const char** args = NULL;
    int argC = 0;
    SplitCommand(inputStr, &args, &argC, true);

    if (argC == 0 || args == NULL)
        return;

    const char* cmd = args[0];

    if (StrEquals(cmd, "cls"))
    {
        Cls();
    }
    else if (StrEquals(cmd, "echo"))
    {
        if (argC == 1)
            outTxt->Println();
        else
            outTxt->Println(args[1]);
    }
    else if (StrEquals(cmd, "help"))
    {
        outTxt->Println("Commands:");
        outTxt->Println(" - cls");
        outTxt->Println(" - echo <text>");
        outTxt->Println(" - help");
        outTxt->Println(" - exit");
        outTxt->Println(" - ls");
        outTxt->Println(" - cd <path>");
        outTxt->Println(" - run <file>");
        outTxt->Println(" - get <name>");
        outTxt->Println(" - set <name> <value>");
        outTxt->Println();
        outTxt->Println("You can find more information in the wiki:");
        outTxt->Println("https://github.com/marceldobehere/MaslOS-2/wiki");
        outTxt->Println();
    }
    else if (StrEquals(cmd, "exit"))
    {
        outTxt->Println("Exiting...");
        programExit(0);
    }
    else if (StrEquals(cmd, "get"))
    {
        if (argC < 2)
            outTxt->Println("> Get command is missing parameter!", Colors.bred);
        else if (argC > 2)
            outTxt->Println("> Get command has too many parameters!", Colors.bred);
        else
        {
            const char* key = args[1];
            if (StrEquals(key, "tsb ms"))
            {
                outTxt->Print("Time since boot (MS): ");
                outTxt->Println(to_string(envGetTimeMs()));
            }
            else if (StrEquals(key, "mouse sens") || StrEquals(key, "mouse sensitivity"))
            {
                outTxt->Print("Mouse Sensitivity: ");
                outTxt->Print(to_string(envGetMouseSens()));
                outTxt->Println("%");
            }
            else
            {
                outTxt->Println("> Invalid parameter for get command!", Colors.bred);
            }
        }
    }
    else if (StrEquals(cmd, "set"))
    {
        if (argC < 3)
            outTxt->Println("> Set command is missing parameter!", Colors.bred);
        else if (argC > 3)
            outTxt->Println("> Set command has too many parameters!", Colors.bred);
        else
        {
            const char* key = args[1];
            const char* val = args[2];

            if (StrEquals(key, "mouse sens") || StrEquals(key, "mouse sensitivity"))
            {
                int mouseSens = to_int(val);
                mouseSens = envSetMouseSens(mouseSens);
                outTxt->Print("Mouse Sensitivity set to ");
                outTxt->Print(to_string(mouseSens));
                outTxt->Println("%");
            }
            else
            {
                outTxt->Println("> Invalid parameter for set command!", Colors.bred);
            }
        }
    }
    else if (StrEquals(cmd, "ls") || StrEquals(cmd, "dir"))
    {
        const char* pathToUse;
        if (argC == 1 || StrEquals(args[1], ""))
            pathToUse = StrCopy(currentPath);
        else if (StrIndexOf(args[1], ':') != -1)
            pathToUse = StrCopy(args[1]);
        else
            pathToUse = StrCombine(currentPath, args[1]);

        if (StrIndexOf(pathToUse, ':') != StrLen(pathToUse) - 1 && StrLen(pathToUse) != 0 && !StrEndsWith(pathToUse, "/"))
            pathToUse = StrCombineAndFree(pathToUse, "/");
        if (StrIndexOf(pathToUse, ':') == -1 && StrLen(pathToUse) != 0 && !StrEndsWith(pathToUse, ":"))
            pathToUse = StrCombineAndFree(pathToUse, ":");

        if (StrEquals(pathToUse, ""))
        {
            uint64_t driveCount = 0;
            const char** driveNames = fsGetDrivesInRoot(&driveCount);

            if (driveCount != 0 && driveNames != NULL)
            {
                outTxt->Println("Drives in root:");
                for (int i = 0; i < driveCount; i++)
                {
                    outTxt->Print(" - ");
                    outTxt->Println(driveNames[i]);
                    _Free(driveNames[i]);
                }
                _Free(driveNames);
            }
        }
        else
        {
            outTxt->Print("Path: ");
            outTxt->Println(pathToUse);

            {
                uint64_t folderCount = 0;
                const char** folders = fsGetFoldersInPath(pathToUse, &folderCount);

                if (folderCount != 0 && folders != NULL)
                {
                    outTxt->Println("Folders:");
                    for (int i = 0; i < folderCount; i++)
                    {
                        const char* lName = folders[i];
                        const char* sName = RemoveCurrentPathFromPath(lName, pathToUse);
                        outTxt->Print(" - ");
                        outTxt->Println(sName);
                        _Free(folders[i]);
                    }
                    _Free(folders);
                }
            }
            {
                uint64_t fileCount = 0;
                const char** files = fsGetFilesInPath(pathToUse, &fileCount);

                if (fileCount != 0 && files != NULL)
                {
                    outTxt->Println("Files:");
                    for (int i = 0; i < fileCount; i++)
                    {
                        const char* lName = files[i];
                        const char* sName = RemoveCurrentPathFromPath(lName, pathToUse);
                        outTxt->Print(" - ");
                        outTxt->Println(sName);
                        _Free(files[i]);
                    }
                    _Free(files);
                }
            }
        }


        _Free(pathToUse);
    }
    else if (StrEquals(cmd, "cd"))
    {
        if (argC == 1)
        {
            _Free(currentPath);
            currentPath = StrCopy("");
        }
        else if (StrEquals(args[1], "..") || StrEquals(args[1], "../"))
        {
            if (StrEndsWith(currentPath, ":"))
            {
                _Free(currentPath);
                currentPath = StrCopy("");
            }
            else if (StrEndsWith(currentPath, "/"))
            {
                const char* temp = currentPath;
                int idx = StrLastIndexOf(currentPath, '/', 1);
                if (idx != -1)
                {
                    currentPath = StrSubstr(currentPath, 0, idx + 1);
                    _Free(temp);
                }
                else
                {
                    idx = StrIndexOf(currentPath, ':');
                    if (idx != -1)
                    {
                        currentPath = StrSubstr(currentPath, 0, idx + 1);
                        _Free(temp);
                    }
                }
            }
        }
        else
        {
            const char* pathToUse = StrCopy(args[1]);

            if (StrEndsWith(pathToUse, ":") || StrEndsWith(pathToUse, "/"))
            {
                const char* temp = pathToUse;
                pathToUse = StrSubstr(pathToUse, 0, StrLen(pathToUse) - 1);
                _Free(temp);
            }

            const char* combined = StrCombine(currentPath, pathToUse);
            if (fsFolderExists(combined))
            {
                _Free(currentPath);
                currentPath = StrCopy(combined);
                if (!StrEndsWith(currentPath, "/"))
                    currentPath = StrCombineAndFree(currentPath, "/");
            }
            else if (fsFolderExists(pathToUse))
            {
                _Free(currentPath);
                currentPath = StrCopy(pathToUse);
                if (!StrEndsWith(currentPath, "/"))
                    currentPath = StrCombineAndFree(currentPath, "/");
            }
            else
            {
                bool isDrive = false;
                uint64_t driveCount = 0;
                const char** driveNames = fsGetDrivesInRoot(&driveCount);

                if (driveCount != 0 && driveNames != NULL)
                {
                    for (int i = 0; i < driveCount; i++)
                    {
                        if (StrEquals(driveNames[i], pathToUse))
                        {
                            isDrive = true;
                            _Free(driveNames[i]);
                            break;
                        }
                        _Free(driveNames[i]);
                    }
                    _Free(driveNames);
                }

                if (isDrive)
                {
                    _Free(currentPath);
                    currentPath = StrCopy(pathToUse);
                    if (!StrEndsWith(currentPath, ":"))
                        currentPath = StrCombineAndFree(currentPath, ":");
                }
                else
                {
                    outTxt->Print("Not a directory: \"", Colors.bred);
                    outTxt->Print(pathToUse, 0xffFFAA00);
                    outTxt->Println("\"", Colors.bred);
                }
            }

            _Free(combined);
            _Free(pathToUse);
        }
    }
    else if (StrEquals(cmd, "run"))
    {
        if (argC > 1)
        {
            const char* pathToUse = StrCopy(args[1]);
            const char* combined = StrCombine(currentPath, pathToUse);

            if (fsFileExists(combined))
            {
                uint64_t newPid = startProcess(combined, argC - 2, args + 2, currentPath);
                if (newPid == 0)
                {
                    outTxt->Print("> Failed to start process: \"", Colors.bred);
                    outTxt->Print(pathToUse, 0xffFFAA00);
                    outTxt->Println("\"", Colors.bred);
                }
                else
                {
                    outTxt->Print("> Started process with PID: ", Colors.bgreen);
                    outTxt->Println(ConvertHexToString(newPid), Colors.bgreen);
                    currStdio = initStdio(newPid);
                }
            }
            else if (fsFileExists(pathToUse))
            {
                uint64_t newPid = startProcess(pathToUse, argC - 2, args + 2, currentPath);
                if (newPid == 0)
                {
                    outTxt->Print("> Failed to start process: \"", Colors.bred);
                    outTxt->Print(pathToUse, 0xffFFAA00);
                    outTxt->Println("\"", Colors.bred);
                }
                else
                {
                    outTxt->Print("> tarted process with PID: ", Colors.bgreen);
                    outTxt->Println(ConvertHexToString(newPid), Colors.bgreen);
                    currStdio = initStdio(newPid);
                }
            }
            else
            {
                outTxt->Print("File not found: \"", Colors.bred);
                outTxt->Print(pathToUse, 0xffFFAA00);
                outTxt->Println("\"", Colors.bred);
            }

            _Free(combined);
            _Free(pathToUse);
        }
        else
            outTxt->Println("Usage: run <file>");
    }
    else
    {
        outTxt->Print("Unknown command: \"", Colors.bred);
        outTxt->Print(cmd, 0xffFFAA00);
        outTxt->Println("\"", Colors.bred);
    }

    // Free mem
    for (int i = 0; i < argC; i++)
        _Free(args[i]);
    _Free(args);
}


// ABC 123 "Tomato lol" "Bruh\"lol" a
// -> "ABC" "123" "Tomato lol" "Bruh\"lol" "a"
void SplitCommand(const char* input, const char*** args, int* argCount, bool removeQuotes)
{
    int len = StrLen(input);
    if (input == NULL || len == 0)
        return;

    int argC = 1;
    for (int i = 0; i < len; i++)
    {
        if (input[i] == ' ')
            argC++;
        else if (input[i] == '\\')
            i++;
        else if (input[i] == '\"')
        {
            i++;
            while (input[i] != '\"' && i < len)
                i += (input[i] == '\\') ? 2 : 1;
        }
    }

    const char** argV = (const char**)_Malloc(argC * sizeof(const char*));

    int argI = 0;
    int startI = 0;
    for (int i = 0; i < len; i++)
    {
        if (input[i] == ' ')
        {
            int argLen = i - startI;
            if (removeQuotes && argLen > 1 && input[startI] == '\"')
            {
                startI++;
                argLen -= 2;
            }

            int actualArgLen = argLen;
            for (int j = 0; j < argLen; j++)
                if (input[startI + j] == '\\')
                    actualArgLen--;

            char* arg = (char*)_Malloc(actualArgLen + 1);
            for (int j = 0, l = 0; j < argLen; j++)
                if (input[startI + j] != '\\')
                    arg[l++] = input[startI + j];
            arg[actualArgLen] = '\0';

            argV[argI] = arg;
            startI = i + 1;
            argI++;
        }
        else if (input[i] == '\\')
            i++;
        else if (input[i] == '\"')
        {
            i++;
            while (input[i] != '\"' && i < len)
                i += (input[i] == '\\') ? 2 : 1;
        }
    }
    {
        int argLen = len - startI;
        if (removeQuotes && argLen > 1 && input[startI] == '\"')
        {
            startI++;
            argLen -= 2;
        }

        int actualArgLen = argLen;
        for (int j = 0; j < argLen; j++)
            if (input[startI + j] == '\\')
                actualArgLen--;

        char* arg = (char*)_Malloc(actualArgLen + 1);
        for (int j = 0, l = 0; j < argLen; j++)
            if (input[startI + j] != '\\')
                arg[l++] = input[startI + j];
        arg[actualArgLen] = '\0';

        argV[argI] = arg;
    }

    *args = argV;
    *argCount = argC; 
}