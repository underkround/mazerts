#include "GameConsole.h"
#include "../Terrain/Terrain.h"
#include "../Terrain/UITerrain.h"
#include "../Player/Player.h"
#include "../Player/PlayerManager.h"
#include "../AI/Lame/LameAI.h"

//it seems these need to be redeclared here, god knows why (and devil won't tell)
TCHAR GameConsole::m_arrMessage[128];
int GameConsole::m_MessageLength = 0;

//*** Protected ***
GameConsole::GameConsole(void) {
    m_pGamestate = NULL;
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
    if(!_tcscmp(m_arrMessage, _T("testi"))) //display test message
    {
        ::MessageBox(NULL, _T("testi"), _T("testi"), NULL);
    }
    if(!_tcscmp(m_arrMessage, _T("fog")))
    {
        //pGamestate->getCurrentPlayer()->getFog()->setEnabled(false);
    }
    if(!_tcscmp(m_arrMessage, _T("spikes"))) //make uniform spikes to terrain
    {

        Terrain*t = Terrain::getInstance();
        int spikes = 10;
        int jump = t->getSize()/spikes;
        for(int i=0;i<spikes;++i)
            for(int j=0;j<spikes;++j)
                t->setTerrainHeight(i*jump, j*jump, 255);
        UITerrain*ut = UITerrain::getInstance();
        ut->reCreate();
    }
    if(!_tcscmp(m_arrMessage, _T("flat"))) //flatten terrain
    {
        Terrain*t = Terrain::getInstance();
        t->flattenMap(Terrain::DEFAULT_FLATHEIGHT);
        UITerrain*ut = UITerrain::getInstance();
        ut->reCreate();
    }
    if(!_tcscmp(m_arrMessage, _T("idkfa"))) //make everyone rich
    {
        for(int i=1;i<PlayerManager::getPlayerCount();++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->setOre(100000);
        }
    }
    if(!_tcscmp(m_arrMessage, _T("energy"))) //free power for everyone
    {
        for(int i=1;i<PlayerManager::getPlayerCount();++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->setEnergyProduced(1000);
        }
    }

}