#pragma once
#include "../base/baseComponent.h"
#include <libm/window/window.h>

namespace GuiComponentStuff
{
    enum class GuiInstanceScreenComponentAttributeType : int32_t
    {
        WINDOW_ID = 10,
        BACKGROUND_COLOR = 11,
        SELECTED_COMPONENT_ID = 12
    };

    class ScreenComponent;
}

#include "../../../guiInstance.h"

namespace GuiComponentStuff
{
    class ScreenComponent : public BaseComponent
    {
        public:
        BaseComponent* selectedComponent = NULL;
        Window* window;
        List<BaseComponent*>* children;
        uint32_t backgroundColor = 0;
        BaseComponent* tempSelectedComponent = NULL;
        List<Field>* finalUpdatedFields;
        List<Field>* childrenFields;
        List<bool>* childrenHidden;
        GuiInstance* guiInstance;

        ScreenComponent(Window* window, GuiInstance* guiInstance);
        void MouseClicked(MouseClickEventInfo info);
        void KeyHit(KeyHitEventInfo info);
        void Render(Field field);
        void CheckUpdates();
        void Destroy(bool destroyChildren, void (*callBackFunc)(BaseComponent* comp));
        ComponentSize GetActualComponentSize();

        bool SetAttribute(int32_t type, uint64_t val);
        uint64_t GetAttribute(int32_t type);
        int GetAttributeSize(int32_t type);
    };
}