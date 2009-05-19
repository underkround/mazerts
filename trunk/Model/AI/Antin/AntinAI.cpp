#include "AntinAI.h"
#include "../Common/DoubleLinkedList.h"
#include "../Asset/AssetFactory.h"
#include "../Asset/AssetCollection.h"
#include "../Terrain/Terrain.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"

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
        m_vUnits.push_back(tmp);
    }
    int buildings[9] = {52, 53, 54, 55, 56,
                        57, 58, 59, 60};
    for(int i=0;i<9;++i)
    {
        BUILDING* tmp = new BUILDING();
        tmp->strName = c.getValueAsString(convertToString(buildings[i]), "asset name", "unknown");
        tmp->cost = c.getValueAsInt(convertToString(buildings[i]), "asset construction cost ore", 10);
        tmp->eType = (BUILDING_TYPE)buildings[i];
        m_vBuildings.push_back(tmp);
    }

}
#pragma endregion

void AntinAI::Update(float fFrameTime)
{
    Config & c = * Config::getInstance();
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > 1.0f) {
        m_fUpdatetime -= 1.0f;
        HammerTime();
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
    for(unsigned int i=0;i<m_vUnits.size();++i)
    {
        if(m_vUnits[i]->eType == unittype)
            return m_vUnits[i]->cost;
    }
    return 0;
}

int AntinAI::FindUnitCost(BUILDING_TYPE buildingtype)
{
    Config & c = * Config::getInstance();
    switch(buildingtype)
    {
    case BUILDING_TYPE_FACTORY: //war factory
        return c.getValueAsInt("52", "asset construction cost ore", 250);
        break;
    case BUILDING_TYPE_YARD: //resource yard
        return c.getValueAsInt("53", "asset construction cost ore", 200);
        break;
    case BUILDING_TYPE_PLANT: //power plant
        return c.getValueAsInt("54", "asset construction cost ore", 80);
        break;
    case BUILDING_TYPE_BORE: //deep bore mine
        return c.getValueAsInt("55", "asset construction cost ore", 400);
        break;
    case BUILDING_TYPE_RADAR: //radar building
        return c.getValueAsInt("56", "asset construction cost ore", 100);
        break;
    case BUILDING_TYPE_SCIENCE: //science building
        return c.getValueAsInt("57", "asset construction cost ore", 400);
        break;
    case BUILDING_TYPE_SILO: //nuke silo
        return c.getValueAsInt("58", "asset construction cost ore", 1500);
        break;
    case BUILDING_TYPE_GUNTOWER: //gun tower
        return c.getValueAsInt("59", "asset construction cost ore", 200);
        break;
    case BUILDING_TYPE_CANTOWER: //cannon tower
        return c.getValueAsInt("60", "asset construction cost ore", 300);
        break;
    }
    return 0;
}

bool AntinAI::HaveBuilding(BUILDING_TYPE buildingtype)
{
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            if(pNode->item->getTypeTag() == buildingtype)
            {
                return true;
            }
        }
        pNode = pNode->next;
    }
    return false;
}

int AntinAI::FindUnitCount(UNIT_TYPE unittype)
{
    int count = 0;
    DoubleLinkedList<Unit*>* units = AssetCollection::getAllUnits();
    ListNode<Unit*>* pNode = units->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            if(pNode->item->getTypeTag() == unittype)
            {
                ++count;
            }
        }
        pNode = pNode->next;
    }
    return count;
}

int AntinAI::FindBuildingCount(BUILDING_TYPE buildingtype)
{
    int count = 0;
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            if(pNode->item->getTypeTag() == buildingtype)
            {
                ++count;
            }
        }
        pNode = pNode->next;
    }
    return count;
}


int AntinAI::FindUnitKillCount(UNIT_TYPE unittype)
{
    //TODO: go and find out
    return 0;
}

bool AntinAI::CanSupport(BUILDING_TYPE buildingtype)
{
    int cost = 0;
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == buildingtype)
            cost = m_vBuildings[i]->cost;
    }
    int energyBalance = (m_pPlayer->getEnergyProduced() - m_pPlayer->getEnergyConsumed()) - cost;
    if(energyBalance >= 0) return true;
    return false;
}

bool AntinAI::CanIAffordToBuild(BUILDING_TYPE buildingtype)
{
    return false;
}

int AntinAI::CountMyBuildings()
{
    //yes it is a bit silly to count them here, should have added new method to asset collection instead
    int count = 0;
    DoubleLinkedList<Building*>* buildings = AssetCollection::getAllBuildings();
    ListNode<Building*>* pNode = buildings->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            ++count;
        }
        pNode = pNode->next;
    }
    return count;
}

int AntinAI::CountMyUnits()
{
    //yes it is a bit silly to count them here, should have added new method to asset collection instead
    int count = 0;
    DoubleLinkedList<Unit*>* units = AssetCollection::getAllUnits();
    ListNode<Unit*>* pNode = units->headNode();
    while(pNode)
    {
        if(pNode->item->getOwner() == m_pPlayer)
        {
            ++count;
        }
        pNode = pNode->next;
    }
    return count;

}



#pragma endregion

#pragma region functions that perform actions

void AntinAI::HammerTime(void)
{
    /*AssetFactory::createBuilding(m_pPlayer, 54, 
    (rand() % (Terrain::getInstance()->getSize())-20)+10,
    (rand() % (Terrain::getInstance()->getSize())-20)+10);*/
    if(!HaveBuilding(BUILDING_TYPE_FACTORY))
    {
        BuildBuilding(BUILDING_TYPE_FACTORY);
    }
    if(!HaveBuilding(BUILDING_TYPE_YARD))
    {
        BuildBuilding(BUILDING_TYPE_YARD);
    }
    if(!HaveBuilding(BUILDING_TYPE_PLANT))
    {
        BuildBuilding(BUILDING_TYPE_PLANT);
    }
    if(!HaveBuilding(BUILDING_TYPE_SCIENCE))
    {
        BuildBuilding(BUILDING_TYPE_SCIENCE);
    }
    if(!HaveBuilding(BUILDING_TYPE_RADAR))
    {
        BuildBuilding(BUILDING_TYPE_RADAR);
    }
    if(!HaveBuilding(BUILDING_TYPE_BORE))
    {
        BuildBuilding(BUILDING_TYPE_BORE);
    }
    if(!HaveBuilding(BUILDING_TYPE_SILO))
    {
        BuildBuilding(BUILDING_TYPE_SILO);
    }
    if(!HaveBuilding(BUILDING_TYPE_GUNTOWER))
    {
        BuildBuilding(BUILDING_TYPE_GUNTOWER);
    }
    if(!HaveBuilding(BUILDING_TYPE_CANTOWER))
    {
        BuildBuilding(BUILDING_TYPE_CANTOWER);
    }
}

void AntinAI::BuildBuilding(BUILDING_TYPE building)
{
    AssetFactory::createBuilding(m_pPlayer, building, 100+CountMyBuildings()*15, 200);
}

void AntinAI::BuildUnit(void)
{
    //find my factory
    //find out what unit to build (implement ChooseUnitToBuild(void))
    //order factory to build that unit
}

#pragma endregion
