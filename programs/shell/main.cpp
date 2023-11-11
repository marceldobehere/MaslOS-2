#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>

void Cls();
void EnterHandler(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info);

GuiInstance* guiInst;
GuiComponentStuff::TextFieldComponent* inTxt;
GuiComponentStuff::TextComponent* pathTxt;
GuiComponentStuff::TextComponent* outTxt;

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
            outTxt = new GuiComponentStuff::TextComponent(
                guiInst->screen, 
                Colors.black, Colors.white, 
                "", 
                GuiComponentStuff::Position(0, 0)
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

            inTxt->keyHitCallBack = EnterHandler;

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
    _Free(outTxt->text);
    outTxt->text = StrCopy("MaslOSv2\n\n");
}

void EnterHandler(GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo info)
{
    if (info.Chr == '\n')
    {
        // Write command
        outTxt->text = StrCombineAndFree(outTxt->text, "> ");
        outTxt->text = StrCombineAndFree(outTxt->text, inTxt->textComp->text);
        
        // Get cmd str
        const char* cmd = StrSubstr(inTxt->textComp->text, 0, StrLen(inTxt->textComp->text) - 1);

        // Reset input
        _Free(inTxt->textComp->text);
        inTxt->textComp->text = StrCopy("");

        // Handle commands
        if (StrEquals(cmd, "cls"))
        {
            Cls();
        }

        // Free
        _Free(cmd);
    }
}