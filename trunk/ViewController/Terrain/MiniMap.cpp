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

MiniMap::MiniMap()
{
    m_pTexture = NULL;
    m_pUnitVertices = NULL;
    m_pBackVertices = NULL;

    m_UpdateCounter = 0.0f;

    //Sleeve-defaults
    setSize(300);
    m_Position.x = 10;
    m_Position.y = 10;

    setUpdateInterval(1.0f);
    
}

MiniMap::~MiniMap()
{
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


void MiniMap::create()
{
    m_pBackVertices = new TRANSLITVERTEX[4];

    m_pBackVertices[0].x = m_Position.x;
    m_pBackVertices[0].y = m_Position.y;
    m_pBackVertices[0].z = 0.000001f;
    m_pBackVertices[0].tu = 0.0f;
    m_pBackVertices[0].tv = 1.0f;
    m_pBackVertices[0].dwColor = 0xFFFFFFFF;
    m_pBackVertices[0].rhw = 0.99f;

    m_pBackVertices[1].x = m_Position.x + m_Size;
    m_pBackVertices[1].y = m_Position.y;
    m_pBackVertices[1].z = 0.000001f;
    m_pBackVertices[1].tu = 1.0f;
    m_pBackVertices[1].tv = 1.0f;
    m_pBackVertices[1].dwColor = 0xFFFFFFFF;
    m_pBackVertices[1].rhw = 0.99f;

    m_pBackVertices[2].x = m_Position.x + m_Size;
    m_pBackVertices[2].y = m_Position.y + m_Size;
    m_pBackVertices[2].z = 0.000001f;
    m_pBackVertices[2].tu = 1.0f;
    m_pBackVertices[2].tv = 0.0f;
    m_pBackVertices[2].dwColor = 0xFFFFFFFF;
    m_pBackVertices[2].rhw = 0.99f;

    m_pBackVertices[3].x = m_Position.x;
    m_pBackVertices[3].y = m_Position.y + m_Size;
    m_pBackVertices[3].z = 0.000001f;
    m_pBackVertices[3].tu = 0.0f;
    m_pBackVertices[3].tv = 0.0f;
    m_pBackVertices[3].dwColor = 0xFFFFFFFF;
    m_pBackVertices[3].rhw = 0.99f;

}

void MiniMap::release()
{
    if(m_pBackVertices)
    {
        delete [] m_pBackVertices;
        m_pBackVertices = NULL;
    }

    if(m_pUnitVertices)
    {
        delete [] m_pUnitVertices;
        m_pUnitVertices = NULL;
    }
}

void MiniMap::updateUnits(DoubleLinkedList<UIUnit*>* pUnitList, float deltaTime)
{
    m_UpdateCounter += deltaTime;

    if(m_UpdateCounter > m_UpdateInterval)
    {
        m_UpdateCounter = 0.0f;

        if(m_pUnitVertices)
        {
            delete [] m_pUnitVertices;
        }
        
        m_pUnitVertices = new TRANSLITVERTEX[pUnitList->count() << 2];

        ListNode<UIUnit*>* pNode = pUnitList->headNode();

        int count = 0;

        const float MARKERSIZE = 5.0f;

        while(pNode)
        {        
            //TODO: Visibility check (do not mark units that don't show on radar)
            //if(PIIPPOLANVAARILLAOLITALO)
            {
                D3DXVECTOR2* pos = (D3DXVECTOR2*)pNode->item->getUnit()->getPosition();

                int index = count * 3;
                float posX = m_Position.x + (pos->x * m_SizeFactor);
                float posY = m_Position.y + m_Size - (pos->y * m_SizeFactor);

                m_pUnitVertices[index].x = posX;
                m_pUnitVertices[index].y = posY - MARKERSIZE;
                m_pUnitVertices[index].z = 0.0f;
                m_pUnitVertices[index].tu = 0.5f;
                m_pUnitVertices[index].tv = 0.0f;
                m_pUnitVertices[index].rhw = 0.999f;
                //TODO: Colors from unit owner
                m_pUnitVertices[index++].dwColor = 0xFFFF0000;
                
                m_pUnitVertices[index].x = posX + MARKERSIZE;
                m_pUnitVertices[index].y = posY + MARKERSIZE;
                m_pUnitVertices[index].z = 0.0f;
                m_pUnitVertices[index].tu = 1.0f;
                m_pUnitVertices[index].tv = 1.0f;
                m_pUnitVertices[index].rhw = 0.999f;
                m_pUnitVertices[index++].dwColor = 0xFFFF0000;

                m_pUnitVertices[index].x = posX - MARKERSIZE;
                m_pUnitVertices[index].y = posY + MARKERSIZE;
                m_pUnitVertices[index].z = 0.0f;
                m_pUnitVertices[index].tu = 0.0f;
                m_pUnitVertices[index].tv = 1.0f;
                m_pUnitVertices[index].rhw = 0.999f;
                m_pUnitVertices[index++].dwColor = 0xFFFF0000;

            }
            pNode = pNode->next;
            ++count;
        }

        m_UnitPrimitiveCount = count;
    }
}



void MiniMap::render(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetFVF(TRANSLITVERTEX::GetFVF());    
    pDevice->SetTexture(0, m_pTexture);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pBackVertices, sizeof(TRANSLITVERTEX));

    pDevice->SetTexture(0, NULL);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_UnitPrimitiveCount, m_pUnitVertices, sizeof(TRANSLITVERTEX));
}