#pragma once
#include "..\iai.h"

class CombatAI :
    public IAI
{
public:
    CombatAI(void);
    virtual ~CombatAI(void);

    void Update(float fFrameTime);

    void Release(void);
};
