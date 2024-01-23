#include <libm/syscallManager.h>
#include <libm/cstrTools.h>
#include <libm/wmStuff/wmStuff.h>
#include <libm/list/list_basics.h>
#include <libm/math.h>
#include <libm/keyboard.h>
#include <libm/msgPackets/windowObjPacket/windowObjPacket.h>
#include <libm/msgPackets/keyPacket/keyPacket.h>
#include <libm/msgPackets/mousePacket/mousePacket.h>
#include <libm/cstr.h>
#include <libm/stdio/stdio.h>

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
bool tempMouseBtns[3];
bool turnMouseOff[3];

Window* window;

void DoDoomInit();
void HandleWinUpdates();
void HandleUpdates();
bool DoFrame(bool force);

using namespace STDIO;

int main(int argc, char** argv)
{
    initWindowManagerStuff();
    initStdio(false);
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("DOOM");
    window->Resizeable = false;
    window->Dimensions.width = DOOM_WIDTH * DOOM_SCALE;
    window->Dimensions.height = DOOM_HEIGHT * DOOM_SCALE;
    window->CaptureMouse = true;
    setWindow(window);
    SendWindowFrameBufferUpdate(window);

    DoDoomInit();

    DoFrame(true);
    SendWindowFrameBufferUpdate(window);

    while (!CheckForWindowClosed(window))
    {
        if (DoFrame(false))
            SendWindowFrameBufferUpdate(window);
    }
    
    return 0;
}

bool DoFrame(bool force)
{
    HandleWinUpdates();

    if (!force && (!window->IsActive || !window->IsCapturing))
    {
        programWait(300);
        return false;
    }

    uint64_t time = envGetTimeMs();
    if (!force && (time < lastTime + 5))
    {
        return false;
    }
    lastTime = time;

    HandleUpdates();

    doom_update();

    uint32_t* toBuff = (uint32_t*)window->Buffer->BaseAddress;
    int toWidth = window->Buffer->Width;
    int toHeight = window->Buffer->Height;
    uint32_t* fromBuff = (uint32_t*)doom_get_framebuffer(4);
    
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

    return true;
}

void HandleWinUpdates()
{
    checkWindowManagerStuff();

    // Window Updates
    {
        bool updateEverHappened = false;
        for (int i = 0; i < 500; i++)
        {
            GenericMessagePacket* wPacket = msgGetConv(window->CONVO_ID_WM_WINDOW_UPDATE);
            if (wPacket != NULL)
            {
                if (wPacket->FromPID == desktopPID)
                {
                    WindowObjectPacket* gotObj = new WindowObjectPacket(wPacket);
                    Window* partialWindow = gotObj->PartialWindow;
                    gotObj->Free();
                    _Free(gotObj);

                    if (partialWindow != NULL)
                    {
                        window->UpdateUsingPartialWindow(partialWindow, true, true, true);
                        updateEverHappened = true;
                        partialWindow->Free();
                        _Free(partialWindow);
                    }
                }
                
                wPacket->Free();
                _Free(wPacket);
            }
            else
                break;
        }
        if (updateEverHappened)
        {
            window->UpdateCheck();
            window->Updates->Clear();
        }
    }
}

void HandleUpdates()
{
    // Keyboard Events
    for (int i = 0; i < 500; i++)
    {
        GenericMessagePacket* mPacket = msgGetConv(window->CONVO_ID_WM_KB_STUFF);
        if (mPacket != NULL)
        {
            if (mPacket->Size >= sizeof(KeyMessagePacket))
            {
                KeyMessagePacket* kbMsg = (KeyMessagePacket*)mPacket->Data;
                
                if (kbMsg->Type == KeyMessagePacketType::KEY_PRESSED)
                    doom_key_down((doom_key_t)kbMsg->KeyChar);
                else if (kbMsg->Type == KeyMessagePacketType::KEY_RELEASE)
                    doom_key_up((doom_key_t)kbMsg->KeyChar);
            }

            mPacket->Free();
            _Free(mPacket);
        }
        else
            break;
    }

    // Mouse Up
    {
        if (turnMouseOff[0])
        {
            turnMouseOff[0] = false;
            if (tempMouseBtns[0])
            {
                tempMouseBtns[0] = false;
                doom_button_up(doom_button_t::DOOM_LEFT_BUTTON);
            }
        }

        if (turnMouseOff[1])
        {
            turnMouseOff[1] = false;
            if (tempMouseBtns[1])
            {
                tempMouseBtns[1] = false;
                doom_button_up(doom_button_t::DOOM_RIGHT_BUTTON);
            }
        }

        if (turnMouseOff[2])
        {
            turnMouseOff[2] = false;
            if (tempMouseBtns[2])
            {
                tempMouseBtns[2] = false;
                doom_button_up(doom_button_t::DOOM_MIDDLE_BUTTON);
            }
        }
    }

    // Mouse Events
    {
        int deltaX = 0;
        int deltaY = 0;

        for (int i = 0; i < 500; i++)
        {
            GenericMessagePacket* mPacket = msgGetConv(window->CONVO_ID_WM_MOUSE_STUFF);
            if (mPacket != NULL)
            {
                if (mPacket->Size >= sizeof(MouseMessagePacket))
                {
                    MouseMessagePacket* mouseMsg = (MouseMessagePacket*)mPacket->Data;

                    // Check for movement
                    if (mouseMsg->Type == MouseMessagePacketType::MOUSE_MOVE)
                    {
                        deltaX += mouseMsg->MouseX;
                        deltaY += mouseMsg->MouseY;
                    }

                    if (mouseMsg->Left)
                    {
                        turnMouseOff[0] = false;
                        if (!tempMouseBtns[0])
                        {
                            tempMouseBtns[0] = true;
                            doom_button_down(doom_button_t::DOOM_LEFT_BUTTON);
                        }
                    }
                    else
                        turnMouseOff[0] = true;

                    if (mouseMsg->Right)
                    {
                        turnMouseOff[1] = false;
                        if (!tempMouseBtns[1])
                        {
                            tempMouseBtns[1] = true;
                            doom_button_down(doom_button_t::DOOM_RIGHT_BUTTON);
                        }
                    }
                    else
                        turnMouseOff[1] = true;

                    if (mouseMsg->Middle)
                    {
                        turnMouseOff[2] = false;
                        if (!tempMouseBtns[2])
                        {
                            tempMouseBtns[2] = true;
                            doom_button_down(doom_button_t::DOOM_MIDDLE_BUTTON);
                        }
                    }
                    else
                        turnMouseOff[2] = true;
                }

                mPacket->Free();
                _Free(mPacket);
            }
            else
                break;
        }

        // Movement
        if (deltaX != 0 || deltaY != 0)
        {
            // serialPrint("MOUSE MOVE (");
            // serialPrint(to_string(deltaX));
            // serialPrint(", ");
            // serialPrint(to_string(deltaY));
            // serialPrintLn(")");

            doom_mouse_move(deltaX * DOOM_SCALE * 2, deltaY * DOOM_SCALE * 2);
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
    print(str);
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
    return (char*)"bruh:programs/doom/assets";
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
    // print("Doom tried to open a file!");
    // print("PATH: \"");
    // print(path);
    // println("\"");
    printlnf("Doom tried to open a file!\nPATH: \"%s\"\nMODE: \"%s\"", path, mode);

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
    // serialPrintLn("Doom tried to write to a file!");
    // serialPrint("PATH: \"");
    // serialPrint(bruhFile->path);
    // serialPrintLn("\"");
    printlnf("Doom tried to write to a file!\nPATH: \"%s\"", bruhFile->path);

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

    for (int i = 0; i < 3; i++)
    {
        tempMouseBtns[i] = false;
        turnMouseOff[i] = false;
    }

    openMallocs = new List<void*>();


    //tempDoomMousePos = MPoint(DOOM_WIDTH * DOOM_SCALE / 2 + window->position.x, DOOM_HEIGHT * DOOM_SCALE / 2 + window->position.y);
    //SetMousePosition(tempDoomMousePos);

    
    lastTime = envGetTimeMs();
    doom_set_resolution(DOOM_WIDTH, DOOM_HEIGHT);

    doom_init(0, NULL, 0);
}