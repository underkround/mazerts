#pragma once
#pragma region includes
//#ifdef _ANTIN_AI
#include "../../Constants.h"
#include "../../Common/Config.h"
#include "../../Common/Enums.h"
#include "../IAI.h"
#include "../../Player/Player.h"
#include <string>
#include <vector>
#include <sstream>
#include "../../Asset/IAssetCollectionListener.h"
#include <map>

//circular dependency here
class Player;
class IAsset;
class Building;
class Unit;

using namespace std;

#pragma endregion
/**
 * LameAI
 *
 * k, this is named after me because I'm pretty sure you guys don't agree with my ideas
 * about this issue and want to make your own AI, if nobody does and this works ok
 * then we will refactor it's name
 */
class LameAI : public IAI, public IAssetCollectionListener
{
public:
#pragma region constructors and structures
    LameAI(Player* player);
    virtual ~LameAI(void);

    void Release(void);

    void Update(float fFrameTime);

    struct UNIT //values about units AI cares about
    {
        string                    strName;
        LameAI::UNIT_TYPE        eType;
        string                    strWeaponName;
        bool                    bOffensive; //don't go attacking with weaponless units
        int                        cost;
        bool                    bAdvanced;

    };
    struct BUILDING
    {
        string                    strName;
        LameAI::BUILDING_TYPE    eType;
        int                       cost;
        int                      energyConsumption;
        int                      width;
        int                      height;
        //here probably should be pointer to actual building
    };

#pragma endregion

private:
    void LoadConfigFromFile(void);
    inline void AddUnitValuesToVector(UNIT* addme) { m_vUnits.push_back(addme); }


#pragma region functions that perform actions

    /**
     * make AI perform a single construction decision
     */
    void HammerTime(void);

    /**
     * find a nice spot in a map for a new building and start building it
     */
    void BuildBuilding(BUILDING_TYPE building);

    /**
     * queue some unit in factory
     */
    void BuildUnit(UNIT_TYPE unittype);

#pragma endregion

#pragma region functions that do strategic calculations
    /**
     *  @return UNIT_TYPE that AI should build in current situation
     */
    UNIT_TYPE ChooseUnitToBuild(void);

    int CalculateUnitBuildValue(UNIT_TYPE unittype);

    /**
     * decides whether more units should be constructed now
     */
    bool NeedMoreUnits();

    /**
     * decides whether more defensive structures should be constructed now
     */
    bool NeedMoreDefensiveStructures();

#pragma endregion

#pragma region functions that gather data from model
    /**
     * @return cost to build this type of building
     */
    int FindBuildingCost(BUILDING_TYPE buildingtype);

    /**
     * @param unittype type of unit
     * @return cost to build this type of unit
     */
    int FindUnitCost(UNIT_TYPE unittype);

    /**
     * @param unittype type of unit
     * @return how many units has this unit type killed in total
     */
    int FindUnitKillCount(UNIT_TYPE unittype);

    /**
     * @return does this player have specified type of building built
     */
    bool HaveBuilding(BUILDING_TYPE buildingtype);

    /**
     * @return how many units of UNIT_TYPE this player has
     */
    int FindUnitCount(UNIT_TYPE unittype);

    /**
     * @return how many buildings of BUILDING_TYPE this player has
     */
    int FindBuildingCount(BUILDING_TYPE buildingtype);

    /**
     * counts total amount of different buildings
     */
    int CountMyBuildings();

    /**
     * counts total amount of different units
     */
    int CountMyUnits();

    /**
     * @return can current energy infrastructure afford new building of specified type
     */
    bool CanSupport(BUILDING_TYPE buildingtype);

    /**
     * @return does AI have enough ore to build a new building of specified type
     */
    bool CanIAffordToBuild(BUILDING_TYPE buildingtype);

    /**
     * @return does AI have enough ore to build a new unit of specified type
     */
    bool CanIAffordToBuild(UNIT_TYPE unittype);

    /**
     * Checks if the area is free for building
     * @return true if area has nothing that prevents building
     */
    bool LocationValidToBuild(int x, int y, BUILDING_TYPE b);

#pragma endregion

#pragma region miscellaneous

    /**
     * converts integer to string
     * probably should be at some common library instead of here
     */
    template <class T>
    inline std::string convertToString(const T& t) {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    // from IAssetCollectionListener
    void handleCreatedAsset(IAsset* instance);
    void handleReleasedAsset(IAsset* instance);

public:

    inline void setUnitLimit(int limit) { m_UnitLimit = limit; }
    inline void setBuildingLimit(int limit) { m_BuildingLimit = limit; }

    /**
     * Finds out middlepoint of our base, puts values to *x and *y
     */
    void FindBaseCenterPoint(unsigned int *xCenter, unsigned int *yCenter);

private:

#pragma endregion

#pragma region members
    // ** Unit AI configuration values ** (these are set from AI config file)
    vector<UNIT*>        m_vUnits;
    vector<BUILDING*>    m_vBuildings;

    Player*              m_pPlayer;

    //modifier values define what weights most when AI decides what unit to build next
    int m_CostMod;
    int m_CountMod;
    int m_KillMod;
    float m_UnitRatio;    //unit ratio tells AI how many unit should it have compared to amount of it's buildings
    float m_UnitRatio2;   //unit ratio after building limit has been reached
    float m_fUpdatetime;
    int m_BaseSpread;     //how tightly to lay out the base (bigger number means more spread out)
    int m_EnergyReserve;  //require this much energy reserve even if not needed by structures
    int m_HarvesterCount; //how many ore harvesters AI always wants to have
    float m_fUpdateInterval;
    int m_UnitLimit;
    int m_BuildingLimit;

    DoubleLinkedList<Building*> m_OwnBuildingList;
    map<int, Building*> m_OwnBuildings;
    map<int, int> m_OwnBuildingCount;
    map<int, int> m_OwnUnitCount;

#pragma endregion
};

//#endif //_ANTIN_AI