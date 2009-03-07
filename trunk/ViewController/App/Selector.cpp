#include <tchar.h>

#include "Selector.h"
#include "../App/vertices.h"
#include "../Terrain/UITerrain.h"
#include "../../Model/Terrain/Terrain.h"

Selector::SELECTION* Selector::buttonUp()
{
    //TODO: get units from grid-array


    m_Render = false;
    m_FirstSet = false;

    return NULL;
}

HRESULT Selector::create(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres;

    m_pDevice = pDevice;

    //Vertex buffer
    m_NumVertices = (SELECTOR_SIZE + 1) * (SELECTOR_SIZE + 1);
    hres = pDevice->CreateVertexBuffer(m_NumVertices * sizeof(VERTEX),
                                        0,
                                        VERTEX::GetFVF(),
                                        D3DPOOL_MANAGED,
                                        &m_pVB,
                                        NULL);

    if(FAILED(hres))
    {
        return hres;
    }
    
    //Index buffer
    m_NumPrimitives = (SELECTOR_SIZE) * (SELECTOR_SIZE) * 2;
    hres = pDevice->CreateIndexBuffer(SELECTOR_SIZE * SELECTOR_SIZE * 6 * sizeof(USHORT), 
                                D3DUSAGE_WRITEONLY,
                                D3DFMT_INDEX16,
                                D3DPOOL_MANAGED,
                                &m_pIB,
                                NULL);
    if(FAILED(hres))
    {
        return hres;
    }

    USHORT* pIndices = NULL;
    int vIndex = 0;
    int ind = 0;
    int vertexSize = SELECTOR_SIZE + 1;

    //Fill index-data
    m_pIB->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
    {
        for(int i = 0; i < SELECTOR_SIZE; i++)
        {
            for(int j = 0; j < SELECTOR_SIZE; j++)
            {
                pIndices[ind++] = vIndex;
                pIndices[ind++] = vIndex + vertexSize;
                pIndices[ind++] = vIndex + 1;
                
                pIndices[ind++] = vIndex + 1;  //Second triangle
                pIndices[ind++] = vIndex + vertexSize;
                pIndices[ind++] = vIndex + vertexSize + 1;
                vIndex++;
            }
            vIndex++;
        }
    }
    m_pIB->Unlock();

    //Load texture
    //TODO: get from resource container
    hres = D3DXCreateTextureFromFile(pDevice, _T("Selector.tga"), &m_pTexture);
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}


HRESULT Selector::update()
{
    float width = (m_Point2.x - m_Point1.x) / SELECTOR_SIZE;
    float height = (m_Point2.y - m_Point1.y) / SELECTOR_SIZE;

    m_mWorld._41 = m_Point1.x;
    m_mWorld._42 = m_Point1.y;

    const unsigned char* const* ppVData = Terrain::getInstance()->getTerrainVertexHeightData();

   
    VERTEX* pVertices = NULL;

    int vertexSize = SELECTOR_SIZE + 1;
    int x = 0;
    int y = 0;
    int loc = 0;

    //Fill vertex-data
    m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
    {
        for(int i = 0; i < vertexSize; i++)
        {
            for(int j = 0; j < vertexSize; j++)
            {   
                //Location in 1D-buffer from 2D-values
                loc = i * vertexSize + j;
                
                x = (int)(m_Point1.x + j * width);
                y = (int)(m_Point1.y + i * height);                

                pVertices[loc].x = (float)j * width;
                pVertices[loc].y = (float)i * height;
                pVertices[loc].z = -(ppVData[y][x] * UITerrain::HEIGHTFACTOR) - 1.5f;
                
                pVertices[loc].nx = 0;
                pVertices[loc].ny = 0;
                pVertices[loc].nz = -1.0f;

                //Repeating texture
                pVertices[loc].tu = i * height * 0.5f;
                pVertices[loc].tv = j * width * 0.5f;
            }
        }
    }
    m_pVB->Unlock();

    return S_OK;
}

void Selector::render(LPDIRECT3DDEVICE9 pDevice)
{
    m_pDevice = pDevice;

    if(m_Render && m_Point1.x != m_Point2.x && m_Point1.y != m_Point2.y)
    {

        pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
        
        pDevice->SetMaterial(&m_Mat);

        pDevice->SetFVF( VERTEX::GetFVF() );    

        if(m_pTexture)
        {
            pDevice->SetTexture(0, m_pTexture);
        }
        
        pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX));
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        
        //Render
        if(m_pIB)
        {
            pDevice->SetIndices( m_pIB );
            pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_NumVertices, 0, m_NumPrimitives );
        }

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }
}