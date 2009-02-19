#pragma once

#ifndef NULL
#define NULL 0 //TODO: this should probably be defined in somewhere common place?
#endif

#include "AntinAIBuilding.h"
#include "AntinAIUnits.h"
#include "../../Common/Config.h"
#include "../../Common/Enums.h"
#include <string>
#include <vector>

using namespace std;

/**
 * AntinAI
 *
 * k, this is named after me because I'm pretty sure you guys don't agree with my ideas
 * about this issue and want to make your own AI, if nobody does and this works ok
 * then we will refactor it's name
 */
class AntinAI
{
public:

	AntinAI(void);
	virtual ~AntinAI(void);

	void Release(void);

	struct UNIT //values about units AI cares about
	{
		string					strName;
		AntinAI::UNIT_TYPE		eType;
		string					strWeaponName;
		bool					bOffensive; //don't go attacking with weaponless units
		int						cost;
	};
	struct BUILDING
	{
		string					strName;
		AntinAI::BUILDING_TYPE	eType;
		int						cost;
	};

private:
	void LoadConfigFromFile(void);
	inline void AddUnitValuesToVector(UNIT* addme) { m_vUnitValues.push_back(addme); }

	AntinAIBuilding*	m_pBuild;
	AntinAIUnits*		m_pUnits;
	vector<UNIT*>		m_vUnitValues;
};
