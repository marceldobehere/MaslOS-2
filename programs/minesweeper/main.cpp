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
    // Set Size
    int tSize = fieldButtonSize * fieldSize + (fieldSize + 1) * fieldButtonSpace;
    window->Dimensions.width = tSize;
    window->Dimensions.height = tSize + 40;
    setWindow(window);

    // Clear Fields
    for (int i = 0; i < fields->GetCount(); i++)
        fields->ElementAt(i)->Destroy(true, NULL);
    fields->Clear();

    inGame = true;
    flagsPlaced = 0;
    startTime = envGetTimeMs();
    timePassed = 0;

    UpdateSizes();

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
            tempBtn->textComp->text = StrCopy("?");
            tempBtn->size.FixedX = fieldButtonSize;
            tempBtn->size.FixedY = fieldButtonSize;
            tempBtn->bgColDef = Colors.white;
            tempBtn->bgColHover = Colors.bgray;
            tempBtn->bgColClick = Colors.gray;
            
            tempBtn->MouseClickedFunc = OnFieldClicked;
        }
        guiInstance->Render(true);
    }
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
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    SetSizeAndBombCount(GameModeEnum::EASY);
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

void Restart()
{
    // _Free(restartBtn->textComp->text);
    // restartBtn->textComp->text = StrCopy("Restart");
    // turn = 0;
    // inGame = true;
    
    // for (int i = 0; i < 9; i++)
    // {
    //     clicked[i] = -1;
    //     ButtonComponent* tempBtn = (ButtonComponent*)buttons->ElementAt(i);
    //     tempBtn->bgColDef = Colors.white;
    //     tempBtn->bgColHover = Colors.gray;
    //     tempBtn->bgColClick = Colors.dgray;
    // }
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
    serialPrint("> FIELD CLICKED: ");
    serialPrintLn(to_string(indx));

    // int index = buttons->GetIndexOf(tempBtn);
    // if (index == -1)
    //     return;

    // if (clicked[index] != -1)
    //     return;
    
    // tempBtn->bgColClick = cols[turn];
    // tempBtn->bgColHover = cols[turn];
    // tempBtn->bgColDef = cols[turn];

    // clicked[index] = turn; 

    // WinCheck();

    // turn = (turn + 1) % 2;
}
