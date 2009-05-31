#include "LameAI.h"
#include "../../Common/DoubleLinkedList.h"
#include "../../Asset/AssetFactory.h"
#include "../../Asset/AssetCollection.h"
#include "../../Terrain/Terrain.h"
#include "../../Asset/Unit.h"
#include "../../Asset/Building.h"
#include "../../Defs/DefManager.h"
#include "../../Console.h"

LameAI::LameAI(Player* player)
{
    //m_vUnits = new vector<UNIT*>();
    //m_vBuildings = new vector<BUILDING*>();
    m_fUpdatetime = 0.0f;
    m_fUpdateInterval = 1.0f;
    m_UnitRatio = 0.4f;
    m_UnitRatio2 = 0.1f;
    m_UnitLimit = 100;
    m_BuildingLimit = 16;
    m_BaseSpread = 25;
    m_pPlayer = player;
    LoadConfigFromFile();
    AssetCollection::registerListener(this);
    m_fUpdatetime = (float)((::rand() / (float)RAND_MAX)) * m_fUpdateInterval;
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
    AssetCollection::unregisterListener(this);
}
#pragma endregion

#pragma region config loading
void LameAI::LoadConfigFromFile(void)
{
    Config & c = * Config::getInstance();
    c.setFilename("../Data/defs/", "AI.ini");
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
        tmp->energyConsumption = c.getValueAsInt(convertToString(buildings[i]), "asset energy consumption", 10);
        tmp->width = c.getValueAsInt(convertToString(buildings[i]), "asset width", 10);
        tmp->height = c.getValueAsInt(convertToString(buildings[i]), "asset height", 10);
        tmp->eType = (BUILDING_TYPE)buildings[i];
        m_vBuildings.push_back(tmp);
    }

    c.updateFloat("AAI", "update interval", m_fUpdateInterval);
    c.updateInt("AAI", "unit limit", m_UnitLimit);
    c.updateInt("AAI", "building limit", m_BuildingLimit);

    c.updateFloat("AAI", "unit ratio", m_UnitRatio);
    c.updateFloat("AAI", "unit ratio finale", m_UnitRatio2);


    c.updateInt("AAI", "cost mod", m_CostMod);
    c.updateInt("AAI", "count mod", m_CountMod);
    c.updateInt("AAI", "kill mod", m_KillMod);
    c.updateInt("AAI", "base spread", m_BaseSpread);
    c.updateInt("AAI", "energy reserve", m_EnergyReserve);
    c.updateInt("AAI", "harvesters", m_HarvesterCount);
}
#pragma endregion

void LameAI::Update(float fFrameTime)
{
    m_fUpdatetime += fFrameTime;
    if(m_fUpdatetime > m_fUpdateInterval) {
        m_fUpdatetime -= m_fUpdateInterval;
        HammerTime();
    }
}

#pragma region functions that do strategic calculations

UNIT_TYPE LameAI::ChooseUnitToBuild(void)
{
    UNIT_TYPE out = UNIT_TYPE_CAR;
    if(FindUnitCount(UNIT_TYPE_MINER) < m_HarvesterCount)
    {
        return UNIT_TYPE_MINER;
    }
    bool advancedPossible = HaveBuilding(BUILDING_TYPE_SCIENCE);
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
            } else if(advancedPossible) {
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
    //if building ratio has been reached we can afford to build more units
    float ratio = (m_BuildingLimit <= buildingcount) ? m_UnitRatio2 : m_UnitRatio;
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
    return m_OwnBuildings[buildingtype] != NULL;
}

int LameAI::FindUnitCount(UNIT_TYPE unittype)
{
    return m_OwnUnitCount[unittype];
}

int LameAI::FindBuildingCount(BUILDING_TYPE buildingtype)
{
    return m_OwnBuildingCount[buildingtype];
}


int LameAI::FindUnitKillCount(UNIT_TYPE unittype)
{
    //TODO: go and find out
    return 10;
}

bool LameAI::CanSupport(BUILDING_TYPE buildingtype)
{
    int ec = 0;
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == buildingtype)
            ec = m_vBuildings[i]->energyConsumption;
    }
    int energyBalance = m_pPlayer->getPredictedEnergyProduction() - (m_pPlayer->getPredictedEnergyConsumption() + ec + m_EnergyReserve);
    //if you want AI to update to run little bit faster here's code that doesn't use predictions
    //int energyBalance = m_pPlayer->getEnergyProduced() - (m_pPlayer->getEnergyConsumed() + ec + m_EnergyReserve);
    if(energyBalance > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LameAI::CanIAffordToBuild(BUILDING_TYPE buildingtype)
{
    int cost = 0;
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == buildingtype) {
            cost = m_vBuildings[i]->cost;
            break;
        }
    }
    if(m_pPlayer->getOre() > cost) return true;
    else return false;
}

bool LameAI::CanIAffordToBuild(UNIT_TYPE unittype)
{
    int cost = 0;
    for(unsigned int i=0;i<m_vUnits.size();++i)
    {
        if(m_vUnits[i]->eType == unittype)
            cost = m_vUnits[i]->cost;
    }
    if(m_pPlayer->getOre() > cost) return true;
    else return false;
}


int LameAI::CountMyBuildings()
{
    int count = 0;
    map<int, int>::iterator it;

    for (it = m_OwnBuildingCount.begin(); it != m_OwnBuildingCount.end(); it++ )
        count += (*it).second;

    return count;
}

int LameAI::CountMyUnits()
{
    int count = 0;
    map<int, int>::iterator it;

    for (it = m_OwnUnitCount.begin(); it != m_OwnUnitCount.end(); it++ )
        count += (*it).second;

    return count;
}

bool LameAI::LocationValidToBuild(int x, int y, BUILDING_TYPE b)
{
    BUILDING* buildingData;
    Terrain* pTerrain = Terrain::getInstance();
    for(unsigned int i=0;i<m_vBuildings.size();++i)
    {
        if(m_vBuildings[i]->eType == b) {
            buildingData = m_vBuildings[i];
            break;
        }
    }
    //check are we inside the map
    if( (x + buildingData->width) > pTerrain->getSize() || (y + buildingData->height) > pTerrain->getSize() )
    {
        return false;
    }
    if( x < 1 || y < 1 )
    {
        return false;
    }
    //check if area is passable
    if( buildingData->width == buildingData->height ) {
        if( !pTerrain->isPassable(x, y, buildingData->width) ) return false;
    }
    else { //not very accurate but is mostly implemented for sanity's sake anyway
        if( !pTerrain->isPassable(x, y, max(buildingData->width, buildingData->height)) ) return false;
    }

    //check how many units are in the area
    const int unitcount = AssetCollection::getUnitsAt(NULL, x, y, buildingData->width, buildingData->height);
    if(unitcount > 0)
    {
        return false;
    }

    //area free!
    return true;
}

void LameAI::FindBaseCenterPoint(unsigned int *xCenter, unsigned int *yCenter)
{
    unsigned int count = 0;
    unsigned int xTotal = 0;
    unsigned int yTotal = 0;

    ListNode<Building*>* pNode = m_OwnBuildingList.headNode();
    while(pNode)
    {
        ++count;
        xTotal += pNode->item->getCenterGridX();
        yTotal += pNode->item->getCenterGridY();
        pNode = pNode->next;
    }

    if(count > 0)
    {
        *xCenter = xTotal/count;
        *yCenter = yTotal/count;
    }
    else
    { //zero buildings! game over man!
        Terrain* pTerrain = Terrain::getInstance();
        *xCenter = (rand() % pTerrain->getSize() / 2)+pTerrain->getSize()/4;
        *yCenter = (rand() % pTerrain->getSize() / 2)+pTerrain->getSize()/4;
    }
}

#pragma endregion

#pragma region functions that perform actions

void LameAI::HammerTime(void)
{
    if(!HaveBuilding(BUILDING_TYPE_YARD) )
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
            if(m_pPlayer->getOre() > 5000)
            { //make some extra factories if we are filthy rich (it's probably caused by units stuck at factory doors :D)
                if( CanSupport(BUILDING_TYPE_FACTORY) ) {
                    BuildBuilding(BUILDING_TYPE_FACTORY);
                } else {
                    BuildBuilding(BUILDING_TYPE_PLANT);
                }
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
}

void LameAI::BuildBuilding(BUILDING_TYPE building)
{
    //int x = 100+CountMyBuildings()*15;
    //int y = 40*m_pPlayer->getId()+20;
    unsigned int x;
    unsigned int y;
    int locationOffsetX;
    int locationOffsetY;
    FindBaseCenterPoint(&x, &y);
    int buildingCount = CountMyBuildings();
    const int tryCountMax = 10;

    if(buildingCount < m_BuildingLimit)
    {
        if(CanIAffordToBuild(building))
        {
            for(int trycount = 0; trycount < tryCountMax; ++trycount)
            {
                /*
                 * antin originelli
                locationOffsetX = (((CountMyBuildings()/5)+1) * m_BaseSpread);
                locationOffsetY = rand() % locationOffsetX;
                locationOffsetX -= locationOffsetY;
                if(rand() % 2 == 1) locationOffsetX *= -1;
                if(rand() % 2 == 1) locationOffsetY *= -1;
                */

                /*
                 * laurin viritys
                int offset = (int)(2 * ((((float)(buildingCount + trycount)/5)+1) * m_BaseSpread));
                locationOffsetX = (rand() % offset) - (offset >> 1);
                locationOffsetY = (rand() % offset) - (offset >> 1);
                */

                /*
                 * hybrid
                 */
                locationOffsetX = (int)(2 * ((((float)(buildingCount + trycount)/5)+1) * m_BaseSpread));
                locationOffsetY = rand() % locationOffsetX;
                locationOffsetX -= locationOffsetY;
                if(rand() % 2 == 1) locationOffsetX *= -1;
                if(rand() % 2 == 1) locationOffsetY *= -1;

                if( LocationValidToBuild(x+locationOffsetX, y+locationOffsetY, building) ) {
                    AssetFactory::createBuilding(m_pPlayer, building, x+locationOffsetX, y+locationOffsetY);
                    return;
                }
            }
            //run out of tries
            //AssetFactory::createBuilding(PlayerManager::getInstance()->getPlayer(0), BUILDING_TYPE_RADAR, 10, 10);
            ++m_iDebug;
        }
    }
    else {
        
    }
}

void LameAI::BuildUnit(UNIT_TYPE unittype)
{
    //find my factory
    //find out what unit to build (implement ChooseUnitToBuild(void))
    //order factory to build that unit

    if(CountMyUnits() < m_UnitLimit)
    {
        if(CanIAffordToBuild(unittype))
        {
            // find factories TODO this is debug/test/stuff
            /*
            ListNode<Building*>* pNode = AssetCollection::getAllBuildings()->headNode();
            Building** factories = new Building*[100];
            int count = 0;
            while (pNode)
            {
                if (pNode->item->getOwner() == m_pPlayer && pNode->item->getTypeTag() == BUILDING_TYPE_FACTORY)
                {
                    if (pNode->item->getState() == IAsset::STATE_ACTIVE)
                    {
                        factories[count] = pNode->item;
                        ++count;
                    }
                }

                pNode = pNode->next;
            }
            */
            Building* factory = m_OwnBuildings[BUILDING_TYPE_FACTORY];
            if (factory)
            {
                AssetFactory::createUnit(m_pPlayer, unittype, factory);
            }

        }
    }
}

void LameAI::handleCreatedAsset(IAsset* instance)
{
    if (instance->getOwner() == m_pPlayer)
    {
        if (instance->getAssetType() == IAsset::BUILDING)
        {
            Building* b = (Building*)instance;
            if (!m_OwnBuildings[b->getTypeTag()]) 
                m_OwnBuildings[b->getTypeTag()] = b;
            m_OwnBuildingList.pushTail(b);
            m_OwnBuildingCount[b->getTypeTag()] = m_OwnBuildingCount[b->getTypeTag()] + 1;
        }
        else if (instance->getAssetType() == IAsset::UNIT)
        {
            m_OwnUnitCount[instance->getTypeTag()] = m_OwnUnitCount[instance->getTypeTag()] + 1;
        }
    }
}

void LameAI::handleReleasedAsset(IAsset* instance)
{
    if (instance->getOwner() == m_pPlayer)
    {
        if (instance->getAssetType() == IAsset::BUILDING)
        {
            Building* b = (Building*)instance;
            m_OwnBuildings.erase(b->getTypeTag());
            m_OwnBuildingList.remove(b);
            m_OwnBuildingCount[b->getTypeTag()] = m_OwnBuildingCount[b->getTypeTag()] - 1;
        }
        else if (instance->getAssetType() == IAsset::UNIT)
        {
            m_OwnUnitCount[instance->getTypeTag()] = m_OwnUnitCount[instance->getTypeTag()] - 1;
        }
    }
}

#pragma endregion
