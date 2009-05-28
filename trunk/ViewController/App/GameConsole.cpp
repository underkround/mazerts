#include "GameConsole.h"
#include "../../Model/Terrain/Terrain.h"
#include "../Terrain/UITerrain.h"
#include "../../Model/Player/Player.h"
#include "../../Model/Player/PlayerManager.h"
#include "../../Model/AI/Lame/LameAI.h"
#include "../../Model/Asset/AssetCollection.h"
#include "../../Model/Asset/Building.h"
#include "../../Model/Common/DoubleLinkedList.h"
#include "../../Model/Console.h"

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
    if(!_tcscmp(m_arrMessage, _T("help")))
    {
        ::MessageBox(NULL,
                     _T("\n \
help \t display this help\n \
spikes \t make uniform spikes to terrain\n \
flat \t flatten terrain\n \
rebuild \t rebuild terrain mesh\n \
idkfa \t give all players a lot of ore\n \
energy \t give all players some power\n \
poor \t everyone loses all resources\n \
nolimits \t set all AI build limits to 100000\n \
recalc \t force power consumption recalculation\n \
suicide \t kill player 1\n \
\n"),
                     _T("game console commands"),
                     NULL);
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
        ut->updateTerrain();
    }
    if(!_tcscmp(m_arrMessage, _T("flat")))
    {
        Terrain*t = Terrain::getInstance();
        t->flattenMap(Terrain::DEFAULT_FLATHEIGHT);
        UITerrain*ut = UITerrain::getInstance();
        ut->updateTerrain();
    }
    if(!_tcscmp(m_arrMessage, _T("rebuild")))
    {
        UITerrain*ut = UITerrain::getInstance();
        ut->updateTerrain();
    }
    if(!_tcscmp(m_arrMessage, _T("idkfa")))
    {
        for(int i=1;i<PlayerManager::getPlayerCount() + 1;++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->setOre(100000);
        }
    }
    if(!_tcscmp(m_arrMessage, _T("energy")))
    {
        for(int i=1;i<PlayerManager::getPlayerCount() + 1;++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->setEnergyProduced(1000);
        }
    }
    if(!_tcscmp(m_arrMessage, _T("poor")))
    {
        for(int i=1;i<PlayerManager::getPlayerCount() + 1;++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->setEnergyProduced(0);
            PlayerManager::getInstance()->getPlayer(i)->setOre(0);
        }
    }
    if(!_tcscmp(m_arrMessage, _T("nolimits")))
    {
        for(int i=1;i<PlayerManager::getPlayerCount() + 1;++i)
        {
            LameAI *tmp = PlayerManager::getInstance()->getPlayer(i)->getLameAI();
            if(tmp)
            {
                tmp->setUnitLimit(100000);
                tmp->setBuildingLimit(100000);
            }
        }
    }
    if(!_tcscmp(m_arrMessage, _T("recalc")))
    {
        for(int i=1;i<PlayerManager::getPlayerCount() + 1;++i)
        {
            PlayerManager::getInstance()->getPlayer(i)->CalculateEnergyBalance();
        }
    }
    if(!_tcscmp(m_arrMessage, _T("suicide")))
    {
        ListNode<Building*>* pNode = AssetCollection::getAllBuildings()->headNode();
        while (pNode)
        {
            Building* b = pNode->item;
            if(b->getOwner()->getId() == 1)
                b->modifyHitpoints(-b->getHitpoints());
            pNode = pNode->next;
        }
    }
}