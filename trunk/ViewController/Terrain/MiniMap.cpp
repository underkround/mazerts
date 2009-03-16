/**
 *
 * Changelog:
 *  r143: changed rhw:s from 0.0f to 0.99f and working now with intel GMA 945
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "MiniMap.h"
#include "../../Model/Terrain/Terrain.h"
#include "../Terrain/UITerrain.h"
#include "../Input/MouseState.h"

MiniMap::MiniMap()
{
    m_pUnitVB = NULL;
    m_pBackVB = NULL;
    m_pCameraVB = NULL;

    m_UpdateCounter = 100.0f;

    //Sleeve-defaults
    setSize(300);
    m_Position.x = 10;
    m_Position.y = 10;

    setUpdateInterval(1.0f);
    
}

MiniMap::~MiniMap()
{
    release();
}

void MiniMap::setPosition(const int x, const int y)
{
    m_Position.x = (float)x;
    m_Position.y = (float)y;
}

void MiniMap::setSize(const int size)
{
    m_Size = (float)size;
    m_SizeFactor = m_Size / Terrain::getInstance()->getSize();
}


HRESULT MiniMap::create(LPDIRECT3DDEVICE9 pDevice)
{
    //Background vertexbuffer
    HRESULT hres = pDevice->CreateVertexBuffer(4 * sizeof(TRANSLITVERTEX), NULL, TRANSLITVERTEX::GetFVF(), D3DPOOL_MANAGED, &m_pBackVB, NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    //Background vertices
    TRANSLITVERTEX* pBackVertices = NULL;

    m_pBackVB->Lock(0, 0, (void**)&pBackVertices, D3DLOCK_DISCARD);
    {
        pBackVertices[0].x = m_Position.x;
        pBackVertices[0].y = m_Position.y;
        pBackVertices[0].z = 0.000001f;
        pBackVertices[0].tu = 0.0f;
        pBackVertices[0].tv = 1.0f;
        pBackVertices[0].dwColor = 0xFFFFFFFF;
        pBackVertices[0].rhw = 0.99f;

        pBackVertices[1].x = m_Position.x + m_Size;
        pBackVertices[1].y = m_Position.y;
        pBackVertices[1].z = 0.000001f;
        pBackVertices[1].tu = 1.0f;
        pBackVertices[1].tv = 1.0f;
        pBackVertices[1].dwColor = 0xFFFFFFFF;
        pBackVertices[1].rhw = 0.99f;

        pBackVertices[2].x = m_Position.x + m_Size;
        pBackVertices[2].y = m_Position.y + m_Size;
        pBackVertices[2].z = 0.000001f;
        pBackVertices[2].tu = 1.0f;
        pBackVertices[2].tv = 0.0f;
        pBackVertices[2].dwColor = 0xFFFFFFFF;
        pBackVertices[2].rhw = 0.99f;

        pBackVertices[3].x = m_Position.x;
        pBackVertices[3].y = m_Position.y + m_Size;
        pBackVertices[3].z = 0.000001f;
        pBackVertices[3].tu = 0.0f;
        pBackVertices[3].tv = 0.0f;
        pBackVertices[3].dwColor = 0xFFFFFFFF;
        pBackVertices[3].rhw = 0.99f;
    }
    m_pBackVB->Unlock();

    //Unit-marker vertexbuffer
    hres = pDevice->CreateVertexBuffer(MINIMAP_MAX_UNITS * 3 * sizeof(TRANSLITVERTEX), NULL, TRANSLITVERTEX::GetFVF(), D3DPOOL_MANAGED, &m_pUnitVB, NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    //Camera vertexbuffer
    hres = pDevice->CreateVertexBuffer(4 * sizeof(TRANSLITVERTEX), NULL, TRANSLITVERTEX::GetFVF(), D3DPOOL_MANAGED, &m_pCameraVB, NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}

void MiniMap::release()
{
    if(m_pBackVB)
    {
        m_pBackVB->Release();
        m_pBackVB = NULL;
    }

    if(m_pUnitVB)
    {
        m_pUnitVB->Release();        
        m_pUnitVB = NULL;
    }

    if(m_pCameraVB)
    {
        m_pCameraVB->Release();
        m_pCameraVB = NULL;
    }
}


void MiniMap::updateCamera(LPDIRECT3DDEVICE9 pDevice)
{
    const static float CLIPZ = -0.0f * UITerrain::HEIGHTFACTOR;

    D3DXMATRIX matProj;
    pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

    D3DXMATRIX matView;
    pDevice->GetTransform(D3DTS_VIEW, &matView);    

    D3DXVECTOR3 rayOrigin,rayDir;    

    D3DVIEWPORT9 viewPort;

    pDevice->GetViewport(&viewPort);

    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir, 0, 0);    
    D3DXVECTOR3 upperLeft = intersectRay(rayOrigin, rayDir, CLIPZ);

    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir, viewPort.Width, 0);
    D3DXVECTOR3 upperRight = intersectRay(rayOrigin, rayDir, CLIPZ);

    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir,0, viewPort.Height);
    D3DXVECTOR3 lowerLeft = intersectRay(rayOrigin, rayDir, CLIPZ);

    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir,viewPort.Width, viewPort.Height);
    D3DXVECTOR3 lowerRight = intersectRay(rayOrigin, rayDir, CLIPZ);

    TRANSLITVERTEX* pVertices = NULL;

    m_pCameraVB->Lock(0, 4 * sizeof(TRANSLITVERTEX), (void**)&pVertices, D3DLOCK_DISCARD);
    {
        float posX = m_Position.x + (lowerLeft.x * m_SizeFactor);
        float posY = m_Position.y + m_Size - (lowerLeft.y * m_SizeFactor);

        pVertices[0].x = posX;
        pVertices[0].y = posY;
        pVertices[0].z = 0.0f;
        pVertices[0].tu = 0.0f;
        pVertices[0].tv = 1.0f;
        pVertices[0].dwColor = 0xAAFFFFFF;
        pVertices[0].rhw = 0.99f;

        if(upperLeft)
        {
            posX = m_Position.x + (upperLeft.x * m_SizeFactor);
            posY = m_Position.y + m_Size - (upperLeft.y * m_SizeFactor);

            /*if(posX < m_Position.x)
            {
                posX = m_Position.x;
            }
            else if(posX > m_Position.x + m_Size)
            {
                posX = m_Position.x + m_Size;
            }

            if(posY < m_Position.y)
            {
                posY = m_Position.y;
            }
            else if(posY > m_Position.y + m_Size)
            {
                posY = m_Position.y + m_Size;
            }*/
        }

        pVertices[1].x = posX;
        pVertices[1].y = posY;
        pVertices[1].z = 0.0f;
        pVertices[1].tu = 0.0f;
        pVertices[1].tv = 1.0f;
        pVertices[1].dwColor = 0xAAFFFFFF;
        pVertices[1].rhw = 0.99f;

        if(upperRight)
        {
            posX = m_Position.x + (upperRight.x * m_SizeFactor);
            posY = m_Position.y + m_Size - (upperRight.y * m_SizeFactor);

            /*if(posX < m_Position.x)
            {
                posX = m_Position.x;
            }
            else if(posX > m_Position.x + m_Size)
            {
                posX = m_Position.x + m_Size;
            }

            if(posY < m_Position.y)
            {
                posY = m_Position.y;
            }
            else if(posY > m_Position.y + m_Size)
            {
                posY = m_Position.y + m_Size;
            }*/
        }

        pVertices[2].x = posX;
        pVertices[2].y = posY;
        pVertices[2].z = 0.0f;
        pVertices[2].tu = 0.0f;
        pVertices[2].tv = 1.0f;
        pVertices[2].dwColor = 0xAAFFFFFF;
        pVertices[2].rhw = 0.99f;

        posX = m_Position.x + (lowerRight.x * m_SizeFactor);
        posY = m_Position.y + m_Size - (lowerRight.y * m_SizeFactor);

        pVertices[3].x = posX;
        pVertices[3].y = posY;
        pVertices[3].z = 0.0f;
        pVertices[3].tu = 0.0f;
        pVertices[3].tv = 1.0f;
        pVertices[3].dwColor = 0xAAFFFFFF;
        pVertices[3].rhw = 0.99f;
    }
    m_pCameraVB->Unlock();

}


void MiniMap::updateUnits(DoubleLinkedList<UIUnit*>* pUnitList, float deltaTime)
{
    m_UpdateCounter += deltaTime;

    if(m_UpdateCounter > m_UpdateInterval)
    {
        m_UpdateCounter = 0.0f;

        m_UnitPrimitiveCount = pUnitList->count();
        
        TRANSLITVERTEX* pUnitVertices;

        ListNode<UIUnit*>* pNode = pUnitList->headNode();

        //int count = 0;
        const float MARKERSIZE = 3.0f;
        int index = 0;

        m_pUnitVB->Lock(0, m_UnitPrimitiveCount * 3 * sizeof(TRANSLITVERTEX), (void**)&pUnitVertices, D3DLOCK_DISCARD);

        while(pNode)
        {        
            //TODO: Visibility check (do not mark units that don't show on radar)
            //if(PIIPPOLANVAARILLAOLITALO)
            {
                D3DXVECTOR2* pos = (D3DXVECTOR2*)pNode->item->getUnit()->getPosition();

                //int index = count * 3;
                float posX = m_Position.x + (pos->x * m_SizeFactor);
                float posY = m_Position.y + m_Size - (pos->y * m_SizeFactor);

                pUnitVertices[index].x = posX;
                pUnitVertices[index].y = posY;
                pUnitVertices[index].z = 0.0f;
                pUnitVertices[index].tu = 0.5f;
                pUnitVertices[index].tv = 0.0f;
                pUnitVertices[index].rhw = 0.999f;
                //TODO: Colors from unit owner
                pUnitVertices[index++].dwColor = 0xFFFF0000;
                
                pUnitVertices[index].x = posX - MARKERSIZE;
                pUnitVertices[index].y = posY - (MARKERSIZE * 2.0f);
                pUnitVertices[index].z = 0.0f;
                pUnitVertices[index].tu = 1.0f;
                pUnitVertices[index].tv = 1.0f;
                pUnitVertices[index].rhw = 0.999f;
                pUnitVertices[index++].dwColor = 0xFFFF0000;

                pUnitVertices[index].x = posX + MARKERSIZE;
                pUnitVertices[index].y = posY - (MARKERSIZE * 2.0f);
                pUnitVertices[index].z = 0.0f;
                pUnitVertices[index].tu = 0.0f;
                pUnitVertices[index].tv = 1.0f;
                pUnitVertices[index].rhw = 0.999f;
                pUnitVertices[index++].dwColor = 0xFFFF0000;

            }
            pNode = pNode->next;
        }

        m_pUnitVB->Unlock();
    }
}



void MiniMap::render(LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 pTexture)
{

    pDevice->SetTexture(0, pTexture);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    
    pDevice->SetFVF(TRANSLITVERTEX::GetFVF());        
    pDevice->SetStreamSource(0, m_pBackVB, 0, sizeof(TRANSLITVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    pDevice->SetTexture(0, NULL);

    pDevice->SetStreamSource(0, m_pUnitVB, 0, sizeof(TRANSLITVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_UnitPrimitiveCount);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    pDevice->SetStreamSource(0, m_pCameraVB, 0, sizeof(TRANSLITVERTEX));
    pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

D3DXVECTOR3 MiniMap::intersectRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir, float clipZ)
{    
    D3DXVECTOR3 result;

    float t = (rayOrigin.z - clipZ) / -rayDir.z;
    result.z = clipZ;
    result.x = rayOrigin.x + rayDir.x * t;
    result.y = rayOrigin.y + rayDir.y * t;

    return result;
}