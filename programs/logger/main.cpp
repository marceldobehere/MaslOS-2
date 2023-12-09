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

void progClosed();
void Cls();
void SpecialKeyHandler(void* bruh, GuiComponentStuff::KeyHitEventInfo info);

GuiInstance* guiInst;
GuiComponentStuff::AdvancedTextComponent* outTxt;

using namespace STDIO;
bool echoInput = true;
bool echoExit = true;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    Window* window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Logger");
    setWindow(window);

    initStdio(false);
    
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
                GuiComponentStuff::ComponentSize(1.0,1.0),
                guiInst->screen
            );

            outTxt->KeyHitFunc = SpecialKeyHandler;

            guiInst->screen->children->Add(outTxt);
        }
    }

    Cls();
    outTxt->Println();
    while (!CheckForWindowClosed(window))
    {
        guiInst->screen->selectedComponent = outTxt;
        if (parent != NULL && !pidExists(parent->pid))
            progClosed();
        if (parent != NULL)
        {
            int c = read();
            if (c != -1)
                outTxt->Print((char*)&c, Colors.white);
        }
        else
            break;

        guiInst->Render(true);
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

void SpecialKeyHandler(void* bruh, GuiComponentStuff::KeyHitEventInfo info)
{
    if (parent != NULL && !pidExists(parent->pid))
        progClosed();
    if (parent == NULL)
        return;

    char temp[2];
    temp[1] = 0;
    const char* send = NULL;

    if (info.Scancode == Key_ArrowLeft)
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
        closeProcess(parent->pid);
        progClosed();
    }
    else
    {
        temp[0] = info.Chr;
        send = (const char*)temp;
    }

    if (send != NULL)
    {
        print(send);
        if (echoInput)
            outTxt->Print(send, Colors.white);
    }

    return;
}

void progClosed()
{
    if (parent == NULL)
        return;

    if (echoExit)   
        outTxt->Println("\n> Process exited.", Colors.yellow);
    parent->Free();
    _Free(parent);
    parent = NULL;
}