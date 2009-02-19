#pragma once

/**
 * AntinAIUnits
 *
 */
class AntinAIUnits
{
public:
	AntinAIUnits(void);
	~AntinAIUnits(void);

	void LoadValues(void);

private:
	//Unit AI configuration values (these are set from AI config file)
	int m_CostMod;
	int m_CountMod;
	int m_KillMod;
};
