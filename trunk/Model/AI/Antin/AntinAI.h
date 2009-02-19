#pragma once

#ifndef NULL
#define NULL 0 //TODO: this should probably be defined in somewhere common place?
#endif

#include "AntinAIBuilding.h"
#include "AntinAIUnits.h"
#include "../../Common/Config.h"

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
	~AntinAI(void);

private:
	void LoadConfigFromFile(void);

	AntinAIBuilding*	m_pBuild;
	AntinAIUnits*		m_pUnits;
};
