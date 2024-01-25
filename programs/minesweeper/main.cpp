#include "main.h"

Window* window;
GuiInstance* guiInstance;

const uint64_t restartBtnId = 0x1000100;
ButtonComponent* restartBtn = NULL;
bool inGame = false;
int mineCount = 0;
int fieldSize = 0;

const uint64_t flagTxtId = 0x1000200;
const uint64_t timeTxtId = 0x1000201;
TextComponent* flagsRemaining = NULL;
TextComponent* time = NULL;
int timePassed = 0;
uint64_t startTime = 0;
int flagsPlaced = 0;

const uint64_t fieldBgId = 0x1000300;
RectangleComponent* fieldBg = NULL;

const uint64_t fieldStartId = 0x1000400;
List<BaseComponent*>* fields = NULL;
int fieldButtonSize = 38;
int fieldButtonSpace = 2;

enum GameModeEnum {
    EASY,
    MEDIUM,
    HARD
};
GameModeEnum gameMode;

bool** mineField = NULL;
bool** exposedField = NULL;
bool** flagField = NULL;
int exposedNormalLeft = 0;

uint32_t bombColor = Colors.black;
uint32_t flagColor =  Colors.brown;
uint32_t blankColor = Colors.white;
uint32_t numColors[8] = {
    Colors.bblue, // 1 Mine
    Colors.bblue, // 2 Mines
    Colors.green, // 3 Mines
    Colors.orange, // 4 Mines
    Colors.orange, // 5 Mines
    Colors.orange, // 6 Mines
    Colors.red, // 7 Mines
    Colors.red, // 8 Mines
};

void Fail(const char* msg)
{
    serialPrintLn("ERROR:");
    serialPrintLn(msg);
    programCrash();
}

PSF1_FONT* customFont = NULL;
void LoadCustomFont(const char* path)
{
    void* buffer;
    uint64_t byteCount = 0;
    if (!fsReadFile(path, &buffer, &byteCount))
        Fail("LOADING FONT FAILED!");

    PSF1_FONT* font = new PSF1_FONT();

    font->psf1_Header = (PSF1_HEADER*)buffer;
    if (font->psf1_Header->magic[0] != 0x36 || font->psf1_Header->magic[1] != 0x04)
        Fail("FONT MAGIC FAILED!");    

    font->glyphBuffer = (void*)((uint64_t)buffer + sizeof(PSF1_HEADER));
    customFont = font;
}

char GetCharAt(int x, int y)
{
    if (x < 0 || y < 0 || x >= fieldSize || y >= fieldSize)
        return '?';

    if (mineField[y][x])
        return 'B';

    // count mines
    int mCount = 0;
    int xMin = max(0, x - 1);
    int yMin = max(0, y - 1);
    int xMax = min(fieldSize - 1, x + 1);
    int yMax = min(fieldSize - 1, y + 1);
    for (int y = yMin; y <= yMax; y++)
        for (int x = xMin; x <= xMax; x++)
            if (mineField[y][x])
                mCount++;
    if (mCount == 0)
        return ' ';
    
    return '0' + mCount;
}

void MatchFieldSize()
{
    if (fieldSize < 1)
        return;
    
    int tSize = 320 / fieldSize;
    fieldButtonSize = tSize;
    if (fieldButtonSize < 24)
        fieldButtonSize = 24;

    fieldButtonSize -= 3;
    fieldButtonSpace = 3;
}

void SetSizeAndBombCount(int fSize, int mCount)
{
    fieldSize = fSize;
    mineCount = mCount;
    MatchFieldSize();
}

void SetSizeAndBombCount(GameModeEnum mode)
{
    gameMode = mode;
    switch (mode)
    {
    case EASY:
        fieldSize = 9; // 9x9 -> 81 Fields
        mineCount = 10;
        break;
    case MEDIUM:
        fieldSize = 16; // 16x16 -> 256 Fields
        mineCount = 40;
        break;
    case HARD:
        fieldSize = 22; // 22x22 -> 484 Fields
        mineCount = 120;
        break;
    }
    MatchFieldSize();
}

void ReInitBoard();

void FirstInit()
{
    // Set Window Title
    _Free(window->Title);
    window->Title = StrCopy("Minesweeper");
    window->Resizeable = false;
    setWindow(window);

    // Create Gui Instance
    guiInstance = new GuiInstance(window);
    guiInstance->Init();

    // Fields
    fields = new List<BaseComponent*>();

    // Restart Button
    guiInstance->CreateComponentWithId(restartBtnId, ComponentType::BUTTON);
    restartBtn = (ButtonComponent*)guiInstance->GetComponentFromId(restartBtnId);
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
    restartBtn->OnMouseClickHelp = NULL;
    restartBtn->OnMouseClickedCallBack = OnRestartClicked;
    restartBtn->bgColHover = Colors.gray;

    // Flags Remaining  
    guiInstance->CreateComponentWithId(flagTxtId, ComponentType::TEXT);
    flagsRemaining = (TextComponent*)guiInstance->GetComponentFromId(flagTxtId);
    _Free(flagsRemaining->text);
    flagsRemaining->text = StrCopy("Flags: 00");
    flagsRemaining->useFixedSize = true;
    flagsRemaining->center = true;

    // Time
    guiInstance->CreateComponentWithId(timeTxtId, ComponentType::TEXT);
    time = (TextComponent*)guiInstance->GetComponentFromId(timeTxtId);
    _Free(time->text);
    time->text = StrCopy("Time: 00");
    time->useFixedSize = true;
    time->center = true;

    // Field BG
    guiInstance->CreateComponentWithId(fieldBgId, ComponentType::RECT);
    fieldBg = (RectangleComponent*)guiInstance->GetComponentFromId(fieldBgId);
    fieldBg->fillColor = Colors.dgray;

    ReInitBoard();
}

void ReInitBoard()
{
    inGame = false;
    // Set Size
    int tSize = fieldButtonSize * fieldSize + (fieldSize + 1) * fieldButtonSpace;
    window->Dimensions.width = tSize;
    window->Dimensions.height = tSize + 40;
    setWindow(window);

    // Clear Fields
    for (int i = 0; i < fields->GetCount(); i++)
        fields->ElementAt(i)->Destroy(true, NULL);
    fields->Clear();

    // Free Mine Fields
    if (mineField != NULL)
    {
        for (int i = 0; i < fieldSize; i++)
            _Free(mineField[i]);
        _Free(mineField);
    }
    // Create Mine Fields
    mineField = (bool**)_Malloc(fieldSize * sizeof(bool*));
    for (int i = 0; i < fieldSize; i++)
        mineField[i] = (bool*)_Malloc(fieldSize * sizeof(bool));

    // Free Exposed Fields
    if (exposedField != NULL)
    {
        for (int i = 0; i < fieldSize; i++)
            _Free(exposedField[i]);
        _Free(exposedField);
    }
    // Create Exposed Fields
    exposedField = (bool**)_Malloc(fieldSize * sizeof(bool*));
    for (int i = 0; i < fieldSize; i++)
        exposedField[i] = (bool*)_Malloc(fieldSize * sizeof(bool));

    // Free Flag Fields
    if (flagField != NULL)
    {
        for (int i = 0; i < fieldSize; i++)
            _Free(flagField[i]);
        _Free(flagField);
    }
    // Create Flag Fields
    flagField = (bool**)_Malloc(fieldSize * sizeof(bool*));
    for (int i = 0; i < fieldSize; i++)
        flagField[i] = (bool*)_Malloc(fieldSize * sizeof(bool));
    
    
    flagsPlaced = 0;
    startTime = envGetTimeMs();
    timePassed = 0;

    UpdateSizes();
    guiInstance->Render(true);

    // Create Fields
    for (int y = 0; y < fieldSize; y++)
    {
        for (int x = 0; x < fieldSize; x++)
        {
            int tPosMult = fieldButtonSpace + fieldButtonSize;
            guiInstance->CreateComponentWithId(fieldStartId + y * fieldSize + x, ComponentType::BUTTON);
            ButtonComponent* tempBtn = (ButtonComponent*)guiInstance->GetComponentFromId(fieldStartId + y * fieldSize + x);
            fields->Add(tempBtn);

            tempBtn->position.x = x * tPosMult + fieldButtonSpace;
            tempBtn->position.y = y * tPosMult + 40 + fieldButtonSpace;
            _Free(tempBtn->textComp->text);
            tempBtn->textComp->text = StrCopy(" ");
            tempBtn->size.FixedX = fieldButtonSize;
            tempBtn->size.FixedY = fieldButtonSize;
            tempBtn->bgColDef = 0xffD0D0D0;
            tempBtn->bgColHover = Colors.gray;
            tempBtn->bgColClick = Colors.dgray;
            tempBtn->textComp->renderer->font = customFont;
            
            tempBtn->MouseClickedFunc = OnFieldClicked;
        }
        guiInstance->Render(true);
    }

    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
}

void UpdateSizes()
{
    if (inGame)
    timePassed = (envGetTimeMs() - startTime) / 1000;

    // Update Flag Text
    _Free(flagsRemaining->text);
    flagsRemaining->text = StrCombineAndFree(StrCopy("Flags: "), to_string(mineCount - flagsPlaced));

    // Update Time Text
    _Free(time->text);
    time->text = StrCombineAndFree(StrCopy("Time: "), to_string(timePassed));

    // Update Restart Button Pos
    restartBtn->size.FixedX = StrLen(restartBtn->textComp->text) * 8 + 8;
    restartBtn->size.FixedY = 16;
    restartBtn->position.x = ((window->Dimensions.width - restartBtn->size.FixedX) * 4) / 8;
    restartBtn->position.y = 12;

    // Update Flags Remaining Pos
    flagsRemaining->size.FixedX = StrLen(flagsRemaining->text) * 8 + 8;
    flagsRemaining->size.FixedY = 16;
    flagsRemaining->position.x = ((window->Dimensions.width - flagsRemaining->size.FixedX) * 1) / 8;
    flagsRemaining->position.y = 12;

    // Update Time Pos
    time->size.FixedX = StrLen(time->text) * 8 + 8;
    time->size.FixedY = 16;
    time->position.x = ((window->Dimensions.width - time->size.FixedX) * 7) / 8;
    time->position.y = 12;

    // Update Field BG Pos
    fieldBg->position.x = 0;
    fieldBg->position.y = 40;
    fieldBg->size.FixedX = window->Dimensions.width;
    fieldBg->size.FixedY = window->Dimensions.height - 40;
}


int main(int argc, const char** argv)
{
    LoadCustomFont("bruh:programs/minesweeper/assets/zap-vga16.psf");
    
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    GameModeEnum mode = GameModeEnum::EASY;

    if (argc > 0)
    {
        if (StrEquals(argv[0], "easy") || StrEquals(argv[0], "EASY"))
            mode = GameModeEnum::EASY;
        else if (StrEquals(argv[0], "medium") || StrEquals(argv[0], "MEDIUM"))
            mode = GameModeEnum::MEDIUM;
        else if (StrEquals(argv[0], "hard") || StrEquals(argv[0], "HARD"))
            mode = GameModeEnum::HARD;
    }

    SetSizeAndBombCount(mode);
    FirstInit();
    Restart();

    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        UpdateSizes();
        guiInstance->Render(false);

        programWaitMsg();
    }

    return 0;
}

void ActuallyRevealField(int x, int y, char chr);

void Restart()
{
    inGame = false;
    flagsPlaced = 0;
    startTime = envGetTimeMs();
    timePassed = 0;
    
    //serialPrintLn("> Doing Restart!");

    // Reset Exposed Fields
    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
            exposedField[y][x] = false;

    // Reset Flag Fields
    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
            flagField[y][x] = false;

    // Reset Mines
    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
            mineField[y][x] = false;

    if (mineCount > fieldSize * fieldSize)
        mineCount = fieldSize * fieldSize;

    exposedNormalLeft = fieldSize * fieldSize - mineCount;

    // Place Mines
    for (int i = 0; i < mineCount; i++)
    {
        int x = RND::RandomInt() % fieldSize;
        int y = RND::RandomInt() % fieldSize;
        if (mineField[y][x])
            i--;
        else
            mineField[y][x] = true;
    }

    // Set Field Chars (For Debug RN)
    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
        {
            ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSize + x);
            _Free(tempBtn->textComp->text);
            tempBtn->textComp->text = StrCopy(" ");
            
            ActuallyRevealField(x, y, '?');
        }

    // Set Reset Text
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");

    inGame = true;
}

void GameOver()
{
    inGame = false;
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Game Over!");
    UpdateSizes();
    guiInstance->Render(true);
    programWait(1000);
    
    // Reveal mines
    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
        {
            ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSize + x);
            if (mineField[y][x])
            {
                ActuallyRevealField(x, y, 'B');
                tempBtn->bgColDef = Colors.bred;
                guiInstance->Render(true);
            }
            tempBtn->bgColClick = tempBtn->bgColDef;
            tempBtn->bgColHover = tempBtn->bgColDef;
        }
}

void GameWon()
{
    inGame = false;
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("You Won!");
    UpdateSizes();
    guiInstance->Render(true);

    for (int y = 0; y < fieldSize; y++)
        for (int x = 0; x < fieldSize; x++)
        {
            ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSize + x);
            tempBtn->bgColClick = tempBtn->bgColDef;
            tempBtn->bgColHover = tempBtn->bgColDef;
        }
}

void ActuallyRevealField(int x, int y, char chr)
{
    ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSize + x);
    _Free(tempBtn->textComp->text);
    char bruh[2] {chr, 0};
    if (bruh[0] == '?')
        bruh[0] = ' ';
    tempBtn->textComp->text = StrCopy(bruh);
    
    if (chr == ' ')
        tempBtn->textColDef = blankColor;
    else if (chr == 'B')
        tempBtn->textColDef = bombColor;
    else if (chr == '?')
        tempBtn->textColDef = Colors.black;
    else if (chr == 'F')
        tempBtn->textColDef = flagColor;
    else
        tempBtn->textColDef = numColors[chr - '1'];

    if (chr == '?' || chr == 'F')
    {
        tempBtn->bgColDef = 0xffD0D0D0;
        tempBtn->bgColHover = Colors.gray;
        tempBtn->bgColClick = Colors.dgray;
    }
    else
    {
        tempBtn->bgColDef = Colors.white;
        tempBtn->bgColHover = Colors.gray;
        tempBtn->bgColClick = Colors.dgray;
    }

    tempBtn->textColHover = tempBtn->textColDef;
    tempBtn->textColClick = tempBtn->textColDef;
}

void PlaceFlag(int x, int y)
{
    if (!inGame || exposedField[y][x])
        return;

    if (flagField[y][x])
    {
        flagField[y][x] = false;
        flagsPlaced--;
        ActuallyRevealField(x, y, '?');
    }
    else
    {
        flagField[y][x] = true;
        flagsPlaced++;
        ActuallyRevealField(x, y, 'F');
    }
}

void ExposeField(int x, int y)
{
    if (!inGame || exposedField[y][x])
        return;

    if (flagField[y][x])
        return;
    
    char chr = GetCharAt(x,y);
    ActuallyRevealField(x, y, chr);

    if (chr == 'B')
    {
        GameOver();
        return;
    }
    exposedField[y][x] = true;
    exposedNormalLeft--;
    if (exposedNormalLeft < 1)
    {
        GameWon();
        return;
    }

    if (chr == ' ')
    {
        int xMin = max(0, x - 1);
        int yMin = max(0, y - 1);
        int xMax = min(fieldSize - 1, x + 1);
        int yMax = min(fieldSize - 1, y + 1);
        for (int y = yMin; y <= yMax; y++)
            for (int x = xMin; x <= xMax; x++)
                ExposeField(x, y);
    }
}

// void WinCheck()
// {
//     if (!inGame)
//         return;
    
//     int playerWon = -1;

//     // Horizontal and Vertical
//     if (playerWon == -1)
//         for (int i = 0; i < 3; i++)
//         {
//             if (clicked[i * 3] != -1 && clicked[i * 3] == clicked[i * 3 + 1] && clicked[i * 3 + 1] == clicked[i * 3 + 2])
//                 playerWon = clicked[i * 3];
//             if (clicked[i] != -1 && clicked[i] == clicked[i + 3] && clicked[i + 3] == clicked[i + 6])
//                 playerWon = clicked[i];
//         }

//     // Diagonal
//     if (playerWon == -1)
//     {
//         if (clicked[0] != -1 && clicked[0] == clicked[4] && clicked[4] == clicked[8])
//             playerWon = clicked[0];
//         if (clicked[2] != -1 && clicked[2] == clicked[4] && clicked[4] == clicked[6])
//             playerWon = clicked[2];
//     }

//     // Draw 
//     if (playerWon == -1)
//     {
//         bool draw = true;
//         for (int i = 0; i < 9; i++)
//             if (clicked[i] == -1)
//                 draw = false;
//         if (draw)
//             playerWon = 2;
//     }

//     if (playerWon == -1)
//         return;

//     inGame = false;
//     _Free(restartBtn->textComp->text);
//     if (playerWon == 0 || playerWon == 1)
//     {
//         restartBtn->textComp->text = StrCopy("Player ");
//         restartBtn->textComp->text = StrCombineAndFree(restartBtn->textComp->text, texts[playerWon]);
//         restartBtn->textComp->text = StrCombineAndFree(restartBtn->textComp->text, " won!");
//     }
//     else
//     {
//         restartBtn->textComp->text = StrCopy("Draw!");
//     }
// }

void OnRestartClicked(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    Restart();
}

void OnFieldClicked(void* bruh, MouseClickEventInfo click)
{
    if (!inGame)
        return;
    ButtonComponent* btn = (ButtonComponent*)bruh;
    int indx = btn->id - fieldStartId;

    int x = indx % fieldSize;
    int y = indx / fieldSize;

    if (click.LeftClickPressed)
        ExposeField(x, y);
    
    if (click.RightClickPressed)
        PlaceFlag(x, y);
}