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
    for(unsigned int i = 0; i < m_arrMeshData.size(); i++)
    {
        if(m_arrMeshData[i].pMaterial)
        {
            delete m_arrMeshData[i].pMaterial;
            m_arrMeshData[i].pMaterial = NULL;
        }
        if(m_arrMeshData[i].pTexture)
        {
            m_arrMeshData[i].pTexture->Release();
            delete m_arrMeshData[i].pTexture;
            m_arrMeshData[i].pTexture = NULL;
        }

    }

    m_pMesh->Release();
    C3DObject::Release();    
    m_pMesh = NULL;
}