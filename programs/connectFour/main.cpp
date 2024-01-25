#include "main.h"

Window* window;
GuiInstance* guiInstance;

const uint64_t restartBtnId = 0x1000100;
ButtonComponent* restartBtn = NULL;
bool inGame = false;
int currentTurn = 0;

const uint64_t turnTxtId = 0x1000200;
TextComponent* currentTurnText = NULL;

const uint64_t fieldBgId = 0x1000300;
RectangleComponent* fieldBg = NULL;

const uint64_t fieldStartId = 0x1000400;
List<BaseComponent*>* fields = NULL;
int fieldButtonSize = 37;
int fieldButtonSpace = 3;
int fieldSizeX = 7;
int fieldSizeY = 6;


uint32_t blankColor = Colors.white;
uint32_t player1Color = Colors.bred;
uint32_t player2Color =  Colors.bblue;

const char* playerNames[2] = {"Red", "Blue"};

int** gameBoard = NULL;

void Fail(const char* msg)
{
    serialPrintLn("ERROR:");
    serialPrintLn(msg);
    programCrash();
}


void ReInitBoard();

void FirstInit()
{
    // Set Window Title
    _Free(window->Title);
    window->Title = StrCopy("Connect Four");
    window->Resizeable = false;
    setWindow(window);

    // Create Gui Instance
    guiInstance = new GuiInstance(window);
    guiInstance->Init();

    // Fields
    fields = new List<BaseComponent*>();

    // Current Turn Text
    guiInstance->CreateComponentWithId(turnTxtId, ComponentType::TEXT);
    currentTurnText = (TextComponent*)guiInstance->GetComponentFromId(turnTxtId);
    _Free(currentTurnText->text);
    currentTurnText->text = StrCopy("Turn: Red");
    currentTurnText->useFixedSize = true;
    currentTurnText->center = true;

    // Restart Button
    guiInstance->CreateComponentWithId(restartBtnId, ComponentType::BUTTON);
    restartBtn = (ButtonComponent*)guiInstance->GetComponentFromId(restartBtnId);
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
    restartBtn->OnMouseClickHelp = NULL;
    restartBtn->OnMouseClickedCallBack = OnRestartClicked;
    restartBtn->bgColHover = Colors.gray;

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
    int tSizeX = fieldButtonSize * fieldSizeX + (fieldSizeX + 1) * fieldButtonSpace;
    int tSizeY = fieldButtonSize * fieldSizeY + (fieldSizeY + 1) * fieldButtonSpace;
    window->Dimensions.width = tSizeX;
    window->Dimensions.height = tSizeY + 40;
    setWindow(window);

    // Clear Fields
    for (int i = 0; i < fields->GetCount(); i++)
        fields->ElementAt(i)->Destroy(true, NULL);
    fields->Clear();

    // Free Board
    if (gameBoard != NULL)
    {
        for (int i = 0; i < fieldSizeY; i++)
            _Free(gameBoard[i]);
        _Free(gameBoard);
    }
    // Create Board
    gameBoard = (int**)_Malloc(fieldSizeY * sizeof(int*));
    for (int i = 0; i < fieldSizeY; i++)
        gameBoard[i] = (int*)_Malloc(fieldSizeX * sizeof(int));

    currentTurn = 0;

    UpdateSizes();
    guiInstance->Render(true);

    // Create Fields
    for (int y = 0; y < fieldSizeY; y++)
    {
        for (int x = 0; x < fieldSizeX; x++)
        {
            int tPosMult = fieldButtonSpace + fieldButtonSize;
            guiInstance->CreateComponentWithId(fieldStartId + y * fieldSizeX + x, ComponentType::BUTTON);
            ButtonComponent* tempBtn = (ButtonComponent*)guiInstance->GetComponentFromId(fieldStartId + y * fieldSizeX + x);
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
            
            tempBtn->MouseClickedFunc = OnFieldClicked;
        }
        guiInstance->Render(true);
    }

    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
}

void UpdateSizes()
{
    // Update Turn Text
    _Free(currentTurnText->text);
    currentTurnText->text = StrCombine("Turn: ", playerNames[currentTurn]);

    // Update Turn Pos
    currentTurnText->size.FixedX = StrLen(currentTurnText->text) * 8 + 8;
    currentTurnText->size.FixedY = 16;
    currentTurnText->position.x = ((window->Dimensions.width - currentTurnText->size.FixedX) * 3) / 16;
    currentTurnText->position.y = 12;

    // Update Restart Button Pos
    restartBtn->size.FixedX = StrLen(restartBtn->textComp->text) * 8 + 8;
    restartBtn->size.FixedY = 16;
    restartBtn->position.x = ((window->Dimensions.width - restartBtn->size.FixedX) * 6) / 8;
    restartBtn->position.y = 12;

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

void DrawField(int x, int y, char chr);

void Restart()
{
    inGame = false;
    currentTurn = 0;
    
    // Reset Board
    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX; x++)
            gameBoard[y][x] = 0;


    // Set Field Chars
    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX; x++)
            DrawField(x, y, '?');
        

    // Set Reset Text
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");

    inGame = true;
}



void GameEnd(int pWin)
{
    inGame = false;
    _Free(restartBtn->textComp->text);
    if (pWin == 0 || pWin == 1)
        restartBtn->textComp->text = StrCombine(playerNames[pWin], " won!");
    else
        restartBtn->textComp->text = StrCopy("Draw!");

    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX; x++)
        {
            ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSizeX + x);
            tempBtn->bgColClick = tempBtn->bgColDef;
            tempBtn->bgColHover = tempBtn->bgColDef;
        }
}

void DrawField(int x, int y, char chr)
{
    ButtonComponent* tempBtn = (ButtonComponent*)fields->ElementAt(y * fieldSizeX + x);
    
    if (chr == ' ')
        tempBtn->textColDef = blankColor;
    else if (chr == '1')
        tempBtn->textColDef = player1Color;
    else if (chr == '2')
        tempBtn->textColDef = player2Color;
    else
        tempBtn->textColDef = Colors.black;

    if (chr == '?' || chr == ' ')
    {
        tempBtn->bgColDef = Colors.white;
        tempBtn->bgColHover = Colors.bgray;
        tempBtn->bgColClick = Colors.gray;
    }
    else
    {
        tempBtn->bgColDef = tempBtn->textColDef;
        tempBtn->bgColHover = tempBtn->bgColDef;
        tempBtn->bgColClick = tempBtn->bgColHover;
    }

    tempBtn->textColHover = tempBtn->textColDef;
    tempBtn->textColClick = tempBtn->textColDef;
}

bool CheckWinColor(int col)
{
    // Check Verticals
    for (int y = 0; y < fieldSizeY - 3; y++)
        for (int x = 0; x < fieldSizeX; x++)
            if (gameBoard[y][x] == col && gameBoard[y + 1][x] == col && 
                gameBoard[y + 2][x] == col && gameBoard[y + 3][x] == col)
                return true;

    // Check Horizontals
    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX - 3; x++)
            if (gameBoard[y][x] == col && gameBoard[y][x + 1] == col && 
                gameBoard[y][x + 2] == col && gameBoard[y][x + 3] == col)
                return true;

    // Check Diagonals 1
    for (int y = 0; y < fieldSizeY - 3; y++)
        for (int x = 0; x < fieldSizeX - 3; x++)
            if (gameBoard[y][x] == col && gameBoard[y + 1][x + 1] == col && 
                gameBoard[y + 2][x + 2] == col && gameBoard[y + 3][x + 3] == col)
                return true;

    // Check Diagonals 2
    for (int y = 0; y < fieldSizeY - 3; y++)
        for (int x = 3; x < fieldSizeX; x++)
            if (gameBoard[y][x] == col && gameBoard[y + 1][x - 1] == col && 
                gameBoard[y + 2][x - 2] == col && gameBoard[y + 3][x - 3] == col)
                return true;

    return false;
}

bool CheckDraw()
{
    for (int y = 0; y < fieldSizeY; y++)
        for (int x = 0; x < fieldSizeX; x++)
            if (gameBoard[y][x] == 0)
                return false;
    return true;
}

bool ClickField(int x, int player)
{
    if (!inGame)
        return false;

    // Check if column is full
    if (gameBoard[0][x] != 0)
        return false;


    // Find lowest empty field
    int y = 0;
    while (y < fieldSizeY && gameBoard[y][x] == 0)
        y++;
    y--;

    // Set Field
    gameBoard[y][x] = player + 1;
    DrawField(x, y, player == 0 ? '1' : '2');

    // Check Win
    if (CheckWinColor(player + 1))
        GameEnd(player);
    else if (CheckDraw())
        GameEnd(-1);


    return true;
}

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

    int x = indx % fieldSizeX;
    int y = indx / fieldSizeX;

    if (click.LeftClickPressed)
    {
        if (gameBoard[y][x] != 0)
            return;

        if (ClickField(x, currentTurn))
            currentTurn = (currentTurn + 1) % 2;
    }
}
