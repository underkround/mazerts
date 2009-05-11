#pragma once
#pragma region includes
//#ifdef _ANTIN_AI
#include "../../Constants.h"
#include "../../Common/Config.h"
#include "../../Common/Enums.h"
#include "../IAI.h"
#include <string>
#include <vector>

using namespace std;

#pragma endregion
/**
 * AntinAI
 *
 * k, this is named after me because I'm pretty sure you guys don't agree with my ideas
 * about this issue and want to make your own AI, if nobody does and this works ok
 * then we will refactor it's name
 */
class AntinAI : public IAI
{
public:
#pragma region constructors and structures
    AntinAI(void);
    virtual ~AntinAI(void);

    void Release(void);

    void Update(float fFrameTime);

    struct UNIT //values about units AI cares about
    {
        string                    strName;
        AntinAI::UNIT_TYPE        eType;
        string                    strWeaponName;
        bool                    bOffensive; //don't go attacking with weaponless units
        int                        cost;
        //here probably should be pointer to actual unit
    };
    struct BUILDING
    {
        string                    strName;
        AntinAI::BUILDING_TYPE    eType;
        int                        cost;
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

#pragma endregion

#pragma region functions that do strategic calculations
    /**
     *  @return UNIT_TYPE that AI should build in current situation
     */
    UNIT_TYPE ChooseUnitToBuild(void);

    int CalculateUnitBuildValue(UNIT_TYPE unittype);
#pragma endregion

#pragma region functions that gather data from model
    /**
     * @return does this player have science building built
     */
    bool HaveScienceBuilding(void);

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
     * @return how many units of UNIT_TYPE this player has
     */
    int FindUnitCount(UNIT_TYPE unittype);

#pragma endregion

#pragma region members
    // ** Unit AI configuration values ** (these are set from AI config file)
    vector<UNIT*>        m_vUnits;
    vector<BUILDING*>    m_vBuildings;

    int m_CostMod;
    int m_CountMod;
    int m_KillMod;
    float m_fUpdatetime;
#pragma endregion
};

//#endif //_ANTIN_AI