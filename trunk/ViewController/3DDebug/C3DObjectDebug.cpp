#include "C3DObjectDebug.h"

bool C3DObjectDebug::Update(float fFrameTime)
{
    C3DObject::Update(fFrameTime);

    if(m_SelfDestructing)
    {
        m_LifeTime -= fFrameTime;
        if(m_LifeTime < 0.0f)
        {   
            return false;
        }
    }
    return true;
}

void C3DObjectDebug::Release()
{
    m_pMesh->Release();    
    C3DObject::Release();    
    m_pMesh = NULL;
}