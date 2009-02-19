#include "AntinAIBuilding.h"
#include <iostream>
using namespace std;

AntinAIBuilding::AntinAIBuilding(void)
{
}

AntinAIBuilding::~AntinAIBuilding(void)
{
}

/*AntinAIBuilding::Create(AntinAI ai)
{
	m_pAI = ai;
}*/

void AntinAIBuilding::LoadValues(void)
{
}

UNIT_TYPE AntinAIBuilding::ChooseUnitToBuild(void)
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

int CalculateUnitBuildValue(UNIT_TYPE unittype)
{
	int cost = 0;
	return cost;
}

int FindUnitCost(UNIT_TYPE unittype)
{
	switch(unittype)
	{
	case UNIT_TYPE_CAR:
		return 10;
		break;
	case UNIT_TYPE_TANK:
		break;
	case UNIT_TYPE_LAUNCHER:
		break;
	case UNIT_TYPE_SUPER:
		break;
	case UNIT_TYPE_NUKER:
		break;
	}
	return 0;
}

bool AntinAIBuilding::HaveScienceBuilding(void)
{
	//TODO: go and find out
	return false;
}

int AntinAIBuilding::FindUnitCount(UNIT_TYPE unittype)
{
	return 0;
}
