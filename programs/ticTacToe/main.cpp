#include "main.h"

GuiInstance* guiInstance;
List<BaseComponent*>* buttons;
int clicked[9];
uint32_t cols[2] = {Colors.bblue, Colors.bred};
const char* texts[2] = {"Blue", "Red"};
int turn;
bool inGame;
ButtonComponent* restartBtn;
Window* window;

int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    _Free(window->Title);
    window->Title = StrCopy("Tic Tac Toe");
    setWindow(window);

    window->Dimensions.width = 300;
    window->Dimensions.height = 320;

    guiInstance = new GuiInstance(window);
    guiInstance->Init();

    buttons = new List<BaseComponent*>();

    guiInstance->CreateComponentWithId(1021, ComponentType::BUTTON);
    restartBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1021);
    restartBtn->position.x = 52;
    restartBtn->position.y = 0;
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
    restartBtn->size.FixedX = 50;
    restartBtn->size.FixedY = 20;
    restartBtn->OnMouseClickHelp = NULL;
    restartBtn->OnMouseClickedCallBack = OnRestartClicked;

    for (int i = 0; i < 9; i++)
    {
        guiInstance->CreateComponentWithId(1040 + i, ComponentType::BUTTON);
        ButtonComponent* tempBtn = (ButtonComponent*)guiInstance->GetComponentFromId(1040 + i);
        buttons->Add(tempBtn);

        tempBtn->position.x = (i % 3) * 33;
        tempBtn->position.y = 20 + (i / 3) * 33;
        _Free(tempBtn->textComp->text);
        tempBtn->textComp->text = StrCopy(" ");
        tempBtn->size.FixedX = 33;
        tempBtn->size.FixedY = 33;
        tempBtn->bgColDef = Colors.white;
        tempBtn->bgColHover = Colors.gray;
        tempBtn->bgColClick = Colors.dgray;
        
        tempBtn->OnMouseClickHelp = NULL;
        tempBtn->OnMouseClickedCallBack = OnFieldClicked;
    }

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

void UpdateSizes()
{
    restartBtn->size.FixedX = StrLen(restartBtn->textComp->text) * 8 + 16;
    restartBtn->size.FixedY = 16;    
    restartBtn->position.x = (window->Dimensions.width - restartBtn->size.FixedX) / 2;
    restartBtn->position.y = 0;

    for (int i = 0; i < 9; i++)
    {
        int w  = window->Dimensions.width / 3;
        int h  = (window->Dimensions.height - 20) / 3;
        ButtonComponent* tempBtn = (ButtonComponent*)buttons->ElementAt(i);
        tempBtn->position.x = (i % 3) * w;
        tempBtn->position.y = 20 + (i / 3) * h;
        tempBtn->size.FixedX = w;
        tempBtn->size.FixedY = h;
    }
}

void Restart()
{
    _Free(restartBtn->textComp->text);
    restartBtn->textComp->text = StrCopy("Restart");
    turn = 0;
    inGame = true;
    
    for (int i = 0; i < 9; i++)
    {
        clicked[i] = -1;
        ButtonComponent* tempBtn = (ButtonComponent*)buttons->ElementAt(i);
        tempBtn->bgColDef = Colors.white;
        tempBtn->bgColHover = Colors.gray;
        tempBtn->bgColClick = Colors.dgray;
    }
}

void WinCheck()
{
    if (!inGame)
        return;
    
    int playerWon = -1;

    // Horizontal and Vertical
    if (playerWon == -1)
        for (int i = 0; i < 3; i++)
        {
            if (clicked[i * 3] != -1 && clicked[i * 3] == clicked[i * 3 + 1] && clicked[i * 3 + 1] == clicked[i * 3 + 2])
                playerWon = clicked[i * 3];
            if (clicked[i] != -1 && clicked[i] == clicked[i + 3] && clicked[i + 3] == clicked[i + 6])
                playerWon = clicked[i];
        }

    // Diagonal
    if (playerWon == -1)
    {
        if (clicked[0] != -1 && clicked[0] == clicked[4] && clicked[4] == clicked[8])
            playerWon = clicked[0];
        if (clicked[2] != -1 && clicked[2] == clicked[4] && clicked[4] == clicked[6])
            playerWon = clicked[2];
    }

    // Draw 
    if (playerWon == -1)
    {
        bool draw = true;
        for (int i = 0; i < 9; i++)
            if (clicked[i] == -1)
                draw = false;
        if (draw)
            playerWon = 2;
    }

    if (playerWon == -1)
        return;

    inGame = false;
    _Free(restartBtn->textComp->text);
    if (playerWon == 0 || playerWon == 1)
    {
        restartBtn->textComp->text = StrCopy("Player ");
        restartBtn->textComp->text = StrCombineAndFree(restartBtn->textComp->text, texts[playerWon]);
        restartBtn->textComp->text = StrCombineAndFree(restartBtn->textComp->text, " won!");
    }
    else
    {
        restartBtn->textComp->text = StrCopy("Draw!");
    }
}

void OnRestartClicked(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    Restart();
}

void OnFieldClicked(void* bruh, BaseComponent* btn, MouseClickEventInfo click)
{
    if (!inGame)
        return;
    ButtonComponent* tempBtn = (ButtonComponent*)btn;
    int index = buttons->GetIndexOf(tempBtn);
    if (index == -1)
        return;

    if (clicked[index] != -1)
        return;
    
    tempBtn->bgColClick = cols[turn];
    tempBtn->bgColHover = cols[turn];
    tempBtn->bgColDef = cols[turn];

    clicked[index] = turn; 

    WinCheck();

    turn = (turn + 1) % 2;
}
