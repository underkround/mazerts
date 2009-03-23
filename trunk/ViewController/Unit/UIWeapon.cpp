#include "UIWeapon.h"

#include "UI3DObjectManager.h"
#include "../App/I3DObject.h"

bool UIWeapon::Update(float fFrameTime) 
{
    I3DObject::Update(fFrameTime);
    return m_Alive;
}
