#include "advancedTextComponent.h"
#include <libm/stubs.h>
#include <libm/consoleChar.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include <libm/math.h>
#include <libm/rendering/Cols.h>
#include <libm/syscallManager.h>

namespace GuiComponentStuff
{
    void SerialWrite(char x)
    {
        char bruh[2];
        bruh[0] = x;
        bruh[1] = 0;
        serialPrint(bruh);
    }

    void ClearCharArr(ConsoleChar* charArr, int sizeX, int sizeY, uint32_t fg, uint32_t bg)
    {
        ConsoleChar tempChar = ConsoleChar(' ', fg, bg);
        for (int i = 0; i < sizeX * sizeY; i++)
            charArr[i] = tempChar;
    }

    void ReInitCharArrWithSize(ConsoleChar** charArr, int sizeX, int sizeY, uint32_t fg, uint32_t bg)
    {
        *charArr = (ConsoleChar*)_Malloc((sizeX * sizeY * sizeof(ConsoleChar)));
        ClearCharArr(*charArr, sizeX, sizeY, fg, bg);
    }

    AdvancedTextComponent::AdvancedTextComponent(uint32_t foregroundColor, uint32_t backgroundColor, ComponentSize size, BaseComponent* parent)
    {
        RenderFunc = (void (*)(void*, Field))&Render;
        CheckUpdatesFunc = (void (*)(void*))&CheckUpdates;
        MouseClickedFunc = (void (*)(void*, MouseClickEventInfo))&MouseClicked;
        KeyHitFunc = (void (*)(void*, KeyHitEventInfo))&KeyHit;
        DestroyFunc = (void (*)(void*, bool, void (*)(BaseComponent* comp)))&Destroy;
        GetActualComponentSizeFunc = (ComponentSize (*)(void*))&GetActualComponentSize;
        SetAttributeFunc = (bool (*)(void*, int32_t, uint64_t))&SetAttribute;
        GetAttributeFunc = (uint64_t (*)(void*, int32_t))&GetAttribute;
        GetAttributeSizeFunc = (int (*)(void*, int32_t))&GetAttributeSize;

        this->size = size;
        componentType = ADVANCED_TEXT;
        this->parent = parent;
        ComponentSize temp = GetActualComponentSize();
        renderer = new ComponentRenderer(temp);
        renderer->bgCol = backgroundColor;
        renderer->Fill(backgroundColor);
        oldPosition = position;
        oldSize = temp;
        updateFields = new List<Field>(5);


        AddToStack();
        redirectToSerial = false;
        //textData = List<List<ConsoleChar>*>(4);
        RenderCalled = true;
        //this->currentUser = user;
        this->backgroundColor = backgroundColor;
        this->foregroundColor = foregroundColor;
        scrollX = 0;
        scrollY = 0;
        oldScrollX = 0;
        oldScrollY = 0;

        oldWidth = 80;
        oldHeight = 160;
        //tempPixels = (ConsoleChar*)malloc(((size.FixedX/8)*(size.FixedY/16) * sizeof(ConsoleChar)));
        ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
        ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);

        AddNewLine();
        RemoveFromStack();


        CheckUpdates();
        //Render(Field(Position(), GetActualComponentSize()));
    }

    void AdvancedTextComponent::MouseClicked(MouseClickEventInfo info)
    {

    }

    void AdvancedTextComponent::KeyHit(KeyHitEventInfo info)
    {

    }

    void AdvancedTextComponent::CheckUpdates()
    {
        AddToStack();
        bool update = false;
        ComponentSize temp = GetActualComponentSize();
        if (oldSize != temp)
        {
            renderer->Resize(temp, false);
            renderer->Fill(backgroundColor);
            oldSize = temp;
            update = true;
        }
        // if (oldFillColor != fillColor)
        // {
        //     renderer->Fill(fillColor);
        //     oldFillColor = fillColor;
        //     update = true;
        // }
        
        DoRender();

        if (update)
            parent->updateFields->Add(Field(position, temp));

        RemoveFromStack();
    }

    void AdvancedTextComponent::Render(Field field)
    {
        AddToStack();
        if (!hidden)
            renderer->Render(position, field, parent->renderer->componentFrameBuffer);
        RemoveFromStack();
    }

    void AdvancedTextComponent::Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp))
    {
        AddToStack();
        if (callBackFunc != NULL)
            callBackFunc(this);
        renderer->Free();
        updateFields->Free();
        _Free(updateFields);
        Free();
        RemoveFromStack();
    }

    ComponentSize AdvancedTextComponent::GetActualComponentSize()
    {
        if (size.IsXFixed && size.IsYFixed)
            return size;

        ComponentSize temp = ComponentSize(0, 0);
        if (size.IsXFixed)
            temp.FixedX = size.FixedX;
        else
            temp.FixedX = size.ScaledX * parent->GetActualComponentSize().FixedX;

        if (size.IsYFixed)
            temp.FixedY = size.FixedY;
         else
            temp.FixedY = size.ScaledY * parent->GetActualComponentSize().FixedY;

        if (temp.FixedX < 0)
            temp.FixedX = 0;
        if (temp.FixedY < 0)
            temp.FixedY = 0;

        return temp;
    }

    bool AdvancedTextComponent::SetAttribute(int32_t type, uint64_t val)
    {
        // if (type == 10)
        // {
        //     backgroundColor = *((uint32_t*)&val);
        //     return true;
        // }

        return false;
    }

    uint64_t AdvancedTextComponent::GetAttribute(int32_t type)
    {
        uint8_t temp[8];
        for (int i = 0; i < 8; i++)
            temp[i] = 0;

        // if (type == 10)
        //     *((uint32_t*)temp) = fillColor;

        return *((uint64_t*)temp);
    }

    int AdvancedTextComponent::GetAttributeSize(int32_t type)
    {
        // if (type == 10)
        //     return 4;

        return 0;
    }










    void ClearListList(List<List<ConsoleChar>*>* list)
    {
        AddToStack();
        while(list->GetCount() > 0)
        {
            List<ConsoleChar>* tList = list->ElementAt(0);
            tList->Free();
            _Free(tList);
            list->RemoveFirst();
        }
        RemoveFromStack();
    }

    List<ConsoleChar>* AdvancedTextComponent::AddNewLine()
    {
        AddToStack();
        if (redirectToSerial)
        {
            SerialWrite('\r');
            SerialWrite('\n');
        }

        List<ConsoleChar>* list = (List<ConsoleChar>*)_Malloc(sizeof(List<ConsoleChar>), "New List of console chars");
        *list = List<ConsoleChar>(2);
        textData.Add(list);
        RemoveFromStack();
        return list;
    }

    void AdvancedTextComponent::WriteStringIntoList(const char* chrs, const char* var)
    {
        WriteStringIntoList(chrs, var, true);
    }

    void AdvancedTextComponent::WriteStringIntoList(const char* chrs, const char* var, bool allowEscape)
    {   
        AddToStack();

        if ((uint64_t)chrs < 100)
            Panic("STR IS NULL", true);

        //allowEscape = false;
        int len = StrLen(chrs);
        
        if (var != NULL)
            var = StrCopy(var);


        List<ConsoleChar>* currList = textData.ElementAt(textData.GetCount() - 1);
        if (currList == 0)
            Panic("LAST LIST IS NULL", true);


        uint32_t fg = foregroundColor;
        uint32_t bg = backgroundColor;

        for (int index = 0; index < len; index++)
        {
            if (currList == NULL)
                Panic("LAST LIST IS NULL 2", true);

            //GlobalRenderer->Println("AAAAA");
            if (chrs[index] == '\n')
            {
                currList = AddNewLine();
                currList = textData.ElementAt(textData.GetCount() - 1);
            }
            else if (chrs[index] == '\r')
                ;// Maybe implement moving char
            else if (chrs[index] == '{' && allowEscape && var != NULL)
            {
                if (chrs[index + 1] == '}')
                {
                    WriteStringIntoList(var, NULL, false);
                    currList = textData.ElementAt(textData.GetCount() - 1);
                    index++;
                }
            }
            else if (chrs[index] == '\\' && allowEscape)
            {
                if (chrs[index + 1] == '\\')
                {
                    index++;
                    currList->Add(ConsoleChar('\\', fg, bg));
                    if (redirectToSerial)
                        SerialWrite('\\');
                }
                else if (chrs[index + 1] == '%')
                {
                    index++;
                    currList->Add(ConsoleChar('%', fg, bg));
                    if (redirectToSerial)
                        SerialWrite('%');
                }
                else if (chrs[index + 1] == '{')
                {
                    index++;
                    currList->Add(ConsoleChar('{', fg, bg));
                    if (redirectToSerial)
                        SerialWrite('{');
                }
                else if (chrs[index + 1] == '}')
                {
                    index++;
                    currList->Add(ConsoleChar('}', fg, bg));
                    if (redirectToSerial)
                        SerialWrite('}');
                }
                else if (chrs[index + 1] == 'F') // foreground color
                {
                    index++;
                    if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                    {
                        currList->Add(ConsoleChar('?', fg, bg));
                        if (redirectToSerial)
                            SerialWrite('?');
                    }
                    else
                    {
                        index++;
                        fg = ConvertStringToHex(&chrs[index]);
                        // ignore switching col in serial 
                        index += 5;
                    }
                }
                else if (chrs[index + 1] == 'B') // foreground color
                {
                    index++;
                    if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                    {
                        currList->Add(ConsoleChar('?', fg, bg));
                        if (redirectToSerial)
                            SerialWrite('?');
                    }
                    else
                    {
                        index++;
                        bg = ConvertStringToHex(&chrs[index]);
                        // ignore switching col in serial 
                        index += 5;
                    }
                }
                else
                {
                    currList->Add(ConsoleChar(chrs[index], fg, bg));
                    if (redirectToSerial)
                        SerialWrite(chrs[index]);
                }
            }
            else
            {
                currList->Add(ConsoleChar(chrs[index], fg, bg));
                if (redirectToSerial)
                    SerialWrite(chrs[index]);
            }
            
            //currList->add(ConsoleChar(str[i], fg, bg));
        }


        if (var != NULL)
            _Free((void*)var);
        RemoveFromStack();
    }
    

    void AdvancedTextComponent::SetWindow(Window* window)
    {
        //this->window = window;
        if (window != NULL)
        {
            ComponentSize size = GetActualComponentSize();
            oldWidth = size.FixedX;
            oldHeight = size.FixedY;
            //tempPixels = (ConsoleChar*)malloc(((size.FixedX/8)*(size.FixedY/16) * sizeof(ConsoleChar)));
            _Free(tempPixels);
            _Free(tempPixels2);
            ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
            ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
        }
        else
        {
            oldWidth = 80;
            oldHeight = 160;
            //tempPixels = (ConsoleChar*)malloc(((size.FixedX/8)*(size.FixedY/16) * sizeof(ConsoleChar)));
            _Free(tempPixels);
            _Free(tempPixels2);
            ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
            ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
        }
        Clear();
        //window->brenderer->Clear(backgroundColor);
        renderer->Fill(backgroundColor);
    }
        
    void AdvancedTextComponent::WriteText(const char* text)
    {
        AddToStack();
        //window->brenderer->Println(text);
        //window->BlitBackbuffer();
        WriteStringIntoList(text, NULL);
        AddNewLine();
        Render2();
        RemoveFromStack();
    }

    void AdvancedTextComponent::RenderCharChanges()
    {
        AddToStack();
        int sizeX = oldWidth / 8;
        int sizeY = oldHeight / 16;
        ClearCharArr(tempPixels2, sizeX, sizeY, foregroundColor, backgroundColor);

        {
            AddToStack();
            int fy1 = scrollY;
            int fy2 = fy1 + size.FixedY;
            int dy1 = fy1 / 16;
            int dy2 = fy2 / 16;

            int fx1 = scrollX;
            int fx2 = fx1 + size.FixedX;
            int dx1 = fx1 / 8;
            int dx2 = fx2 / 8;

            int sDy = max(dy1, 0);
            int sDx = max(dx1, 0);

            int maxY = min(dy2 - 1, textData.GetCount() - 1);
            for (int dy = sDy; dy <= maxY; dy++)
            {
                List<ConsoleChar>* tList = textData.ElementAt(dy);

                int maxX = min(dx2 - 1, tList->GetCount() - 1);
                for (int dx = sDx; dx <= maxX; dx++)
                {
                    ConsoleChar chr = tList->ElementAt(dx);
                    // if (((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) < 0) || ((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) > (oldWidth / 8)*(oldHeight / 16)))
                    //         Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!");


                    tempPixels2[(dx - dx1) + ((dy - dy1) * (sizeX))] = chr;

                }
            }
            RemoveFromStack(); 
        }

        for (int y = 0; y < sizeY; y++)
            for (int x = 0; x < sizeX; x++)
                if (tempPixels[x + (y * sizeX)] != tempPixels2[x + (y * sizeX)])
                {
                    ConsoleChar chr = tempPixels2[x + (y * sizeX)];
                    // if (chr.chr == ' ')
                    //    chr.chr = '?';
                    // chr.fg = Colors.yellow;
                    tempPixels[x + (y * sizeX)] = chr;
                    renderer->PrintString(chr.chr, Position(x*8 - (scrollX % 8), y*16 - (scrollY % 16)), chr.fg, chr.bg, false);
                    Position temp = position;
                    temp.x += x*8 - (scrollX % 8);
                    temp.y += y*16 - (scrollY % 16);
                    parent->updateFields->Add(Field(temp, ComponentSize(8,16)));
                }




        // for (int y = 0; y < sizeY; y++)
        //     for (int x = 0; x < sizeX; x++)
        //         if (tempPixels[x + (y * sizeX)] != tempPixels2[x + (y * sizeX)])
        //         {
        //             ConsoleChar chr = tempPixels[x + (y * sizeX)];
        //             // if (chr.chr == ' ')
        //             //    chr.chr = '?';
        //             // chr.fg = Colors.yellow;
        //             tempPixels2[x + (y * sizeX)] = chr;
        //             window->renderer->putChar(chr.chr, x*8 - (scrollX % 8), y*16 - (scrollY % 16), chr.fg, chr.bg);
        //         }


        //window->BlitBackbuffer();
        RemoveFromStack();
    }

    void AdvancedTextComponent::Render2()
    {
        RenderCalled = true;
    }

    void AdvancedTextComponent::Reload()
    {
        oldScrollX = scrollX + 1;
        Render2();
    }

    void AdvancedTextComponent::DoRender()
    {
        ComponentSize size = GetActualComponentSize();
        if (scrollX == oldScrollX && scrollY == oldScrollY && oldHeight == size.FixedY && oldWidth == size.FixedX)
            if (!RenderCalled)
                return;
        RenderCalled = false;

        AddToStack();
        // if (window == NULL)
        //     Panic("Trying to Render with window being NULL!", true);


        if (scrollX == oldScrollX && scrollY == oldScrollY && oldHeight == size.FixedY && oldWidth == size.FixedX)
        {
            //osData.drawBackground = !osData.drawBackground;
            RenderCharChanges();
        }
        else
        {
            oldScrollX = scrollX;
            oldScrollY = scrollY;
            if (oldHeight != size.FixedY || oldWidth != size.FixedX)
            {
                oldWidth = size.FixedX;
                oldHeight = size.FixedY;
                _Free(tempPixels);
                ReInitCharArrWithSize(&tempPixels, (size.FixedX/8), (size.FixedY/16), foregroundColor, backgroundColor);
                _Free(tempPixels2);
                ReInitCharArrWithSize(&tempPixels2, (size.FixedX/8), (size.FixedY/16), foregroundColor, backgroundColor);
            }
            else
            {
                ComponentSize temp = GetActualComponentSize();
                parent->updateFields->Add(Field(position, temp));
            }

            renderer->Fill(backgroundColor);
            ClearCharArr(tempPixels, (size.FixedX/8), (size.FixedY/16), foregroundColor, backgroundColor);
            if (textData.GetCount() == 0)
            {
                //window->BlitBackbuffer();
                RemoveFromStack(); 
                return;
            }

            {
                AddToStack();
                int fy1 = scrollY;
                int fy2 = fy1 + size.FixedY;
                int dy1 = fy1 / 16;
                int dy2 = fy2 / 16;

                int fx1 = scrollX;
                int fx2 = fx1 + size.FixedX;
                int dx1 = fx1 / 8;
                int dx2 = fx2 / 8;

                int sDy = max(dy1, 0);
                int sDx = max(dx1, 0);

                int maxY = min(dy2 - 1, textData.GetCount() - 1);
                for (int dy = sDy; dy <= maxY; dy++)
                {
                    List<ConsoleChar>* tList = textData.ElementAt(dy);

                    int maxX = min(dx2 - 1, tList->GetCount() - 1);
                    for (int dx = sDx; dx <= maxX; dx++)
                    {
                        ConsoleChar chr = tList->ElementAt(dx);
                        renderer->PrintString(chr.chr, Position(dx*8-fx1, dy*16-fy1), chr.fg, chr.bg, false);

                        if (((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) < 0) || ((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) > (oldWidth / 8)*(oldHeight / 16)))
                            Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!", true);
                        tempPixels[(dx - dx1) + ((dy - dy1) * (oldWidth / 8))] = chr;
                    }
                }
                RemoveFromStack(); 
            }

            //window->BlitBackbuffer();
            //RemoveFromStack();
        }
        //RenderCharChanges();
        RemoveFromStack();
        return;
    }

    void AdvancedTextComponent::Clear()
    {
        // if (window == NULL)
        //     Panic("Trying to clear window which is NULL!", true);
        AddToStack();
        //window->brenderer->Clear(backgroundColor);
        //window->renderer->Clear(backgroundColor);
        //ClearCharArr(tempPixels,  (oldWidth / 8), (oldHeight / 16), foregroundColor, backgroundColor);
        //ClearCharArr(tempPixels2, (oldWidth / 8), (oldHeight / 16), foregroundColor, backgroundColor);
        ClearListList(&textData);
        AddNewLine();
        Render2();
        //Render();
        //window->BlitBackbuffer();
        RemoveFromStack();
    }

    void AdvancedTextComponent::Free()
    {
        ClearListList(&textData);
        textData.Free();
        _Free(tempPixels);
        _Free(tempPixels2);
    }


    void AdvancedTextComponent::Println()
    {
        AddNewLine();
        Render2();
    }
    void AdvancedTextComponent::Print(const char* msg)
    {
        WriteStringIntoList(msg, NULL, true);
        Render2();
    }
    void AdvancedTextComponent::Print(const char* chrs, const char* var)
    {
        WriteStringIntoList(chrs, var, true);
        Render2();
    }

    char tempCharData[2]; 
    void AdvancedTextComponent::Print(char chr)
    {
        tempCharData[0] = chr;
        tempCharData[1] = 0;
        WriteStringIntoList(tempCharData, NULL, true);
        Render2();
    }
    void AdvancedTextComponent::DeleteLastCharInLine()
    {
        List<ConsoleChar>* currList = textData.ElementAt(textData.GetCount() - 1);
        if (currList->GetCount() > 0)
            currList->RemoveLast();

        Render2();
    }

    void AdvancedTextComponent::Print(const char* chrs, uint32_t col)
    {
        uint32_t tCol = foregroundColor;
        foregroundColor = col;
        Print(chrs);
        foregroundColor = tCol;
    }
    void AdvancedTextComponent::Print(const char* chrs, const char* var, uint32_t col)
    {
        uint32_t tCol = foregroundColor;
        foregroundColor = col;
        Print(chrs, var);
        foregroundColor = tCol;
    }






    void AdvancedTextComponent::Println(const char* msg)
    {
        Print(msg);
        Println();
    }
    void AdvancedTextComponent::Println(const char* chrs, const char* var)
    {
        Print(chrs, var);
        Println();
    }
    void AdvancedTextComponent::Println(const char* chrs, uint32_t col)
    {
        Print(chrs, col);
        Println();
    }
    void AdvancedTextComponent::Println(const char* chrs, const char* var, uint32_t col)
    {
        Print(chrs, var, col);
        Println();
    }
}