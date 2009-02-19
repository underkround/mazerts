#pragma once
/**
 * AntinAIBuilding
 * my take on AI, this class handles decisions about building structures and units
 *
 */
#include "../../Common/Enums.h"

class AntinAIBuilding
{
public:
	AntinAIBuilding(void);
	virtual ~AntinAIBuilding(void);

//	void Create(AntinAI* ai);

	/**
	 * Transfer values from config into internal logic
	 */
	void LoadValues(void);


	//** functions that perform actions **



	//** functions that do strategic calculations
	/**
	 *  @return UNIT_TYPE that AI should build in current situation
	 */
	UNIT_TYPE ChooseUnitToBuild(void);

	int CalculateUnitBuildValue(UNIT_TYPE unittype);

private:
	//** functions that gather data from model **
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

	// ** Unit AI configuration values ** (these are set from AI config file)
	int m_CostMod;
	int m_CountMod;
	int m_KillMod;

};