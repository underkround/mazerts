#include "AntinAI.h"
#include "../Asset/AssetFactory.h"
#include "../Terrain/Terrain.h"

AntinAI::AntinAI(Player* player)
{
    //m_vUnits = new vector<UNIT*>();
    //m_vBuildings = new vector<BUILDING*>();
    LoadConfigFromFile();
    m_fUpdatetime = 0.0f;
    m_pPlayer = player;
}

AntinAI::~AntinAI() 
{
}

#pragma region release resources
void AntinAI::Release(void)
{
    unsigned int i;
    for (i=0; i<m_vUnits.size(); ++i)
    {
        UNIT* pUnit = m_vUnits[i];
        delete pUnit;
    }
    m_vUnits.clear();
}
#pragma endregion

#pragma region config loading
void AntinAI::LoadConfigFromFile(void)
{
    Config & c = * Config::getInstance();
    c.setFilename("../Data/", "AntinAI.ini");
    c.readFile();

    //gather unit specs for simple strategic calculation access purposes
    int units[6] = {1,2,3,4,5,6};
    for(int i=0;i<6;++i)
    {
        UNIT* tmp = new UNIT();
        tmp->strName = c.getValueAsString(convertToString(units[i]), "asset name", "unknown");
        tmp->cost = c.getValueAsInt(convertToString(units[i]), "asset construction cost ore", 10);
        tmp->eType = (UNIT_TYPE)units[i];
    }
}
#pragma endregion

void AntinAI::Update(float fFrameTime)
{
    Config & c = * Config::getInstance();
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > 1.0f) {
        m_fUpdatetime -= 1.0f;
        /*AssetFactory::createBuilding(m_pPlayer, 54, 
            (rand() % (Terrain::getInstance()->getSize())-20)+10,
            (rand() % (Terrain::getInstance()->getSize())-20)+10);*/
    }
}

#pragma region functions that do strategic calculations

UNIT_TYPE AntinAI::ChooseUnitToBuild(void)
{
    UNIT_TYPE out = UNIT_TYPE_CAR;
    int value = 0;
    int i;
    for(i=0;i<UNIT_TYPE_END;++i)
    {
        //
    }
    return out;
}

int AntinAI::CalculateUnitBuildValue(UNIT_TYPE unittype)
{
    int cost = 0;
    cost = FindUnitCost(unittype) * m_CostMod / ( ( 1+(FindUnitCount(unittype)/m_CountMod) ) * ( 1+FindUnitKillCount(unittype) ) );
    return cost;
}

#pragma endregion

#pragma region functions that gather data from model

int AntinAI::FindUnitCost(UNIT_TYPE unittype)
{
    //ei näitä pitäisi tässä konfikista lukea - muuta!
    Config & c = * Config::getInstance();
    switch(unittype)
    {
    case UNIT_TYPE_CAR:
        return c.getValueAsInt("1", "asset construction cost ore", 10);
        break;
    case UNIT_TYPE_TANK:
        return c.getValueAsInt("2", "asset construction cost ore", 20);
        break;
    case UNIT_TYPE_LAUNCHER:
        return c.getValueAsInt("4", "asset construction cost ore", 30);
        break;
    case UNIT_TYPE_SUPER:
        return c.getValueAsInt("3", "asset construction cost ore", 40);
        break;
    case UNIT_TYPE_NUKER:
        return c.getValueAsInt("5", "asset construction cost ore", 50);
        break;
    }
    return 0;
}

bool AntinAI::HaveScienceBuilding(void)
{
    //TODO: go and find out
    return false;
}

int AntinAI::FindUnitCount(UNIT_TYPE unittype)
{
    //TODO: go and find out
    return 0;
}

int AntinAI::FindUnitKillCount(UNIT_TYPE unittype)
{
    //TODO: go and find out
    return 0;
}

#pragma endregion
