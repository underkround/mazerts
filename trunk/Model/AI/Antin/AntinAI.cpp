#include "AntinAI.h"

AntinAI::AntinAI()
{
	m_pBuild = NULL;
	m_pUnits = NULL;
	LoadConfigFromFile();
}

AntinAI::~AntinAI() 
{
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
	//c.printSettings();
}