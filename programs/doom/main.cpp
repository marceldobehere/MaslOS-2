#include <libm/syscallManager.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/gui/guiInstance.h>
#include <libm/list/list_basics.h>
#include <libm/math.h>
#include <libm/keyboard.h>

extern "C"
{
    #define DOOM_IMPLEMENTATION
    #include "pureDoom/pureDoom.h"
}

#define DOOM_WIDTH 320
#define DOOM_HEIGHT 200
bool doomRunning = false;
bool doomInit = false;
uint64_t lastTime = 0;
List<void*>* openMallocs = NULL;


int DOOM_SCALE = 2;

bool doomKeyboardTemp[256];
bool doomMouseTemp[3];
MPoint tempDoomMousePos;

Window* window;

void DoDoomInit();
void DoFrame();


int main(int argc, char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("DOOM");
    window->Resizeable = false;
    window->Dimensions.width = DOOM_WIDTH * DOOM_SCALE;
    window->Dimensions.height = DOOM_HEIGHT * DOOM_SCALE;
    setWindow(window);
    SendWindowFrameBufferUpdate(window);

    DoDoomInit();

    while (!CheckForWindowClosed(window))
    {
        // Get Window Update
        DoFrame();
        SendWindowFrameBufferUpdate(window);
        //programWaitMsg();
    }
    
    return 0;
}

void DoFrame()
{
    if (!window->IsActive)
        return;

    // if (window == NULL ||
    //     window->Dimensions.width != DOOM_WIDTH * DOOM_SCALE ||
    //     window->size.height != DOOM_HEIGHT * DOOM_SCALE)
    // {
    //     window->newSize.width = DOOM_WIDTH * DOOM_SCALE;
    //     window->newSize.height = DOOM_HEIGHT * DOOM_SCALE;
    //     return;
    // }

    if (envGetKeyState(Key_Escape) && envGetKeyState(Key_GeneralControl))
    {
        // window->CaptureMouse = false;
        // setWindow(window);
        return;
    }
    else
    {
        // window->CaptureMouse = true;
        // setWindow(window);
    }

    uint64_t time = envGetTimeMs();
    if (time < lastTime + 5)
    {
        //return;
    }
    lastTime = time;

    //MouseClickState
    //doom_button_t

    //doom_key_t t;
    // for (int i = 0; i < 256; i++)
    // {
    //     if (doomKeyboardTemp[i] != KeyboardScancodeState[i])
    //     {
    //         doomKeyboardTemp[i] = KeyboardScancodeState[i];
    //         doom_key_t key = (doom_key_t)QWERTYKeyboard::Translate((uint8_t)i, false);
    //         if (doomKeyboardTemp[i])
    //             doom_key_down(key);
    //         else
    //             doom_key_up(key);
    //     }
    // }

    // for (int i = 0; i < 3; i++)
    // {
    //     if (doomMouseTemp[i] != MouseClickState[i])
    //     {
    //         doomMouseTemp[i] = MouseClickState[i];
    //         doom_button_t button = (doom_button_t)i;
    //         if (doomMouseTemp[i])
    //             doom_button_down(button);
    //         else
    //             doom_button_up(button);
    //     }
    // }

    tempDoomMousePos = MPoint(DOOM_WIDTH * DOOM_SCALE / 2 + window->Dimensions.x, DOOM_HEIGHT * DOOM_SCALE / 2 + window->Dimensions.y);
    // if (tempDoomMousePos.x != MousePosition.x || 
    //     tempDoomMousePos.y != MousePosition.y)
    // {
    //     int xDiff = MousePosition.x - tempDoomMousePos.x;
    //     int yDiff = MousePosition.y - tempDoomMousePos.y;
    //     SetMousePosition(tempDoomMousePos);
    //     doom_mouse_move(xDiff, yDiff);
    // }

    doom_update();


    uint32_t* toBuff = (uint32_t*)window->Buffer->BaseAddress;
    int toWidth = window->Buffer->Width;
    int toHeight = window->Buffer->Height;
    uint32_t* fromBuff = (uint32_t*)doom_get_framebuffer(4 /* RGBA */);
    
    for (int y = 0; y < DOOM_HEIGHT * DOOM_SCALE; y++)
    {
        for (int x = 0; x < DOOM_WIDTH * DOOM_SCALE; x++)
        {
            int tX = x / DOOM_SCALE;
            int tY = y / DOOM_SCALE;
            int indexFrom = tY * DOOM_WIDTH + tX;
            int indexTo = y * toWidth + x;

            int fromCol = fromBuff[indexFrom]; // RGBA
            int toCol = 0; // ARGB
            toCol |= (fromCol & 0xFF000000); // A
            toCol |= (fromCol & 0x00FF0000) >> 16; // R
            toCol |= (fromCol & 0x0000FF00); // G
            toCol |= (fromCol & 0x000000FF) << 16; // B

            toBuff[indexTo] = toCol;
        }
    }
}








void doomExit(int code)
{
    doomRunning = false;
    doomInit = false;
    programExit(0);
}

void doomPrint(const char* str)
{
    serialPrint("DOOM> ");
    serialPrintLn(str);
    return;
}

void* doomMalloc(int size)
{
    void* bruh = _Malloc(size, "DOOM MALLOC");
    openMallocs->Add(bruh);
    return bruh;
}

void doomFree(void* ptr)
{
    int indx = openMallocs->GetIndexOf(ptr);
    if (indx != -1)
    {
        openMallocs->RemoveAt(indx);
        _Free(ptr);
    }
}

void doomGetTime(int* s, int* u)
{
    long t = envGetTimeMicroS();
    *s = t / 1000000;
    *u = t % 1000000;
}

char* doomGetEnv(const char* var)
{
    return "bruh:programs/doom/assets";
}



struct BruhFile
{
    public:
    char* buffer;
    int bufferLen;
    int pos;
    const char* path;
};



void* doomOpen(const char* path, const char* mode)
{
    //GlobalRenderer->Clear(Colors.black);
    //GlobalRenderer->Println("Doom tried to open a file!", Colors.white);
    //GlobalRenderer->Println("PATH: \"{}\"", path, Colors.white);
    //GlobalRenderer->Println("MODE: \"{}\"", mode, Colors.white);
    serialPrintLn("Doom tried to open a file!");
    serialPrint("PATH: \"");
    serialPrint(path);
    serialPrintLn("\"");

    char* resBuffer = NULL;
    uint64_t resBufferLen = 0;
    if (!fsReadFile(path, (void**)(&resBuffer), &resBufferLen))
    {
        resBuffer = NULL;
    }

    if (resBuffer == NULL)
    {
        //GlobalRenderer->Println("FILE DOES NOT EXIST!", Colors.bred);
        //PIT::Sleep(1000);
        return NULL;
    }
    BruhFile* bruhFile = (BruhFile*)_Malloc(sizeof(BruhFile));
    bruhFile->buffer = resBuffer;
    bruhFile->bufferLen = resBufferLen;
    bruhFile->pos = 0;
    bruhFile->path = StrCopy(path);
    openMallocs->Add((void*)bruhFile);
    openMallocs->Add((void*)bruhFile->buffer);
    openMallocs->Add((void*)bruhFile->path);


    //PIT::Sleep(1000);
    return bruhFile;
}

void doomClose(void* handle)
{
    if (handle == NULL)
        return;
    BruhFile* bruhFile = (BruhFile*)handle;
    doomFree((void*)bruhFile->buffer);
    doomFree((void*)bruhFile->path);
    doomFree((void*)bruhFile);
}

int doomRead(void* handle, void* buffer, int size)
{
    if (handle == NULL)
        return 0;
    int count = 0;
    BruhFile* bruhFile = (BruhFile*)handle;
    for (int i = 0; i < size; i++)
    {
        if (bruhFile->pos >= bruhFile->bufferLen)
            break;
        ((char*)buffer)[i] = bruhFile->buffer[bruhFile->pos];
        bruhFile->pos++;
        count++;
    }
    return count;
}

int doomWrite(void* handle, const void* buffer, int size)
{
    if (handle == NULL)
        return 0;

    BruhFile* bruhFile = (BruhFile*)handle;
    serialPrintLn("Doom tried to write to a file!");
    serialPrint("PATH: \"");
    serialPrint(bruhFile->path);
    serialPrintLn("\"");

    // GlobalRenderer->Clear(Colors.black);
    // GlobalRenderer->Println("Doom tried to write to a file!", Colors.white);
    // GlobalRenderer->Println("PATH: \"{}\"", bruhFile->path, Colors.white);

    programWait(1000);

    return size;
}

int doomSeek(void *handle, int offset, doom_seek_t origin)
{
    if (handle == NULL)
        return 0;
    BruhFile* bruhFile = (BruhFile*)handle;
    int whence = origin;
    // 0-SET, 1-CURR, 2-END

    if (whence == 0) 
    {
        bruhFile->pos = offset;
    }
    else if (whence == 1)
    {
        bruhFile->pos += offset;
    }
    else if (whence == 2)
    {
        bruhFile->pos = bruhFile->bufferLen + offset;
    }
    return 0;
}

int doomTell(void *handle)
{
    if (handle == NULL)
        return 0;
    BruhFile* bruhFile = (BruhFile*)handle;
    return bruhFile->pos;
}

int doomEof(void *handle)
{
    if (handle == NULL)
        return 0;
    BruhFile* bruhFile = (BruhFile*)handle;
    return bruhFile->pos >= bruhFile->bufferLen ? 1 : 0;
}


void DoDoomInit()
{
    doom_print_fn print_fn = doomPrint;
    doom_malloc_fn malloc_fn = doomMalloc;
    doom_free_fn free_fn = doomFree;

    doom_open_fn open_fn = doomOpen;
    doom_close_fn close_fn = doomClose;
    doom_read_fn read_fn = doomRead;
    doom_write_fn write_fn = doomWrite;
    doom_seek_fn seek_fn = doomSeek;
    doom_tell_fn tell_fn = doomTell;
    doom_eof_fn eof_fn = doomEof;

    doom_gettime_fn gettime_fn = doomGetTime;
    doom_exit_fn exit_fn = doomExit;
    doom_getenv_fn getenv_fn = doomGetEnv;

   
    doom_set_print(print_fn);
    doom_set_malloc(malloc_fn, free_fn);
    doom_set_file_io(open_fn,close_fn,read_fn,write_fn,seek_fn,tell_fn,eof_fn);
    doom_set_gettime(gettime_fn);
    doom_set_exit(exit_fn);
    doom_set_getenv(getenv_fn);


    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("mouse_move", 0); // Mouse will not move forward

    doomRunning = true;

    for (int i = 0; i < 256; i++)
        doomKeyboardTemp[i] = false;

    for (int i = 0; i < 3; i++)
        doomMouseTemp[i] = false;

    openMallocs = new List<void*>();


    //tempDoomMousePos = MPoint(DOOM_WIDTH * DOOM_SCALE / 2 + window->position.x, DOOM_HEIGHT * DOOM_SCALE / 2 + window->position.y);
    //SetMousePosition(tempDoomMousePos);

    
    lastTime = envGetTimeMs();
    doom_set_resolution(DOOM_WIDTH, DOOM_HEIGHT);

    doom_init(0, NULL, 0);
}