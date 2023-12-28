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
#include <libm/gui/guiStuff/components/button/buttonComponent.h>
#include <libm/stdio/stdio.h>

void UpdateSizes();
const char* GetPath();
void SetPath(const char* path);
void Reload();

void OnGoUpClick(void* bruh, GuiComponentStuff::BaseComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
bool PathTypeCallBack(void* bruh, GuiComponentStuff::BaseComponent* comp, GuiComponentStuff::KeyHitEventInfo event);

void OnExternalWindowClose(Window* window);
void OnExternalWindowResize(Window* window);

void OnFolderClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
void OnFileClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);
void OnDriveClick(void* bruh, GuiComponentStuff::ButtonComponent* btn, GuiComponentStuff::MouseClickEventInfo info);

void Free();
void ClearLists();