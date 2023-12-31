#include "main.h"

GuiInstance* guiInstance;
Window* window;
CanvasComponent* canvas;

const int width = 320;
const int height = 300;

int camera_x = 0;
int camera_y = 0;
float camera_z = 0;

double theta_x = 0;
double theta_y = 0;

float x_values[8] = {1, -1, 1, -1, 1, -1, 1, -1};
float y_values[8] = {1, 1, -1, -1, 1, 1, -1, -1};
float z_values[8] = {3, 3, 3, 3, 4, 4, 4, 4};

int a1 = 0;
int b1 = 0;
int a2 = 0;
int b2 = 0;
char move;
float x_rotated[8];
float y_rotated[8];
float z_rotated[8];
float z_rotated_again[8];
float x_projected[8];
float y_projected[8];

float projection(float coord, float z)
{
    return (float)coord/((float)z+camera_z);
}

float rotate_x(float x, float y, double theta)
{
    return (x * cos(theta) - y * sin(theta));
}

float rotate_y(float x, float y, double theta)
{
    return (x * sin(theta) + y * cos(theta));
}

void HandleMouse()
{
    MouseState* state = envGetMouseState();
    if(state == NULL) return;
    theta_x = (state->MouseX - window->Dimensions.x - (double)width/2)/((double)width/4);
    theta_y = (state->MouseY - window->Dimensions.y - (double)height/2)/((double)height/4);

    _Free(state);
}

void HandleKey(void* bruh, KeyHitEventInfo event)
{
    move = event.Chr;
    if(move == 'd') camera_x-=15;
    if(move == 'a') camera_x+=15;
    if(move == ' ') camera_y+=15;
    if(move == 'q') camera_y-=15;
    if(move == 'w') camera_z-=0.3;
    if(move == 's') camera_z+=0.3;
    /*
    if(move == 'l') theta_x+=0.1;
    if(move == 'j') theta_x-=0.1;
    if(move == 'i') theta_y+=0.1;
    if(move == 'k') theta_y-=0.1;
    */
}

int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("3D Kub");

    window->Dimensions.width = 300;
    window->Dimensions.height = 320;
    window->Resizeable = false;

    setWindow(window);

    guiInstance = new GuiInstance(window);
    guiInstance->Init();

    guiInstance->screen->backgroundColor = Colors.white;
    guiInstance->CreateComponentWithId(1400, ComponentType::CANVAS);
    canvas = (CanvasComponent*)guiInstance->GetComponentFromId(1400);
    canvas->position.x = 0;
    canvas->position.y = 0;
    canvas->size.IsXFixed = false;
    canvas->size.ScaledX = 1;
    canvas->size.IsYFixed = false;
    canvas->size.ScaledY = 1;
    canvas->bgColor = Colors.white;
    canvas->CheckUpdates();
    canvas->Clear();
    canvas->KeyHitFunc = HandleKey;

    MainLoop();

    return 0;
}

void MainLoop()
{
    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        HandleFrame();
        guiInstance->Render(false);

        programWaitMsg();
    }
}

void HandleFrame()
{
    canvas->Clear();
    HandleMouse();
    //canvas->DrawLine(2, 2, 30, 30, Colors.black, 0);
    /*
    for(int i = 0; i < 504; i++)
    {
        canvas->DrawPixel((int)(cos((double)((double)i/80.0))*150+150), (int)(sin((double)((double)i/80.0))*150+160), Colors.black, true);
    }
    */
    //canvas->DrawTri(5, 50, 30, 10, 25, 70, Colors.black, 0);

    for(int i = 0; i < 8; i++)
    {
        x_rotated[i] = rotate_x(x_values[i], z_values[i], theta_x);
        z_rotated[i] = rotate_y(x_values[i], z_values[i], theta_x);
    }

    for(int i = 0; i < 8; i++)
    {
        y_rotated[i] = rotate_x(y_values[i], z_rotated[i], theta_y);
        z_rotated_again[i] = rotate_y(y_values[i], z_rotated[i], theta_y);
    }

    for(int i = 0; i < 8; i++)
    {
        x_projected[i] = projection(x_rotated[i], z_rotated_again[i]);
        y_projected[i] = projection(y_rotated[i], z_rotated_again[i]);
    }

    int index_a[12] = {0, 1, 2, 2, 4, 5, 6, 6, 0, 1, 2, 3};
    int index_b[12] = {1, 3, 3, 0, 5, 7, 7, 4, 4, 5, 6, 7};

    for(int i = 0; i < 12; i++)
    {
        a1 = (int)(x_projected[index_a[i]]*(float)width/2)+width/2+camera_x;
        b1 = (int)(y_projected[index_a[i]]*(float)height/2)+height/2+camera_y;
        a2 = (int)(x_projected[index_b[i]]*(float)width/2)+width/2+camera_x;
        b2 = (int)(y_projected[index_b[i]]*(float)height/2)+height/2+camera_y;
        if (
            a1 >= -width && b1 >= -height && a1 < 2*width && b1 < 2*height && 
            a2 >= -width && b2 >= -height && a2 < 2*width && b2 < 2*height
        ) 
        canvas->DrawLine(a1, b1, a2, b2, Colors.black, 3);
    }
}
