#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>

#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/stubs.h>

#include <libm/gui/guiInstance.h>
#include <libm/gui/guiStuff/components/box/boxComponent.h>
#include <libm/gui/guiStuff/components/rectangle/rectangleComponent.h>
#include <libm/gui/guiStuff/components/text/textComponent.h>
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/gui/guiStuff/components/textField/textFieldComponent.h>
#include <libm/gui/guiStuff/components/imageRect/imageRectangleComponent.h>


void TestClickHandler(GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo mouse)
{
    serialPrintLn("YOOO BUTTON CLICKED!");
}

char buffer[512];

int main(int argc, char** argv)
{
    // int argc = getArgC();
    // char **argv = getArgV();
    //ENV_DATA *env = getEnvData();
    initWindowManagerStuff();
    
    programWait(1000);
    
    globalPrintLn("Hello from window Test");

    uint64_t pid = getPid();
    globalPrint("A> THIS PID: ");
    globalPrintLn(to_string(pid));

    globalPrint("A> DESKTOP PID: ");
    globalPrintLn(to_string(desktopPID));

    programWait(1000);

    globalPrintLn("A> Requesting Window...");
    Window* window = requestWindow();
    globalPrintLn("A> Requested Window!");

    if (window == NULL)
    {
        globalPrintLn("A> Window is NULL!");
        programWait(1000);
        return 0;
    }

    globalPrint("A> Window ID: ");
    globalPrintLn(ConvertHexToString(window->ID));
    
    globalPrint("A> Window Title (1): \"");
    globalPrint(window->Title);
    globalPrintLn("\"");

    _Free(window->Title);
    window->Title = StrCopy("Hello World!");
    setWindow(window);

    globalPrint("A> Window Title (2): \"");
    globalPrint(window->Title);
    globalPrintLn("\"");


    programWait(1000);

    globalPrintLn("> Window Buffer ADDR 1: ");
    globalPrintLn(ConvertHexToString((uint64_t)window->Buffer));

    globalPrintLn("> Window Buffer ADDR 2: ");
    globalPrintLn(ConvertHexToString((uint64_t)window->Buffer->BaseAddress));

    ENV_DATA* env = envData;

    globalPrintLn("> ENV ADDR: ");
    globalPrintLn(ConvertHexToString((uint64_t)env));

    globalPrintLn("> ENV FONT ADDR: ");
    globalPrintLn(ConvertHexToString((uint64_t)env->globalFont));

    window->Dimensions.width = 500;
    window->Dimensions.height = 400;
    setWindow(window);
    SendWindowFrameBufferUpdate(window);

    TempRenderer* renderer = new TempRenderer(window->Buffer, env->globalFont);

    renderer->Clear(Colors.black);
    renderer->Println("Hello, world!");

    // send update for full window
    SendWindowFrameBufferUpdate(window);
    
    // send update for partial region
    //SendWindowFrameBufferUpdate(window, 0, 0, 40, 40);


    programWait(1000);

    GuiInstance* testGui;
    GuiComponentStuff::BoxComponent* box;
    GuiComponentStuff::RectangleComponent* testRect;
    {
        //Window* window = (Window*)_Malloc(sizeof(Window), "GUI Window");
        testGui = new GuiInstance(window);
        //*(window) = Window((DefaultInstance*)gui, Size(50, 50), Position(500, 100), "Testing GUI Window", true, true, true);
        //osData.windows.add(window);
        //window->hidden = true;
        testGui->Init();

        SendWindowFrameBufferUpdate(window);
        

        {
            GuiComponentStuff::ComponentSize s = GuiComponentStuff::ComponentSize(60, 20);
            
            s.IsXFixed = false;
            s.ScaledX = 0.5;
            
            
            testRect = new GuiComponentStuff::RectangleComponent(Colors.purple, s, testGui->screen);
            testRect->position = GuiComponentStuff::Position(100, 20);
        }
        testGui->screen->children->Add(testRect);
    

        {
            box = new GuiComponentStuff::BoxComponent(
                testGui->screen, GuiComponentStuff::ComponentSize(240, 240), Colors.tblack
            );
            testGui->screen->children->Add(box);
            box->position = GuiComponentStuff::Position(20, 30);

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.dgray, GuiComponentStuff::ComponentSize(180, 180), box);
                t->position = GuiComponentStuff::Position(0, 40);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.white, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(40, 80);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.white, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(120, 80);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.orange, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(80, 120);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(20, 140);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(20, 20), box);
                t->position = GuiComponentStuff::Position(140, 140);
                box->children->Add(t);
            }

            {
                GuiComponentStuff::RectangleComponent* t = new GuiComponentStuff::RectangleComponent(
                    Colors.bred, GuiComponentStuff::ComponentSize(100, 20), box);
                t->position = GuiComponentStuff::Position(40, 160);
                box->children->Add(t);
            }
        }

        


        {
            GuiComponentStuff::TextComponent* txt = new GuiComponentStuff::TextComponent(testGui->screen, Colors.black, Colors.white, "Hello!\nThis is an amazing test.", 
            GuiComponentStuff::Position(200, 90));
            txt->id = 993344;
            testGui->screen->children->Add(txt);
        }

        {
            GuiComponentStuff::ButtonComponent* btn = new GuiComponentStuff::ButtonComponent("CLICK\nME\nPLS", 
            Colors.black, Colors.dgray, Colors.gray, 
            Colors.bgreen, Colors.yellow, Colors.bred, 
            GuiComponentStuff::ComponentSize(150, 80),
            GuiComponentStuff::Position(210, 160), testGui->screen
            );
            btn->mouseClickedCallBack = TestClickHandler;
            //btn->keyHitCallBack = TestKeyHandler;

            //btn->stickToDefaultColor = true;
            btn->id = 995544;
            
            testGui->screen->children->Add(btn);
        }

        {
            GuiComponentStuff::TextFieldComponent* txtField = new GuiComponentStuff::TextFieldComponent(
            Colors.white,
            Colors.black,
            GuiComponentStuff::ComponentSize(150, 80),
            GuiComponentStuff::Position(100, 260), testGui->screen
            );
            //btn->mouseClickedCallBack = TestClickHandler;
            //btn->keyHitCallBack = TestKeyHandler;
            //btn->stickToDefaultColor = true;
            
            testGui->screen->children->Add(txtField);
        }

        {
            GuiComponentStuff::ImageRectangleComponent* imgRect = new GuiComponentStuff::ImageRectangleComponent(
                "bruh:images/rocc.mbif",
                GuiComponentStuff::ComponentSize(150, 80),
                testGui->screen
            );
            imgRect->id = 996655;
            imgRect->position = GuiComponentStuff::Position(300, 260);

            testGui->screen->children->Add(imgRect);
        }
        


        {
            uint64_t bleh = 0;
            *((int*)&bleh) = 1;
            testGui->SetBaseComponentAttribute(993344, GuiInstanceBaseAttributeType::POSITION_Y, bleh);
        }

        {
            uint64_t bleh = 0;
            *((uint32_t*)&bleh) = Colors.red;
            testGui->SetSpecificComponentAttribute(995544, 10, bleh);
        }

        {
            uint64_t bleh = 0;
            *((double*)&bleh) = 0.3;
            testGui->SetBaseComponentAttribute(995544, GuiInstanceBaseAttributeType::SIZE_SCALED_Y, bleh);
        }

        {
            uint64_t bleh = 0;
            *((bool*)&bleh) = false;
            testGui->SetBaseComponentAttribute(995544, GuiInstanceBaseAttributeType::SIZE_IS_FIXED_Y, bleh);
        }
    }



    //programWait(1000);

    uint64_t endTime = envGetTimeMs() + 6000;

    while (!CheckForWindowClosed(window))
    {
        testGui->Render(true);

        testRect->position.x += 1;
        if (testRect->position.x > 300)
            testRect->position.x = 0;
        
        programWait(20);
        //programYield();
        //programWait(500);

        // if (envGetTimeMs() >= endTime)
        //     break;
    }

    programWait(1000);

    window->Free();
    _Free(window);

    programWait(1000);

    window = requestWindow();

    for (int i = 0; i < 10; i++)
    {
        window->Hidden = true;
        setWindow(window);
        programWait(500);

        window->DefaultBorderColor = (uint32_t)RND::RandomInt();
        window->Hidden = false;
        setWindow(window);
        programWait(500);
    }


    programWait(1000);

    // while (true)
    // {

    //     renderer->Clear(40, 40, 100, 100, (uint32_t)RND::RandomInt());
    //     SendWindowFrameBufferUpdate(window, 40, 40, 100, 100);

    //     // _Free(window->Title);
    //     // window->Title = StrCopy("AAA");
    //     // setWindow(window);

    //     programYield();

    //     // _Free(window->Title);
    //     // window->Title = StrCopy("BBB");
    //     // setWindow(window);

    //     // programYield();

    //     // Check for mem leaks
    //     // serialPrint("A> Used Heap Count: ");
    //     // serialPrintLn(to_string(Heap::GlobalHeapManager->_usedHeapCount));
    // }

    return 0;
}
