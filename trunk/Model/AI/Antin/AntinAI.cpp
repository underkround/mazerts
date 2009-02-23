#include "AntinAI.h"

AntinAI::AntinAI()
{
	//m_vUnits = new vector<UNIT*>();
	//m_vBuildings = new vector<BUILDING*>();
	LoadConfigFromFile();
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
	c.setFilename("AntinAI.ini");
	c.readFile();
	c.setFilename("Buildings.ini");
	c.readFile();
	c.setFilename("Units.ini");
	c.readFile();
	//test
	//ChooseUnitToBuild();
}
#pragma endregion

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