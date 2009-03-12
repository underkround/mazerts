#include "GameConsole.h"

//*** Protected ***
GameConsole::GameConsole(void) {
}

GameConsole* GameConsole::pInstance = 0;// initialize pointer

GameConsole* GameConsole::getInstance () 
{
    if (pInstance == 0)  // is it the first call?
    {  
      pInstance = new GameConsole;
    }
    return pInstance;
}

//*** public ***

void GameConsole::input(char* input)
{
}

void GameConsole::clear(void)
{
}