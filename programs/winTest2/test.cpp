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

char buffer[512];

int main(int argc, char** argv)
{
    //return 0;
    initWindowManagerStuff();
    programWait(2000);
    
    uint64_t pid = getPid();

    Window* window = requestWindow();

    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Testo!");
    window->Dimensions.y += 300;
    window->Dimensions.width = 150;
    window->Dimensions.height = 200;
    setWindow(window);

    programWait(500);


    ENV_DATA* env = envData;

    TempRenderer* renderer = new TempRenderer(window->Buffer, env->globalFont);

    renderer->Clear(Colors.dblue);
    renderer->Println("lol");

    // send update for full window
    SendWindowFrameBufferUpdate(window);
    
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
            
            //btn->mouseClickedCallBack = TestClickHandler;
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

        testGui->screen->children->Add(testRect);


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



    while (true)
    {
        for (int i = 0; i < 5; i++)
        {
            testGui->Render(true);
            // testRect->position.x += 5;
            // if (testRect->position.x > 300)
            //     testRect->position.x = 0;
            // testRect->position.y += 3;
            // if (testRect->position.y > 100)
            //     testRect->position.y = 0;
            
            //programYield();
            programWaitMsg();
        }

        // MouseState* mState = envGetMouseState();
        // window->Dimensions.y = mState->MouseY;
        // _Free(mState);
        
        // window->Dimensions.x += 10;
        // if (window->Dimensions.x > 500)
        //     window->Dimensions.x = 300;
        // setWindow(window);

        programWait(20);
    }

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
