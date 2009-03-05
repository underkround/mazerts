#include "C3DObjectDebug.h"

bool C3DObjectDebug::Update(float fFrameTime)
{
    C3DObject::Update(fFrameTime);

    if(m_SelfDestructing)
    {
        m_LifeTime -= fFrameTime;
        if(m_LifeTime < 0.0f)
        {   
            this->Release();
            delete m_pMesh; //Debug objects use one mesh per object, so this is safe
            delete this; //NOT RECOMMENDED! DEBUG PURPOSES ONLY!
          //If you crashed here, check that you are not using automatic object-variable
            return false;
        }
    }
    return true;
}