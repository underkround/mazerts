#include "GameConsole.h"

//it seems these need to be redeclared here, god knows why (and devil won't tell)
TCHAR GameConsole::m_arrMessage[128];
int GameConsole::m_MessageLength = 0;

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

void GameConsole::input(TCHAR input)
{
    m_arrMessage[++m_MessageLength] = input;
}

void GameConsole::clear(void)
{
}