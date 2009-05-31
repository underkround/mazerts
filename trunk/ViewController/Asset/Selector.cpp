#include <tchar.h>

#include "Selector.h"
#include "../App/vertices.h"
#include "../Terrain/UITerrain.h"
#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Asset/Building.h"

Selector::SELECTION* Selector::buttonUp()
{
    SELECTION* result = new SELECTION;

    if(m_SelectorState == SELECTOR_NORMAL)
    {        
        int x, y;
        int width, height;

        //Get the points
        if(m_Point1.x > m_Point2.x)
        {        
            x = (int)m_Point2.x;
            width = (int)m_Point1.x - (int)m_Point2.x;
        }
        else
        {        
            x = (int)m_Point1.x;
            width = (int)m_Point2.x - (int)m_Point1.x;
        }

        if(m_Point1.y > m_Point2.y)
        {        
            y = (int)m_Point2.y;
            height = (int)m_Point1.y - (int)m_Point2.y;
        }
        else
        {        
            y = (int)m_Point1.y;
            height = (int)m_Point2.y - (int)m_Point1.y;
        }    
       
        //Model-side units
        DoubleLinkedList<IAsset*>* modelAssetList = new DoubleLinkedList<IAsset*>;
        AssetCollection::getAssetsAt(modelAssetList, x, y, width, height);

        //UI-side units
        DoubleLinkedList<UIAsset*>* assetList = UI3DObjectManager::getInstance()->getAssetList();

        ListNode<UIAsset*>* pNode = assetList->headNode();

        while(pNode)
        {
            ListNode<IAsset*>* modelNode = modelAssetList->headNode();
            while(modelNode)
            {
                if(modelNode->item == pNode->item->getAsset())
                {
                    result->assets.pushTail(pNode->item);
                }

                modelNode = modelNode->next;
            }
            
            pNode = pNode->next;
        }
        delete modelAssetList;
    }

    m_Render = false;
    m_FirstSet = false;

    return result;
}

HRESULT Selector::create(LPDIRECT3DDEVICE9 pDevice, Player* pPlayer)
{
    HRESULT hres;

    m_pDevice = pDevice;
    m_pPlayer = pPlayer;

    //Vertex buffer
    m_NumVertices = (SELECTOR_SIZE + 1) * (SELECTOR_SIZE + 1);
    hres = pDevice->CreateVertexBuffer(m_NumVertices * sizeof(LITVERTEX),
                                        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                        LITVERTEX::GetFVF(),
                                        D3DPOOL_DEFAULT,
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
    hres = D3DXCreateTextureFromFile(pDevice, _T("../data/textures/Selector.tga"), &m_pTexture);
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}


HRESULT Selector::update()
{
    float width = 0;
    float height = 0;
    D3DXCOLOR baseColor(0.7f, 0.7f, 0.7f, 1.0f);

    //Width/height-steps
    if(m_SelectorState == SELECTOR_NORMAL)
    {
        width = (m_Point2.x - m_Point1.x) / SELECTOR_SIZE;
        height = (m_Point2.y - m_Point1.y) / SELECTOR_SIZE;
        m_mWorld._41 = m_Point1.x;
        m_mWorld._42 = m_Point1.y;
    }
    else if(m_SelectorState == SELECTOR_BUILDINGPLACEMENT)
    {
        width = m_BuildingPlacementSize.x / SELECTOR_SIZE;
        height = m_BuildingPlacementSize.y / SELECTOR_SIZE;
        m_mWorld._41 = m_Point1.x - m_BuildingPlacementSize.x * 0.5f;
        m_mWorld._42 = m_Point1.y - m_BuildingPlacementSize.y * 0.5f;

        if(isBuildable())
        {
            baseColor.r = 0.1f;
            baseColor.g = 0.8f;
            baseColor.b = 0.2f;
            baseColor.a = 1.0f;
        }
        else
        {
            baseColor.r = 0.6f;
            baseColor.g = 0.1f;
            baseColor.b = 0.1f;
            baseColor.a = 1.0f;
        }
    }
    else
    {
        //"Should never happen" ;)
        //Fallback for unknown state
        width = 100.0f;
        height = 100.0f;
        m_mWorld._41 = m_Point1.x;
        m_mWorld._42 = m_Point1.y;
    }


    const unsigned char* const* ppVData = Terrain::getInstance()->getTerrainVertexHeightData();
    const unsigned short size = Terrain::getInstance()->getSize();

   
    LITVERTEX* pVertices = NULL;

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
                
                x = (int)(m_mWorld._41 + j * width);
                y = (int)(m_mWorld._42 + i * height);                

                pVertices[loc].x = (float)j * width;
                pVertices[loc].y = (float)i * height;                
                
                if(y > -1 && y < size && x > -1 && x < size)
                {
                    pVertices[loc].z = -(ppVData[y][x] * UITerrain::HEIGHTFACTOR) - 1.5f;
                }
                else
                {
                    pVertices[loc].z = 0;
                }
                
                if(m_SelectorState == SELECTOR_NORMAL)
                {
                    pVertices[loc].dwColor = baseColor;
                }
                else if(m_SelectorState == SELECTOR_BUILDINGPLACEMENT)
                {
                    //Check if the ground is passable
                    if(AssetCollection::getAssetsAt(NULL, x, y, 2, 2) != 0)
                    //if(AssetCollection::getAssetAt(x, y) != NULL)
                    {
                        pVertices[loc].dwColor = D3DXCOLOR(1.0f, 0.15f, 0.15f, 1.0);
                    }
                    else
                    {
                        pVertices[loc].dwColor = baseColor;
                    }
                }

                //Repeating texture
                pVertices[loc].tu = i * height; //* 0.5f;
                pVertices[loc].tv = j * width; //* 0.5f;
            }
        }
    }
    m_pVB->Unlock();

    return S_OK;
}

bool Selector::isBuildable()
{
    if(m_SelectorState != SELECTOR_BUILDINGPLACEMENT)
    {
        return false;
    }
   
    int cornerX = (int)(m_Point1.x - m_BuildingPlacementSize.x * 0.5f);
    int cornerY = (int)(m_Point1.y - m_BuildingPlacementSize.y * 0.5f);

    const unsigned char* const* ppVData = Terrain::getInstance()->getTerrainVertexHeightData();
    const unsigned short size = Terrain::getInstance()->getSize();

    if(cornerX > -1 && (cornerX + m_BuildingPlacementSize.x) < size &&
        cornerY > -1 && (cornerY + m_BuildingPlacementSize.y) < size)
    {
        //Check units/buildings
        if(AssetCollection::getAssetsAt(NULL, cornerX, cornerY, (unsigned short)m_BuildingPlacementSize.x, (unsigned short)m_BuildingPlacementSize.y) != 0)
        {
            return false;
        }

        //Check terrain by getting lowest and highest point from the area, and checking if the difference is too great
        unsigned char low = 255;
        unsigned char high = 0;
        unsigned char waterlevel = Terrain::getInstance()->getWaterLevel();
        for(int i = cornerY; i < (cornerY + m_BuildingPlacementSize.y); ++i)
        {
            for(int j = cornerX; j < (cornerX + m_BuildingPlacementSize.x); ++j)
            {
                //Check waterlevel
                if(ppVData[i][j] < waterlevel)
                {
                    return false;
                }

                //Store heights
                if(ppVData[i][j] < low)
                {
                    low = ppVData[i][j];
                }
                if(ppVData[i][j] > high)
                {
                    high = ppVData[i][j];
                }
            }
        }

        //Heightdifference check
        if(high - low > 10)
        {
            return false;
        }

        ListNode<Building*>* pNode = AssetCollection::getAllBuildings()->headNode();
        bool closeBuildings = false;
        while (pNode)
        {
            if (pNode->item->getOwner() == m_pPlayer)
            {
                const unsigned short x = pNode->item->getCenterGridX();
                const unsigned short y = pNode->item->getCenterGridY();
                const float x2 = (m_Point1.x - x) * (m_Point1.x - x);
                const float y2 = (m_Point1.y - y) * (m_Point1.y - y);
                const float dist = sqrtf(x2 + y2);
                if (dist < pNode->item->getDef()->pDefRadar->range / 2)
                {
                    closeBuildings = true;
                    break;
                }
            }
            pNode = pNode->next;
        }
        if (!closeBuildings) return false;

        //All ok
        return true;
    }

    return false;
}


void Selector::render(LPDIRECT3DDEVICE9 pDevice)
{
    m_pDevice = pDevice;

    if(m_Render)// && m_Point1.x != m_Point2.x && m_Point1.y != m_Point2.y)
    {
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        pDevice->SetTransform(D3DTS_WORLD, &m_mWorld);
        
        pDevice->SetMaterial(&m_Mat);

        pDevice->SetFVF( LITVERTEX::GetFVF() );    

        if(m_pTexture)
        {
            pDevice->SetTexture(0, m_pTexture);
        }
        
        pDevice->SetStreamSource(0, m_pVB, 0, sizeof(LITVERTEX));
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        
        //Render
        if(m_pIB)
        {
            pDevice->SetIndices( m_pIB );
            pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_NumVertices, 0, m_NumPrimitives );
        }

        if(m_pTexture)
        {
            pDevice->SetTexture(0, NULL);
            pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    }
}

HRESULT Selector::onDeviceLost()
{
    if(m_pVB)
    {
        HRESULT hres = m_pVB->Release();
        if(FAILED(hres))
        {
            return hres;
        }

        m_pVB = NULL;
    }

    return S_OK;
}

HRESULT Selector::onRestore(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres;

    //Vertex buffer
    hres = pDevice->CreateVertexBuffer(m_NumVertices * sizeof(LITVERTEX),
                                        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                        LITVERTEX::GetFVF(),
                                        D3DPOOL_DEFAULT,
                                        &m_pVB,
                                        NULL);
    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}