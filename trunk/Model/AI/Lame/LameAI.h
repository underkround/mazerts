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

//circular dependency here
class Player;

using namespace std;

#pragma endregion
/**
 * LameAI
 *
 * k, this is named after me because I'm pretty sure you guys don't agree with my ideas
 * about this issue and want to make your own AI, if nobody does and this works ok
 * then we will refactor it's name
 */
class LameAI : public IAI
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

    };
    struct BUILDING
    {
        string                    strName;
        LameAI::BUILDING_TYPE    eType;
        int                       cost;
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
    void BuildUnit(void);

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
#pragma endregion

#pragma region functions that gather data from model
    /**
     * @return cost to build this type of building
     */
    int FindUnitCost(BUILDING_TYPE buildingtype);

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


#pragma endregion

#pragma region members
    // ** Unit AI configuration values ** (these are set from AI config file)
    vector<UNIT*>        m_vUnits;
    vector<BUILDING*>    m_vBuildings;

    Player*              m_pPlayer;

    int m_CostMod;
    int m_CountMod;
    int m_KillMod;
    float m_fUpdatetime;
#pragma endregion
};

//#endif //_ANTIN_AI