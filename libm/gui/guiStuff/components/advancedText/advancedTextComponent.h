#pragma once
#include "../base/baseComponent.h"
#include <libm/window/window.h>
#include <libm/list/list_consoleChar.h>

namespace GuiComponentStuff
{
    enum class GuiInstanceAdvancedTextComponentAttributeType : int32_t
    {
        //FILL_COLOR = 10
    };

    class AdvancedTextComponent : public BaseComponent
    {
        public:

        AdvancedTextComponent(uint32_t foregroundColor, uint32_t backgroundColor, ComponentSize size, BaseComponent* parent);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);


    private:
        
    //Window* window;
    bool printUser;
    void WriteStringIntoList(const char* str, const char* var);
    void WriteStringIntoList(const char* str, const char* var, bool allowEscape);
    List<ConsoleChar>* AddNewLine();
    void Render2();
    void Free();


    public:
    // OSUser* currentUser;
    bool redirectToSerial;
    int32_t scrollX, scrollY, oldScrollX, oldScrollY, oldHeight, oldWidth;
    uint32_t backgroundColor;
    uint32_t foregroundColor;
    List<List<ConsoleChar>*> textData;
    ConsoleChar* tempPixels;
    ConsoleChar* tempPixels2;
    
    //NewTerminalInstance();
    
    void WriteText(const char* text);
    bool RenderCalled;
    void Reload();
    void DoRender();
    void RenderCharChanges();
    void Clear();
    void SetWindow(Window* window);

    void Println();
    void Print(const char* msg);
    void Print(const char* chrs, const char* var);
    void Print(char chr);
    void Print(const char* chrs, uint32_t col);
    void Print(const char* chrs, const char* var, uint32_t col);
    // void Print(const char* chrs, dispVar vars[]);
    // void Print(const char *chrs, dispVar vars[], uint32_t col);
    void DeleteLastCharInLine();

    void Println(const char* msg);
    void Println(const char* chrs, const char* var);
    void Println(const char* chrs, uint32_t col);
    void Println(const char* chrs, const char* var, uint32_t col);
    // void Println(const char* chrs, dispVar vars[]);
    // void Println(const char *chrs, dispVar vars[], uint32_t col);

    };
}