#include "main.h"

Window* window;
GuiInstance* guiInstance;

RectangleComponent* player;
TextComponent* scoreText;

Pipe pipes[PIPE_COUNT];

void Pipe::Update()
{
    top->position.x = x;
    top->position.y = 0;
    top->size.FixedX = width;
    top->size.FixedY = height;

    bottom->position.x = x;
    bottom->position.y = height + gap;
    bottom->size.FixedX = width;
    bottom->size.FixedY = window->Dimensions.height - (height + gap);

    if (x < -20)
        Reset(true);
}

void Pipe::Reset(bool pos)
{
    if (pos)
        x = pipes[(index - 1 + PIPE_COUNT) % PIPE_COUNT].x + pipes[(index - 1 + PIPE_COUNT) % PIPE_COUNT].width + 80 +  RND::RandomInt() % 100;
    gap = 45 + RND::RandomInt() % 50;
    height = 50 + RND::RandomInt() % (((window->Dimensions.width - 50) * 2) / 3);

    Update();
}

void Pipe::Init(int index)
{
    this->index = index;
    GuiComponentStuff::ComponentSize s = GuiComponentStuff::ComponentSize(60, 20);
    top = new GuiComponentStuff::RectangleComponent(0xff009000, s, guiInstance->screen);
    bottom = new GuiComponentStuff::RectangleComponent(0xff009000, s, guiInstance->screen);
    guiInstance->screen->children->Add(top);
    guiInstance->screen->children->Add(bottom);
    
    width = 40;
    height = 100;

    x = 0;

    Reset(false);
}

bool spacePressed = false;
int vel = 0;
bool gameOver = false;
int score = 0;
bool passedPipe = false;

int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Flappy Rocc");
    setWindow(window);

    window->Dimensions.width = 300;
    window->Dimensions.height = 320;
    window->Resizeable = false;

    guiInstance = new GuiInstance(window);
    guiInstance->Init();
    uint32_t bgCol = 0xffA0E0FF;
    guiInstance->screen->backgroundColor = bgCol;
    window->DefaultBackgroundColor = bgCol;
    setWindow(window);

    // Player
    {
        GuiComponentStuff::ComponentSize s = GuiComponentStuff::ComponentSize(15, 15);
        player = new GuiComponentStuff::RectangleComponent(Colors.orange, s, guiInstance->screen);
        guiInstance->screen->children->Add(player);
    }

    // Pipes
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].Init(i);
    }

    // Score
    {
        GuiComponentStuff::Position s = GuiComponentStuff::Position(0, 0);
        scoreText = new GuiComponentStuff::TextComponent(guiInstance->screen, Colors.tblack, Colors.black, "<TEST>", s);
        guiInstance->screen->children->Add(scoreText);
    }

    while (!CheckForWindowClosed(window))
    {
        Restart();
        while (!gameOver)
        {
            guiInstance->Update();
            if (!window->IsActive)
            {
                programWait(100);
                continue;
            }
                
            UpdateSizes();
            Frame();
            guiInstance->Render(false);

            spacePressed &= envGetKeyState(0x39);
            programWait(20);
            spacePressed &= envGetKeyState(0x39);
            programWait(20);
        }

        programWait(500);
        while (!envGetKeyState(0x39) || !window->IsActive)
            guiInstance->Update();
    }

    return 0;
}


void Restart()
{
    gameOver = true;
    score = 0;
    passedPipe = false;
    player->position.x = 20;
    player->position.y = (window->Dimensions.height - player->size.FixedY) / 2; 

    _Free(scoreText->text);
    scoreText->text = StrCopy("PRESS SPACE TO START");

    for (int i = 0; i < PIPE_COUNT; i++)
    {
        if (i == 0)
            pipes[i].x = window->Dimensions.width - 100 + RND::RandomInt() % 50;

        pipes[i].Reset(i != 0);
        pipes[i].Update();
    }

    guiInstance->Update();
    UpdateSizes();
    guiInstance->Render(false);

    spacePressed = false;
    while (!envGetKeyState(0x39) || !window->IsActive)
        guiInstance->Update();
    while (envGetKeyState(0x39) || !window->IsActive)
        guiInstance->Update();
    gameOver = false;
}

void UpdateSizes()
{
    if (!gameOver)
    {
        _Free(scoreText->text);
        scoreText->text = StrCopy("Score: ");
        scoreText->text = StrCombineAndFree(scoreText->text, to_string(score));
    }
    int len = StrLen(scoreText->text);

    scoreText->size.FixedY = 16;
    scoreText->size.FixedX = len * 8;
    
    scoreText->position.y = 10;
    scoreText->position.x = (window->Dimensions.width - scoreText->size.FixedX) / 2;
}

void Frame()
{
    bool space = envGetKeyState(0x39); // space
    if (space && !spacePressed)
    {
        if (vel < 0)
            vel = 0;
        vel += 7;
        if (vel > 15)
            vel = 15;
    }

    spacePressed = space;

    vel -= 2;
    if (vel < -25)
        vel = -25;

    player->position.y -= vel;

    if (player->position.y < 0)
        gameOver = true;

    if (CollisionCheck())
        gameOver = true;

    if (player->position.y + player->size.FixedY > window->Dimensions.height)
        gameOver = true;

    if (gameOver)
        return;


    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].x -= 5;
        pipes[i].Update();
    }
}


bool CollisionCheck()
{
    bool passedAny = false;
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        if (player->position.x + player->size.FixedX > pipes[i].x &&
            player->position.x < pipes[i].x + pipes[i].width)
        {
            passedAny = true;
            if (player->position.y < pipes[i].height ||
                player->position.y + player->size.FixedY > pipes[i].height + pipes[i].gap)
                return true;
        }
    }

    if (passedAny && !passedPipe)
        score++;
    
    passedPipe = passedAny;
    
    return false;
}