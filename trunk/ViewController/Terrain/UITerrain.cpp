#include <tchar.h>

#include "UITerrain.h"
#include "../../Model/Terrain/Terrain.h"
#include "../App/vertices.h"
#include "../Camera/FrustumCull.h"

const float UITerrain::HEIGHTFACTOR = 0.15f;


UITerrain::UITerrain()
{
    //For single index- & vertexbuffer rendering
    m_pIB = NULL;
    m_pVB = NULL;
    
    //For multibuffer rendering (even using single index-buffer with multiple vertexbuffers didn't work on laptop)
    m_pppIB = NULL;
    m_pppVB = NULL;

    m_pTexture = NULL;
    m_pPixelTexture = NULL;
    m_Patches = 0;
    m_NumPrimitives = 0;
    m_NumVertices = 0;
    m_DetailLevel = 0;
    m_ChangeCounter = 0;

    //Place at 0, 0, 0
    D3DXMatrixIdentity(&m_World);

    m_FillMode = D3DFILL_SOLID;
    
    m_TextureRepeat = 24.0f;
    
    ::memset(&m_Mat, 0, sizeof(D3DMATERIAL9));
    m_Mat.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
 
}

UITerrain::~UITerrain(void)
{
    release();
}

void UITerrain::release()
{
    //Release minimap
    m_MiniMap.release();

    //Triangle normal releases
    if(m_pppTriangleNormals)
    {
        for(int i = 0; i < m_Size; i++)
        {
            for(int j = 0; j < m_Size; j++)
            {            
                delete [] m_pppTriangleNormals[i][j];
                m_pppTriangleNormals[i][j] = NULL;
            }
            delete [] m_pppTriangleNormals[i];
            m_pppTriangleNormals[i] = NULL;
        }
        delete [] m_pppTriangleNormals;
        m_pppTriangleNormals = NULL;
    }

    //AABB releases
    if(m_pppPatchAABBs)
    {
        for(int i = 0; i < m_Patches; i++)
        {
            for(int j = 0; j < m_Patches; j++)
            {            
                delete [] m_pppPatchAABBs[i][j];
                m_pppPatchAABBs[i][j] = NULL;
            }
            delete [] m_pppPatchAABBs[i];
            m_pppPatchAABBs[i] = NULL;
        }
        delete [] m_pppPatchAABBs;
        m_pppPatchAABBs = NULL;
    }

    //Texture releases
    if (m_pTexture)
    {
        m_pTexture->Release();        
        m_pTexture = NULL;
    }

    if(m_pPixelTexture)
    {
        m_pPixelTexture->Release();
        m_pPixelTexture = NULL;
    }

    //Single vertex- and indexbuffer releases
    if(m_pIB)
    {
        m_pIB->Release();                
        m_pIB = NULL;
    }

    if (m_pVB)
    {
        m_pVB->Release();
        m_pVB = NULL;
    }

    //Multibuffer releases
    //Patch vertex- and indexbuffer releases
    if(m_pppVB)
    {
        for(int i = 0; i < m_Patches; i++)
        {
            for(int j = 0; j < m_Patches; j++)
            {
                if (m_pppVB[i][j])
                {
                    m_pppVB[i][j]->Release();
                    m_pppVB[i][j] = NULL;
                }
            }
            delete [] m_pppVB[i];
            m_pppVB[i] = NULL;
        }
        delete [] m_pppVB;
        m_pppVB = NULL;
    }
    if(m_pppIB)
    {
        for(int i = 0; i < m_Patches; i++)
        {
            for(int j = 0; j < m_Patches; j++)
            {
                if(m_pppIB[i][j])
                {
                    m_pppIB[i][j]->Release();                
                    m_pppIB[i][j] = NULL;
                }
            }
            delete [] m_pppIB[i];
            m_pppIB[i] = NULL;
        }        
        delete [] m_pppIB;
        m_pppIB = NULL;
    }

    m_ChangeCounter++;
}

void UITerrain::render(LPDIRECT3DDEVICE9 pDevice)
{

    pDevice->SetTransform(D3DTS_WORLD, &m_World);
    
    pDevice->SetFVF( VERTEX2UV::GetFVF() );
    pDevice->SetRenderState(D3DRS_FILLMODE, m_FillMode);

    if(m_pTexture)
    {
        // set the texturing parameters
        pDevice->SetTexture(0, m_pTexture);
        pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    }

    if(m_pPixelTexture)
    {
        //Color map on second coordinates
        pDevice->SetTexture(1, m_pPixelTexture);
        pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
        pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
    }

    pDevice->SetMaterial(&m_Mat);

    if(!m_MultiBufferRender)
    {
        //Render using single index- and vertexbuffer
        pDevice->SetIndices(m_pIB);
        pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX2UV));

        for(int i = 0; i < m_Patches; i++)
        {
            for(int j = 0; j < m_Patches; j++)
            {
                //Cull against frustum
                if(FrustumCull::cullAABB(m_pppPatchAABBs[i][j][0], m_pppPatchAABBs[i][j][1]))
                {
                    //Move BaseVertexIndex on each patch
                    pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, (i * (((PATCHSIZE+1) * (PATCHSIZE+1)) * m_Patches)) + (j * ((PATCHSIZE+1) * (PATCHSIZE+1))), 0, m_NumVertices, 0, m_NumPrimitives );
                }
            }
        }
    }
    else
    {
        //Render using multiple index- and vertexbuffers
        //Needed for old videocards & laptops  (MaxVertexIndex = 65565)
        for(int i = 0; i < m_Patches; i++)
        {
            for(int j = 0; j < m_Patches; j++)
            {
                //Cull against frustum
                if(FrustumCull::cullAABB(m_pppPatchAABBs[i][j][0], m_pppPatchAABBs[i][j][1]))
                {
                    pDevice->SetStreamSource( 0, m_pppVB[i][j], 0, sizeof(VERTEX2UV) );        
            
                    if(m_pppIB[i][j])
                    {
                        //Using single indexbuffer didn't work on laptop? (the contents are same for all indexbuffers)
                        pDevice->SetIndices( m_pppIB[i][j] );
                        pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_NumVertices, 0, m_NumPrimitives );
                    }
                }
            }
        }
    }

    if(m_pPixelTexture)
    {
        pDevice->SetTexture(1, NULL);
    }

    m_MiniMap.render(pDevice, m_pPixelTexture);
}

HRESULT UITerrain::create(LPDIRECT3DDEVICE9 pDevice, Player* pCurrentPlayer)
{
    UITerrain* pInstance = getInstance();

    //Release old instance, if exists
    if(pInstance)
    {
        pInstance->release();
    }    

    pInstance->m_pCurrentPlayer = pCurrentPlayer;

    m_ChangeCounter++;

    //Caps-check for old/cheapo/laptop videocards (16-bit vertex-buffer indices), needs lots of buffers
    D3DCAPS9 caps;
    pDevice->GetDeviceCaps(&caps);

    //Number of patches for vertex-index calculations
    unsigned int patches = Terrain::getInstance()->getSize() / PATCHSIZE;

    //MaxVertexIndex limit calculated based on terrain- and patch size 
    //(the single-buffer method still uses 16bit index offsets, so there's some overhead)
    if(caps.MaxVertexIndex < (DWORD)((PATCHSIZE+1) * patches * (PATCHSIZE+1) * patches))
    {
        pInstance->m_MultiBufferRender = true;
    }
    else
    {
        pInstance->m_MultiBufferRender = false;
    }    

    HRESULT hres;

    if(pInstance->m_MultiBufferRender)
    {
        hres = pInstance->initializeMulti(pDevice);
    }
    else
    {
        hres = pInstance->initialize(pDevice);
    }

    if(FAILED(hres))
    {
        return hres;
    }

    hres = D3DXCreateTextureFromFile(pDevice, _T("../data/textures/grass01.png"), &pInstance->m_pTexture);
    if(FAILED(hres))
    {
        return hres;
    }

    //Create colormap
    bool** fog = NULL;
    if (m_pCurrentPlayer)
        fog = m_pCurrentPlayer->getFog()->getFogArray();
    hres = pInstance->createColorMapTexture(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }
    hres = pInstance->updateColorMapTexture(fog);
    if(FAILED(hres))
    {
        return hres;
    }

    //Create minimap
    hres = pInstance->m_MiniMap.create(pDevice);

    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}

HRESULT UITerrain::initialize(LPDIRECT3DDEVICE9 pDevice)
{
    Terrain* pTerrain = Terrain::getInstance();    
    m_Size = pTerrain->getSize();

    //Calculate amount of patches per side (total patches = patches * patches)
    m_Patches = m_Size / PATCHSIZE;
    
    //One extra row and column of vertices per patch
    m_NumVertices = (PATCHSIZE+1)*m_Patches * (PATCHSIZE+1)*m_Patches;

    //Create triangle normals-array
    m_pppTriangleNormals = new D3DXVECTOR3**[m_Size];
    for(int i = 0; i < m_Size; i++)
    {
        m_pppTriangleNormals[i] = new D3DXVECTOR3*[m_Size];

        for(int j = 0; j < m_Size; j++)
        {
            m_pppTriangleNormals[i][j] = new D3DXVECTOR3[2];
        }
    }

    calculateTriangleNormals();


    //Create AABBs-array
    m_pppPatchAABBs = new D3DXVECTOR3**[m_Patches];
    for(int i = 0; i < m_Patches; i++)
    {
        m_pppPatchAABBs[i] = new D3DXVECTOR3*[m_Patches];

        for(int j = 0; j < m_Patches; j++)
        {
            m_pppPatchAABBs[i][j] = new D3DXVECTOR3[2];
        }
    }
    

    //Vertex and indexbuffers for patches
    HRESULT hres;


    //Total amount of indices, 6 per tile
    int indAmount = PATCHSIZE * PATCHSIZE * 6;
    
    //Create buffer
    hres = pDevice->CreateIndexBuffer(indAmount * sizeof(USHORT),
        D3DUSAGE_WRITEONLY,         //Index- and vertexbuffers are modified, but arent marked as dynamical because modifications occur so rarely
        D3DFMT_INDEX16,             //16-bit index offsets for legacy support
        D3DPOOL_MANAGED,
        &m_pIB,
        NULL);

    if(FAILED(hres))
    {
        return hres;
    }

    hres = pDevice->CreateVertexBuffer(m_NumVertices * sizeof(VERTEX2UV),
                                D3DUSAGE_WRITEONLY,
                                VERTEX2UV::GetFVF(),
                                D3DPOOL_MANAGED,
                                &m_pVB,
                                NULL);

    if(FAILED(hres))
    {
        return hres;
    }


    //Fill vertex-data
    VERTEX2UV* pVertices = NULL;

    int loc = 0;

    m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
    {
        for(int y = 0; y < m_Patches; y++)
        {        
            for(int x = 0; x < m_Patches; x++)
            {
                //X- and Y of the AABBs are already known
                m_pppPatchAABBs[y][x][0].x = (float)(x * PATCHSIZE);
                m_pppPatchAABBs[y][x][1].x = (float)((x + 1) * PATCHSIZE);
                m_pppPatchAABBs[y][x][0].y = (float)((y) * PATCHSIZE);
                m_pppPatchAABBs[y][x][1].y = (float)((y + 1) * PATCHSIZE);

                //Min and max height within patch, used to set the AABB-size
                //Initialize to maximum and minimum that the heights can be
                float minHeight = (-255.0f * HEIGHTFACTOR);
                float maxHeight = 0.0f;

                //+1 because patches need to duplicate the vertices at the line between two patches 
                //(could share a vertex, but the index buffers would become a pain to calculate)
                for(int i = 0; i < PATCHSIZE+1; i++)
                {
                    for(int j = 0; j < PATCHSIZE+1; j++)
                    {                                                                        
                        //Offsets in x- and y-axis for vertex location
                        int offsetX = j + (x * PATCHSIZE);
                        int offsetY = i + (y * PATCHSIZE);

                        pVertices[loc].x = (float)offsetX;
                        pVertices[loc].y = (float)offsetY;
                        pVertices[loc].z = -calculateAverageHeightForVertex(offsetX, offsetY);
                        
                        //Check for AABBs
                        if(pVertices[loc].z < minHeight)
                        {
                            minHeight = pVertices[loc].z;
                        }
                        if(pVertices[loc].z > maxHeight)
                        {
                            maxHeight = pVertices[loc].z;
                        }

                        //Calculate normals
                        D3DXVECTOR3 normal = getNormalAt((float)offsetX, (float)offsetY, 1);

                        pVertices[loc].nx = normal.x;
                        pVertices[loc].ny = normal.y;
                        pVertices[loc].nz = normal.z;

                        //Repeating texture
                        pVertices[loc].tu = (m_TextureRepeat / m_Size) * j;
                        pVertices[loc].tv = (m_TextureRepeat / m_Size) * i;

                        //Colormap-texture
                        pVertices[loc].tu2 = (1.0f / m_Size) * offsetX;
                        pVertices[loc].tv2 = (1.0f / m_Size) * offsetY;

                        ++loc;
                    }
                }
            
                //Set AABBs z-values
                m_pppPatchAABBs[y][x][0].z = minHeight;
                m_pppPatchAABBs[y][x][1].z = maxHeight;
            }
        }
    }
    m_pVB->Unlock();



    //Setting detail level creates the contents of the indexbuffers and adjusts the normals for vertexbuffers
    char value = m_DetailLevel;
    m_DetailLevel = 255;    //Set m_DetailLevel to other value, so the data is really created
    //setDetailLevel won't do anything if parameter == m_DetailLevel (this is to prevent unnecessary recalculations)
    setDetailLevel(value);

    return S_OK;
}

HRESULT UITerrain::initializeMulti(LPDIRECT3DDEVICE9 pDevice)
{
    Terrain* pTerrain = Terrain::getInstance();    
    m_Size = pTerrain->getSize();

    //Calculate amount of patches per side (total patches = patches * patches)
    m_Patches = m_Size / PATCHSIZE;
    
    unsigned short vertexSize = PATCHSIZE + 1;
    m_NumVertices = vertexSize * vertexSize;

    //Create triangle normals-array
    m_pppTriangleNormals = new D3DXVECTOR3**[m_Size];
    for(int i = 0; i < m_Size; i++)
    {
        m_pppTriangleNormals[i] = new D3DXVECTOR3*[m_Size];

        for(int j = 0; j < m_Size; j++)
        {
            m_pppTriangleNormals[i][j] = new D3DXVECTOR3[2];
        }
    }

    //Calculate trianglenormals
    calculateTriangleNormals();


    //Create AABBs-array
    m_pppPatchAABBs = new D3DXVECTOR3**[m_Patches];
    for(int i = 0; i < m_Patches; i++)
    {
        m_pppPatchAABBs[i] = new D3DXVECTOR3*[m_Patches];

        for(int j = 0; j < m_Patches; j++)
        {
            m_pppPatchAABBs[i][j] = new D3DXVECTOR3[2];
        }
    }
    

    //Vertex and indexbuffers for patches
    HRESULT hres;

    m_pppVB = new LPDIRECT3DVERTEXBUFFER9*[m_Patches];
    m_pppIB = new LPDIRECT3DINDEXBUFFER9*[m_Patches];

    for(int y = 0; y < m_Patches; y++)
    {        
        m_pppVB[y] = new LPDIRECT3DVERTEXBUFFER9[m_Patches];
        m_pppIB[y] = new LPDIRECT3DINDEXBUFFER9[m_Patches];

        for(int x = 0; x < m_Patches; x++)
        {
            //X- and Y of the AABBs are already known
            m_pppPatchAABBs[y][x][0].x = (float)(x * PATCHSIZE);
            m_pppPatchAABBs[y][x][1].x = (float)((x + 1) * PATCHSIZE);
            m_pppPatchAABBs[y][x][0].y = (float)((y) * PATCHSIZE);
            m_pppPatchAABBs[y][x][1].y = (float)((y + 1) * PATCHSIZE);


            hres = pDevice->CreateVertexBuffer(    m_NumVertices * sizeof(VERTEX2UV),
                                        0,
                                        VERTEX2UV::GetFVF(),
                                        D3DPOOL_MANAGED,
                                        &m_pppVB[y][x],
                                        NULL);
            if (FAILED(hres))
            {
                return hres;
            }

            //Total amount of indices, 6 per tile
            int indAmount = PATCHSIZE * PATCHSIZE * 6;
            
            //Create buffer
            pDevice->CreateIndexBuffer(indAmount * sizeof(USHORT),
                D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16,
                D3DPOOL_MANAGED,
                &m_pppIB[y][x],
                NULL);
        }
    }

    //Setting detail level creates the contents of the indexbuffers and adjusts the normals for vertexbuffers
    char value = m_DetailLevel;
    m_DetailLevel = 255;    //Set m_DetailLevel to other value, so the data is really created
    //setDetailLevel won't do anything if parameter == m_DetailLevel
    setDetailLevel(value);

    return S_OK;
}



HRESULT UITerrain::createPassabilityTexture(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres;

    if(m_pPixelTexture)
    {
        m_pPixelTexture->Release();
        m_pPixelTexture = NULL;
    }

    Terrain* pTerrain = Terrain::getInstance();    
    unsigned short terraSize = pTerrain->getSize();

    //Default-pool, needs to be released on devicelost and recreated after restoring
    hres = D3DXCreateTexture(pDevice, terraSize, terraSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pPixelTexture);

    if (FAILED(hres))
    {
        return hres;
    }

    D3DLOCKED_RECT lockedRect;  
  
    hres = m_pPixelTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
    
    //Write color values
    if(!FAILED(hres))
    {
        for(int y = 0; y < terraSize; y++)
        {
            for(int x = 0; x < terraSize; x++)
            {
                int i = y * (lockedRect.Pitch / 4) + x;

                if(pTerrain->isPassable(x, y, 1))
                {
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (192 << 8) + 128;
                }
                else
                {
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (255 << 16);
                }
            }
        }
        
    }
    m_pPixelTexture->UnlockRect(0);

    return S_OK;

}

HRESULT UITerrain::createColorMapTexture(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres;

    if(m_pPixelTexture)
    {
        m_pPixelTexture->Release();
        m_pPixelTexture = NULL;
    }
     
    unsigned short terraSize = Terrain::getInstance()->getSize();

    //Default-pool, needs to be released on devicelost and recreated after restoring
    hres = D3DXCreateTexture(pDevice, terraSize, terraSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pPixelTexture);

    if (FAILED(hres))
    {
        return hres;
    }

    return updateColorMapTexture(NULL);

}

HRESULT UITerrain::updateColorMapTexture(bool** fogArray)
{
    Terrain* pTerrain = Terrain::getInstance();
    unsigned const char* const* ppVData = pTerrain->getTerrainVertexHeightData();
    unsigned short terraSize = pTerrain->getSize();

    D3DLOCKED_RECT lockedRect;  
  
    HRESULT hres = m_pPixelTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
    
    //Write colordata
    if(!FAILED(hres))
    {
        for(int y = 0; y < terraSize; y++)
        {
            int i = y * (lockedRect.Pitch >> 2);

            for(int x = 0; x < terraSize; x++)
            {
                if (fogArray && !fogArray[y][x]) 
                {
                    // tile is hidden
                    //Create blue for water and otherwise gray colordata based on the heightdata
                    if(ppVData[y][x] < pTerrain->getWaterLevel())
                    {                         
                        ((unsigned int*)lockedRect.pBits)[i] = (256 << 24) + (ppVData[y][x]);                        
                    }
                    else
                    {
                        int shadeFactor = ppVData[y][x] >> 1;
                        ((unsigned int*)lockedRect.pBits)[i] = (256 << 24) + (shadeFactor << 16) + (shadeFactor << 8) + (shadeFactor);
                    }
                }
                else
                {
                    // tile is visible
                    //Create colordata based on the heightdata
                    if(ppVData[y][x] < pTerrain->getWaterLevel())
                    {
                        ((unsigned int*)lockedRect.pBits)[i] = (ppVData[y][x] << 7) + (ppVData[y][x]);
                    }
                    else if(ppVData[y][x] > 192)
                    {
                        ((unsigned int*)lockedRect.pBits)[i] = (ppVData[y][x] << 16) + (ppVData[y][x] << 8) + (ppVData[y][x]);
                    }
                    else if(ppVData[y][x] > 128)
                    {    
                        ((unsigned int*)lockedRect.pBits)[i] = (ppVData[y][x] << 8) + (ppVData[y][x] >> 1);// + (ppVData[y][x] >> 1);
                    }
                    else
                    {    
                        ((unsigned int*)lockedRect.pBits)[i] = (ppVData[y][x] << 8);// + (ppVData[y][x] >> 1);
                    }
                }

                i++;
            }
        }
        
    }
    else
    {
        return hres;
    }
    m_pPixelTexture->UnlockRect(0);

    return S_OK;
}


D3DXVECTOR3 UITerrain::calculateNormalForVertex(unsigned short x, unsigned short y)
{
    unsigned char const * const * ppTerrainVertexData = Terrain::getInstance()->getTerrainVertexHeightData();    

    D3DXVECTOR3 result;
    D3DXVECTOR3 normal(0, 0, 0);

/*
Triangle edges intersecting in vertex:
   \ |
    \|
  ---*----
     |\
     | \
*/

    //Calculate sum of touching triangles' normals and normalize
    if(x > 0 && y > 0 && x < m_Size && y < m_Size)
    {
        normal += m_pppTriangleNormals[y][x-1][0];
        normal += m_pppTriangleNormals[y][x-1][1];
        normal += m_pppTriangleNormals[y][x][0];
        normal += m_pppTriangleNormals[y-1][x][0];
        normal += m_pppTriangleNormals[y-1][x][1];
        normal += m_pppTriangleNormals[y-1][x-1][1];
    }
    else
    {
        //Special case for map edges
        int xMinus1 = abs((x-1) % m_Size);
        int yMinus1 = abs((y-1) % m_Size);
        if(x == m_Size)
            x = 0;
        if(y == m_Size)
            y = 0;

        normal += m_pppTriangleNormals[y][xMinus1][0];
        normal += m_pppTriangleNormals[y][xMinus1][1];
        normal += m_pppTriangleNormals[y][x][0];
        normal += m_pppTriangleNormals[yMinus1][x][0];
        normal += m_pppTriangleNormals[yMinus1][x][1];
        normal += m_pppTriangleNormals[yMinus1][xMinus1][1];
    }

    D3DXVec3Normalize(&normal, &normal);

    return normal;
}

D3DXVECTOR3 UITerrain::getNormalAt(float x, float y, int tiles)
{
    
    D3DXVECTOR3 result(0, 0, 0);

    int halfTile = 1;

    if(tiles > 1)
    {
        halfTile = tiles >> 1;        
    }

    //Outside map, just return normal pointing "upwards" (up = -z, since map is built on xy-plane)
    if(x - halfTile < 0 || y - halfTile < 0 || x + halfTile > m_Size || y + halfTile > m_Size)
    {
        return D3DXVECTOR3(0, 0, -1);
    }
    
    //Select correct detail level and calculate index
    int detail = 1 << m_DetailLevel;
    float inverseFactor = 1.0f / detail;
    
    int iX = (int)(x * inverseFactor);
    int iY = (int)(y * inverseFactor);   

    if(tiles > 1)
    {
        int offsetX = 0;
        int offsetY = 0;
        for(int i = -halfTile; i < halfTile; i++) 
        {
            for(int j = -halfTile; j < halfTile; j++) 
            {
                offsetX = iX + j;
                offsetY = iY + i;

                if(offsetY > m_Size)
                {
                    offsetY = m_Size;
                }
                else if(offsetY < 0)
                {
                    offsetY = 0;
                }


                if(offsetX > m_Size)
                {
                    offsetX = m_Size;
                }
                else if(offsetX < 0)
                {
                    offsetX = 0;
                }

                result += m_pppTriangleNormals[offsetY][offsetX][0];
                result += m_pppTriangleNormals[offsetY][offsetX][1];
            }
        }
    }
    else
    {
        result += m_pppTriangleNormals[iY][iX][0];
        result += m_pppTriangleNormals[iY][iX][1];
    }
    D3DXVec3Normalize(&result, &result);

    return result;
}


void UITerrain::calculateTriangleNormals()
{

    D3DXVECTOR3 v0;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVECTOR3 sub1;
    D3DXVECTOR3 sub2;

    //Get detail-factor (2^m_DetailLevel = 1, 2, 4, 8...)
    unsigned char detail = 1 << m_DetailLevel;

    unsigned char const * const * ppVertexHeightData = Terrain::getInstance()->getTerrainVertexHeightData();

    int loops = m_Size / detail;

    for(int i = 0; i < loops; i++) 
    {
        for(int j = 0; j < loops; j++)
        {
            int x = j * detail;
            int y = i * detail;
            //Lower left
            v0.x = (float)x + detail;
            v0.y = (float)y;
            v0.z = -(float)ppVertexHeightData[y][x + detail] * HEIGHTFACTOR;

            v1.x = (float)x;
            v1.y = (float)y;
            v1.z = -(float)ppVertexHeightData[y][x] * HEIGHTFACTOR;

            v2.x = (float)x;
            v2.y = (float)y + detail;
            v2.z = -(float)ppVertexHeightData[y + detail][x] * HEIGHTFACTOR;

            D3DXVec3Subtract(&sub1, &v1, &v0);
            D3DXVec3Subtract(&sub2, &v2, &v0);

            D3DXVec3Cross(&m_pppTriangleNormals[i][j][0], &sub1, &sub2);
            D3DXVec3Normalize(&m_pppTriangleNormals[i][j][0], &m_pppTriangleNormals[i][j][0]);


            //Upper right
            v0.x = (float)x;
            v0.y = (float)y + detail;
            v0.z = -(float)ppVertexHeightData[y + detail][x] * HEIGHTFACTOR;

            v1.x = (float)x + detail;
            v1.y = (float)y + detail;
            v1.z = -(float)ppVertexHeightData[y + detail][x + detail] * HEIGHTFACTOR;

            v2.x = (float)x + detail;
            v2.y = (float)y;
            v2.z = -(float)ppVertexHeightData[y][x + detail] * HEIGHTFACTOR;

            D3DXVec3Subtract(&sub1, &v1, &v0);
            D3DXVec3Subtract(&sub2, &v2, &v0);
            D3DXVec3Cross(&m_pppTriangleNormals[i][j][1], &sub1, &sub2);
            D3DXVec3Normalize(&m_pppTriangleNormals[i][j][1], &m_pppTriangleNormals[i][j][1]);

        }
    }

}



float UITerrain::calculateTriangleHeightAt(float x, float y)
{

    //Sanity check
    if(x < 0 || y < 0 || x > m_Size || y > m_Size)
    {
        return 0;
    }


    float height;
    D3DXVECTOR3 normal;

    //Get detail-factor (2^m_DetailLevel = 1, 2, 4, 8...)
    int detail = 1 << m_DetailLevel;

    //Get the integer locations and "fractional part" of the float
    
    //Precalculate inverse factor so divisions can be done as multiplications
    float inverseFactor = 1.0f / detail;
    
    //Casting will drop the fractional part off, which acts like rounding down
    //The result will be the coordinate of the detail level-quad (0, 1, 2, 3...)
    int iX = (int)(x * inverseFactor);
    int iY = (int)(y * inverseFactor);
    float fractX = x - (iX * detail); //0...detail, coordinate within detail level-quad
    float fractY = y - (iY * detail); //0...detail

    //Vertex heights are got from nearest vertex that is used on this detail-level
    //Coordinate of the "real" quad (0, detail, detail * 2, detail * 3...) in model-side
    int verX = (int)(x - fractX);
    int verY = (int)(y - fractY);

    float result = 0.0f;
    bool upperRight = false;
    const unsigned char* const * ppVertexHeightData = Terrain::getInstance()->getTerrainVertexHeightData();

    //See which side of the square quad the point is
    if(fractX + fractY < detail)
    {        
        //Lower left
        height = ppVertexHeightData[verY][verX] * HEIGHTFACTOR;

        normal = m_pppTriangleNormals[iY][iX][0];
    }
    else
    {
        //Upper right
        upperRight = true;
        height = ppVertexHeightData[verY + detail][verX] * HEIGHTFACTOR;

        normal = m_pppTriangleNormals[iY][iX][1];
    }
    
    //Upper right triangles' height point is chosen from
    //upper left:
    //this one --> +--+
    //             |\ |
    //             | \|
    //             +--+
    //So the y-fraction needs to be adjusted with negative detail for upper right triangles
    if(upperRight)
    {
        fractY -= detail;
    }

    //P.z = (N.x(P.x-V0.x) + N.y(P.y-V0.y))/-N.z + V0.z 
    //result = (((normal.x * (x - verX) + normal.y * (y - verY)) / -normal.z) - height);    
    result = (((normal.x * (fractX) + normal.y * (fractY)) / -normal.z) - height);    

    return result;
    
}

void UITerrain::setDetailLevel(unsigned char detailLevel)
{
    if(m_DetailLevel == detailLevel)
    {
        return;
    }
        
    m_ChangeCounter++;
    
    m_DetailLevel = detailLevel;

    //Detail-factor, 2^m_DetailLevel (=1,2,4,8...)
    unsigned char detail = 1 << m_DetailLevel;

    //Recalculate triangle normals for new detail-level
    calculateTriangleNormals();

    //Check if using multiple buffers
    if(m_MultiBufferRender)
    {
        setDetailLevelMultiBuffer();
        return;
    }


    USHORT* pIndices = NULL;
    
    //Index-array index
    int ind = 0;
    //Vertice index
    int vIndex = 0;

    int loops = (PATCHSIZE / detail);

    //Fill index-data  (draw every "detail"th vertex as triangle corner, rowhops = (PATCHSIZE+1)*detail)
    m_pIB->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
    {
        for(int i = 0; i < loops; i++)
        {
            vIndex = i * ((PATCHSIZE + 1) * detail);

            for(int j = 0; j < loops; j++)
            {
                pIndices[ind++] = vIndex;
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail;
                pIndices[ind++] = vIndex + detail;//1;
                
                pIndices[ind++] = vIndex + detail;//1;  //Second triangle
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail;
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail + detail;
                vIndex += detail;
            }
        }
    }
    m_pIB->Unlock();



    //Single patch is PATCHSIZE * PATCHSIZE squares, detail^2 squares averaged to single quad,  2 triangles per quad
    /**
    +-+-+       +----+
    |\|\|       | \  |
    +-+-+  =>   |  \ |
    |\|\|       |   \|
    +-+-+       +----+
    */
    m_NumPrimitives = (PATCHSIZE * PATCHSIZE * 2) / (detail * detail);

    VERTEX2UV* pVertices = NULL;

    //Location within vertexbuffer
    int loc = 0;

    //Fill vertex-data
    m_pVB->Lock(0, 0, (void**)&pVertices, NULL);
    {
        for(int y = 0; y < m_Patches; y++)
        {
            for(int x = 0; x < m_Patches; x++)
            {
                loc = (y * (((PATCHSIZE+1) * (PATCHSIZE+1)) * m_Patches)) + (x * ((PATCHSIZE+1) * (PATCHSIZE+1)));
                for(int i = 0; i < ((PATCHSIZE+1) * (PATCHSIZE+1)); i += detail)
                {                   
                    pVertices[loc].z = -calculateAverageHeightForVertex((int)pVertices[loc].x, (int)pVertices[loc].y);
                    
                    //Calculate normals
                    D3DXVECTOR3 normal = getNormalAt(pVertices[loc].x, pVertices[loc].y, detail);

                    pVertices[loc].nx = normal.x;
                    pVertices[loc].ny = normal.y;
                    pVertices[loc].nz = normal.z;
                    loc += detail;
                }
            }
        }        
    }
    m_pVB->Unlock();
}

void UITerrain::setDetailLevelMultiBuffer()
{

    //Detail-factor, 2^m_DetailLevel (=1,2,4,8...)
    unsigned char detail = 1 << m_DetailLevel;

    m_NumPrimitives = (PATCHSIZE * PATCHSIZE * 2) / (detail * detail);
    unsigned short vertexSize = ((PATCHSIZE) / detail) + 1;
    VERTEX2UV* pVertices = NULL;

    for(int y = 0; y < m_Patches; y++)
    {
        for(int x = 0; x < m_Patches; x++)
        {

            //Min and max height within patch, used to set the AABB-size
            //Initialize to maximum and minimum that the heights can be
            float minHeight = (-255.0f * HEIGHTFACTOR);
            float maxHeight = 0.0f;

            //Fill vertex-data
            m_pppVB[y][x]->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
            {
                for(int i = 0; i < vertexSize; i++)
                {
                    //Y-offset to vertices based on detail level
                    int offsetY = (i * detail) + (y * PATCHSIZE);

                    for(int j = 0; j < vertexSize; j++)
                    {   
                        //Location in 1D-buffer from 2D-values
                        int loc = i * vertexSize + j;
                        
                        //X-offset to vertices based on detail-level
                        int offsetX = (j * detail) + (x * PATCHSIZE);                        

                        pVertices[loc].x = (float)offsetX;
                        pVertices[loc].y = (float)offsetY;
                        pVertices[loc].z = -calculateAverageHeightForVertex(offsetX, offsetY);
                        
                        //Check for AABBs
                        if(pVertices[loc].z < minHeight)
                        {
                            minHeight = pVertices[loc].z;
                        }
                        if(pVertices[loc].z > maxHeight)
                        {
                            maxHeight = pVertices[loc].z;
                        }

                        //Calculate normals
                        D3DXVECTOR3 normal = getNormalAt((float)offsetX, (float)offsetY, detail);

                        pVertices[loc].nx = normal.x;
                        pVertices[loc].ny = normal.y;
                        pVertices[loc].nz = normal.z;

                        //Repeating texture
                        pVertices[loc].tu = ((m_TextureRepeat * detail) / m_Size) * j;
                        pVertices[loc].tv = ((m_TextureRepeat * detail) / m_Size) * i;
                        pVertices[loc].tu2 = (1.0f / m_Size) * offsetX;
                        pVertices[loc].tv2 = (1.0f / m_Size) * offsetY;
                    }
                }
            }
            m_pppVB[y][x]->Unlock();
            
            //Set AABBs z-values
            m_pppPatchAABBs[y][x][0].z = minHeight;
            m_pppPatchAABBs[y][x][1].z = maxHeight;
           
            USHORT* pIndices = NULL;
            
            //Index-array index
            int ind = 0;
            //Vertice index
            int vIndex = 0;

            int loops = (PATCHSIZE / detail);

            //Fill index-data
            m_pppIB[y][x]->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
            {
                for(int i = 0; i < loops; i++)
                {
                    vIndex = i * vertexSize;

                    for(int j = 0; j < loops; j++)
                    {
                        pIndices[ind++] = vIndex;
                        pIndices[ind++] = vIndex + vertexSize;
                        pIndices[ind++] = vIndex + 1;
                        
                        pIndices[ind++] = vIndex + 1;  //Second triangle
                        pIndices[ind++] = vIndex + vertexSize;
                        pIndices[ind++] = vIndex + vertexSize + 1;
                        vIndex++;
                    }                    
                }
            }
            m_pppIB[y][x]->Unlock();

        }
    }
}


float UITerrain::calculateAverageHeightForVertex(unsigned short x, unsigned short y)
{

    unsigned char const * const * ppTerrainVertexData = Terrain::getInstance()->getTerrainVertexHeightData();    

    //Highest detail level uses model-data directly
    if(m_DetailLevel == 0)
    {
        return ppTerrainVertexData[y][x] * HEIGHTFACTOR;
    }

    //Calculate detail-factors
    unsigned char detail = 1 << m_DetailLevel;
    unsigned char halfDetail = detail >> 1;

    float result = 0.0f;

    int steps = 0;
    unsigned short offX;
    unsigned short offY;

    //Step a "rectangle" around the point, get height-values and average
    for(int iY = -halfDetail; iY < halfDetail; iY++)
    {
        offY = y + iY;
        if(offY < 0 || offY > m_Size)
        {
            continue;
        }

        for(int iX = -halfDetail; iX < halfDetail; iX++)
        {
            offX = x + iX;
            if(offX < 0 || offX > m_Size)
            {
                continue;
            }            
            
            result += ppTerrainVertexData[offY][offX];
            steps++;
        }
    }

    result /= (float)steps;

    return result * HEIGHTFACTOR;
}

void UITerrain::updateFog(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_FogChangeCounter != m_pCurrentPlayer->getFog()->getChangeCounter())
    {
        m_FogChangeCounter = m_pCurrentPlayer->getFog()->getChangeCounter();

        bool** fogArray = m_pCurrentPlayer->getFog()->getFogArray();
        updateColorMapTexture(fogArray);
    }
}

void UITerrain::reCreate()
{
    m_ChangeCounter++;

    //Detail-factor, 2^m_DetailLevel (=1,2,4,8...)
    unsigned char detail = 1 << m_DetailLevel;

    //Recalculate triangle normals for new detail-level
    calculateTriangleNormals();

    //Check if using multiple buffers
    if(m_MultiBufferRender)
    {
        setDetailLevelMultiBuffer();
        return;
    }

    USHORT* pIndices = NULL;
    
    //Index-array index
    int ind = 0;
    //Vertice index
    int vIndex = 0;

    int loops = (PATCHSIZE / detail);

    //indices dont't change here right?
    //Fill index-data  (draw every "detail"th vertex as triangle corner, rowhops = (PATCHSIZE+1)*detail)
    /*m_pIB->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
    {
        for(int i = 0; i < loops; ++i)
        {
            vIndex = i * ((PATCHSIZE + 1) * detail);

            for(int j = 0; j < loops; ++j)
            {
                pIndices[ind++] = vIndex;
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail;
                pIndices[ind++] = vIndex + detail;//1;
                
                pIndices[ind++] = vIndex + detail;//1;  //Second triangle
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail;
                pIndices[ind++] = vIndex + (PATCHSIZE + 1) * detail + detail;
                vIndex += detail;
            }
        }
    }
    m_pIB->Unlock();*/



    //Single patch is PATCHSIZE * PATCHSIZE squares, detail^2 squares averaged to single quad,  2 triangles per quad
    /**
    +-+-+       +----+
    |\|\|       | \  |
    +-+-+  =>   |  \ |
    |\|\|       |   \|
    +-+-+       +----+
    */
    m_NumPrimitives = (PATCHSIZE * PATCHSIZE * 2) / (detail * detail);

    VERTEX2UV* pVertices = NULL;

    //Location within vertexbuffer
    int loc = 0;

    //Fill vertex-data
    m_pVB->Lock(0, 0, (void**)&pVertices, NULL);
    {
        for(int y = 0; y < m_Patches; ++y)
        {
            for(int x = 0; x < m_Patches; ++x)
            {
                loc = (y * (((PATCHSIZEPLUSONE) * (PATCHSIZEPLUSONE)) * m_Patches)) + (x * ((PATCHSIZEPLUSONE) * (PATCHSIZEPLUSONE)));
                for(int i = 0; i < ((PATCHSIZEPLUSONE) * (PATCHSIZEPLUSONE)); i += detail)
                {                   
                    pVertices[loc].z = -calculateAverageHeightForVertex((int)pVertices[loc].x, (int)pVertices[loc].y);
                    
                    //Calculate normals
                    D3DXVECTOR3 normal = getNormalAt(pVertices[loc].x, pVertices[loc].y, detail);

                    pVertices[loc].nx = normal.x;
                    pVertices[loc].ny = normal.y;
                    pVertices[loc].nz = normal.z;
                    loc += detail;
                }
            }
        }        
    }
    m_pVB->Unlock();
}