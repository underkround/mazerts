#include "LameAI.h"
#include "../Common/DoubleLinkedList.h"
#include "../Asset/AssetFactory.h"
#include "../Asset/AssetCollection.h"
#include "../Terrain/Terrain.h"
#include "../Asset/Unit.h"
#include "../Asset/Building.h"

#define AI_UPDATE_INTERVAL 1.0f
#define AI_UNIT_LIMIT 100
#define AI_BUILDING_LIMIT 16

LameAI::LameAI(Player* player)
{
    //m_vUnits = new vector<UNIT*>();
    //m_vBuildings = new vector<BUILDING*>();
    LoadConfigFromFile();
    m_fUpdatetime = 0.0f;
    m_pPlayer = player;
}

LameAI::~LameAI() 
{
}

#pragma region release resources
void LameAI::Release(void)
{
    unsigned int i;
    for (i=0; i<m_vUnits.size(); ++i)
    {
        UNIT* pUnit = m_vUnits[i];
        delete pUnit;
    }
    m_vUnits.clear();
    for (i=0; i<m_vBuildings.size(); ++i)
    {
        BUILDING* pBuilding = m_vBuildings[i];
        delete pBuilding;
    }
    m_vBuildings.clear();
}
#pragma endregion

#pragma region config loading
void LameAI::LoadConfigFromFile(void)
{
    Config & c = * Config::getInstance();
    c.setFilename("../Data/defs/", "LameAI.ini");
    c.readFile();

    //gather unit specs for simple strategic calculation access purposes
    int units[6] = {1,2,3,4,5,6};
    for(int i=0;i<6;++i)
    {
        UNIT* tmp = new UNIT();
        tmp->strName = c.getValueAsString(convertToString(units[i]), "asset name", "unknown");
        tmp->cost = c.getValueAsInt(convertToString(units[i]), "asset construction cost ore", 10);
        tmp->eType = (UNIT_TYPE)units[i];
        tmp->bOffensive = (c.getValueAsInt(convertToString(units[i]), "asset weapon tag", 0) > 0) ? true : false;
        tmp->bAdvanced = c.getValueAsBool(convertToString(units[i]), "asset advanced", false);
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

    c.updateFloat("AAI", "unit ratio", m_UnitRatio);
    c.updateInt("AAI", "cost mod", m_CostMod);
    c.updateInt("AAI", "count mod", m_CountMod);
    c.updateInt("AAI", "kill mod", m_KillMod);
}
#pragma endregion

void LameAI::Update(float fFrameTime)
{
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > AI_UPDATE_INTERVAL) {
        m_fUpdatetime -= AI_UPDATE_INTERVAL;
        HammerTime();
    }
}

#pragma region functions that do strategic calculations

UNIT_TYPE LameAI::ChooseUnitToBuild(void)
{
    UNIT_TYPE out = UNIT_TYPE_CAR;
    float value = 0.0f;
    unsigned int i;
    for(i=0;i<m_vUnits.size();++i)
    {
        float cost = (float)m_vUnits[i]->cost / m_CostMod;
        float total = (float)FindUnitCount(m_vUnits[i]->eType) / m_CountMod;
        float kills = (float)FindUnitKillCount(m_vUnits[i]->eType) * m_KillMod;
        float calc = kills / (cost*total);
        if( calc > value && m_vUnits[i]->bOffensive )
        {
            if(!m_vUnits[i]->bAdvanced)
            {
                value = calc;
                out = m_vUnits[i]->eType;
            } else if(HaveBuilding(BUILDING_TYPE_SCIENCE)) {
                value = calc;
                out = m_vUnits[i]->eType;
            }
        }
    }
    return out;
}

int LameAI::CalculateUnitBuildValue(UNIT_TYPE unittype)
{
    int cost = 0;
    cost = FindUnitCost(unittype) * m_CostMod / ( ( 1+(FindUnitCount(unittype)/m_CountMod) ) * ( 1+FindUnitKillCount(unittype) ) );
    return cost;
}

bool LameAI::NeedMoreUnits()
{
    float unitcount = (float)CountMyUnits();
    float buildingcount = (float)CountMyBuildings();
    if( (unitcount * m_UnitRatio / buildingcount) < 1.0f )
        return true; //I need more units!
    else
        return false; //enough units for now
}

bool LameAI::NeedMoreDefensiveStructures()
{
    //let's not need these until they have been actually implemented
    return false;
}

#pragma endregion

#pragma region functions that gather data from model

int LameAI::FindUnitCost(UNIT_TYPE unittype)
{
    for(unsigned int i=0;i<m_vUnits.size();++i)
    {
        if(m_vUnits[i]->eType == unittype)
            return m_vUnits[i]->cost;
    }
    return 0;
}

int LameAI::FindBuildingCost(BUILDING_TYPE buildingtype)
{
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == buildingtype)
            return m_vBuildings[i]->cost;
    }
    return 0;
}

bool LameAI::HaveBuilding(BUILDING_TYPE buildingtype)
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

int LameAI::FindUnitCount(UNIT_TYPE unittype)
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

int LameAI::FindBuildingCount(BUILDING_TYPE buildingtype)
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


int LameAI::FindUnitKillCount(UNIT_TYPE unittype)
{
    //TODO: go and find out
    return 10;
}

bool LameAI::CanSupport(BUILDING_TYPE buildingtype)
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

bool LameAI::CanIAffordToBuild(BUILDING_TYPE buildingtype)
{
    int cost = 0;
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == buildingtype)
            cost = m_vBuildings[i]->cost;
    }
    if(m_pPlayer->getOre() > cost) return true;
    else return false;
}

int LameAI::CountMyBuildings()
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

int LameAI::CountMyUnits()
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

void LameAI::HammerTime(void)
{
    if( !HaveBuilding(BUILDING_TYPE_YARD) )
    {
        BuildBuilding(BUILDING_TYPE_YARD);
    }
    else 
    {
        if( !HaveBuilding(BUILDING_TYPE_FACTORY) )
        {
            BuildBuilding(BUILDING_TYPE_FACTORY);
        }
        else 
        {
            if(NeedMoreUnits())
            {
                BuildUnit(ChooseUnitToBuild());
            }
            else {
                if( !HaveBuilding(BUILDING_TYPE_SCIENCE) )
                {
                    if( CanSupport(BUILDING_TYPE_SCIENCE) ) {
                        BuildBuilding(BUILDING_TYPE_SCIENCE);
                    } else {
                        BuildBuilding(BUILDING_TYPE_PLANT);
                    }
                }
                else
                {
                    if( !NeedMoreDefensiveStructures() )
                    {
                        if( !HaveBuilding(BUILDING_TYPE_RADAR) )
                        {
                            if( CanSupport(BUILDING_TYPE_RADAR) ) {
                                BuildBuilding(BUILDING_TYPE_RADAR);
                            } else {
                                BuildBuilding(BUILDING_TYPE_PLANT);
                            }
                        }
                        else 
                        {
                            if(!HaveBuilding(BUILDING_TYPE_SILO))
                            {
                                if( CanSupport(BUILDING_TYPE_SILO) ) {
                                    BuildBuilding(BUILDING_TYPE_SILO);
                                } else {
                                    BuildBuilding(BUILDING_TYPE_PLANT);
                                }
                            }
                            else 
                            {
                                if( CanSupport(BUILDING_TYPE_BORE) ) {
                                    BuildBuilding(BUILDING_TYPE_BORE);
                                } else {
                                    BuildBuilding(BUILDING_TYPE_PLANT);
                                }
                            }
                        }
                    }
                    else 
                    {
                        //build base defences!
                        /*
                            if(!HaveBuilding(BUILDING_TYPE_GUNTOWER))
                            {
                                BuildBuilding(BUILDING_TYPE_GUNTOWER);
                            }
                            if(!HaveBuilding(BUILDING_TYPE_CANTOWER))
                            {
                                BuildBuilding(BUILDING_TYPE_CANTOWER);
                            }
                        */
                    }
                }
            }
        }
    }
}

void LameAI::BuildBuilding(BUILDING_TYPE building)
{
    if(CountMyBuildings() < AI_BUILDING_LIMIT)
    {
        if(CanIAffordToBuild(building)) AssetFactory::createBuilding(m_pPlayer, building, 100+CountMyBuildings()*15, 40*m_pPlayer->getId()+20 );
    }
}

void LameAI::BuildUnit(UNIT_TYPE unittype)
{
    //find my factory
    //find out what unit to build (implement ChooseUnitToBuild(void))
    //order factory to build that unit

    if(CountMyUnits() < AI_UNIT_LIMIT)
    {
        AssetFactory::createUnit(m_pPlayer, unittype, 100+CountMyUnits()*5, 40*m_pPlayer->getId()+10 );
    }
}

#pragma endregion
