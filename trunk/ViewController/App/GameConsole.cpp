#include "GameConsole.h"
#include "../Terrain/Terrain.h"
#include "../Terrain/UITerrain.h"

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
      clear();
    }
    return pInstance;
}

//*** public ***

void GameConsole::input(TCHAR input)
{
    if(input == 8 && m_MessageLength > 0) //backspace
    {
        m_arrMessage[--m_MessageLength] = '\0';
    }
    else if(input == 13) //enter
    {
        parse();
        clear();
    }
    else if(m_MessageLength < 128)
    {
        m_arrMessage[m_MessageLength++] = input;
    }
}

TCHAR* GameConsole::output()
{
    return m_arrMessage;
}

void GameConsole::clear(void)
{
    for(DWORD i=0;i<128;++i)
    {
        m_arrMessage[i] = '\0';
    }
    m_MessageLength = 0;
}

void GameConsole::parse(void)
{
    if(!_tcscmp(m_arrMessage, _T("testi")))
    {
        ::MessageBox(NULL, _T("testi"), _T("testi"), NULL);
    }
    if(!_tcscmp(m_arrMessage, _T("fog")))
    {
        //pGamestate->getCurrentPlayer()->getFog()->setEnabled(false);
    }
    if(!_tcscmp(m_arrMessage, _T("spikes")))
    {

        Terrain*t = Terrain::getInstance();
        int spikes = 10;
        int jump = t->getSize()/spikes;
        for(int i=0;i<spikes;++i)
            for(int j=0;j<spikes;++j)
                t->setTerrainHeight(i*jump, j*jump, 255);

        UITerrain*ut = UITerrain::getInstance();
        ut->increaseChangeCounter();
    }
}