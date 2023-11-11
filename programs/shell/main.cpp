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


void Cls();
bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info);
void HandleCommand(const char* input);
void SplitCommand(const char* input, const char*** args, int* argCount, bool removeQuotes);

GuiInstance* guiInst;
GuiComponentStuff::TextFieldComponent* inTxt;
GuiComponentStuff::TextComponent* pathTxt;
GuiComponentStuff::AdvancedTextComponent* outTxt;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    Window* window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Terminal");
    setWindow(window);
    
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
                "<PATH GOES HERE>", 
                GuiComponentStuff::Position(0, 0)
            );
            guiInst->screen->children->Add(pathTxt);
        }
    
    }

    Cls();

    int inputHeight = 32;
    int pathHeight = 16;
    while (!CheckForWindowClosed(window))
    {
        // Update Positions and Sizes
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
        }

        guiInst->Render();
        
        programWait(20);
    }
}

void Cls()
{
    outTxt->Clear();
    outTxt->Println("MaslOS v2", Colors.bgreen);
    outTxt->Println();
    outTxt->scrollX = 0;
    outTxt->scrollY = 0;
}

bool SpecialKeyHandler(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info)
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

void HandleCommand(const char* inputStr)
{
    const char** args = NULL;
    int argC = 0;
    SplitCommand(inputStr, &args, &argC, true);

    if (argC == 0 || args == NULL)
        return;

    // for (int i = 0; i < argC; i++)
    // {
    //     outTxt->Print("\"");
    //     if (args[i] == NULL)
    //         outTxt->Print("<NULL>", Colors.bred);
    //     else
    //         outTxt->Print(args[i], Colors.bgreen);
    //     outTxt->Print("\" ");
    // }
    // outTxt->Println();

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
    }
    else if (StrEquals(cmd, "exit"))
    {
        outTxt->Println("Exiting...");
        programExit(0);
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