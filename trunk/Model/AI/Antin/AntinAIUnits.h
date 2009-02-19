#pragma once
#include "../../Common/Enums.h"

/**
 * AntinAIUnits
 *
 * Handles combat calculations (sends units to attack enemy targets)
 */
class AntinAIUnits
{
public:
	AntinAIUnits(void);
	virtual ~AntinAIUnits(void);

	/**
	 * Transfer values from config into internal logic
	 */
	void LoadValues(void);

private:
};
