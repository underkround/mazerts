#include "AntinAI.h"

AntinAI::AntinAI()
{
	m_pBuild = NULL;
	m_pUnits = NULL;
	//m_vUnitValues = new vector<UNIT>();
	LoadConfigFromFile();
}

AntinAI::~AntinAI() 
{
}

void AntinAI::Release(void)
{
	unsigned int i;
	for (i=0; i<m_vUnitValues.size(); ++i)
	{
		UNIT* pUnit = m_vUnitValues[i];
		delete pUnit;
	}
	m_vUnitValues.clear();
}



void AntinAI::LoadConfigFromFile(void)
{
	Config & c = * Config::getInstance();
	c.setFilename("AntinAI.ini");
	c.readFile();
	c.setFilename("Buildings.ini");
	c.readFile();
	c.setFilename("Units.ini");
	c.readFile();
	m_pBuild->LoadValues();
	m_pUnits->LoadValues();
	m_pBuild->ChooseUnitToBuild();
}