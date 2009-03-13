#include <tchar.h>

#include "UITerrain.h"
#include "../../Model/Terrain/Terrain.h"
#include "../App/vertices.h"
#include "../Camera/FrustumCull.h"

const float UITerrain::HEIGHTFACTOR = 0.15f;
UITerrain* UITerrain::pInstance = NULL;

UITerrain* UITerrain::getInstance()
{
    return pInstance;
}

UITerrain::UITerrain()
{
    m_pppIB = NULL;
    m_pppVB = NULL;
    m_pTexture = NULL;
    m_pPixelTexture = NULL;
    m_Patches = 0;
    m_NumPrimitives = 0;
    m_NumVertices = 0;
    m_DetailLevel = 0;

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
    pInstance->m_MiniMap.release();

    //Triangle normal releases
    for(int i = 0; i < m_Size; i++)
    {
        for(int j = 0; j < m_Size; j++)
        {            
            delete [] m_pppTriangleNormals[i][j];
        }
        delete [] m_pppTriangleNormals[i];
    }
    delete [] m_pppTriangleNormals;

    //AABB releases
    for(int i = 0; i < m_Patches; i++)
    {
        for(int j = 0; j < m_Patches; j++)
        {            
            delete [] m_pppPatchAABBs[i][j];
        }
        delete [] m_pppPatchAABBs[i];
    }
    delete [] m_pppPatchAABBs;



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

    //Patch vertex- and indexbuffer releases
    for(int i = 0; i < m_Patches; i++)
    {
        for(int j = 0; j < m_Patches; j++)
        {
            if(m_pppIB[i][j])
            {
                m_pppIB[i][j]->Release();                
                m_pppIB[i][j] = NULL;
            }

            if (m_pppVB[i][j])
            {
                m_pppVB[i][j]->Release();
                m_pppVB[i][j] = NULL;
            }
        }
        delete [] m_pppVB[i];
        delete [] m_pppIB[i];
    }
    delete [] m_pppVB;
    delete [] m_pppIB;
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
        pDevice->SetTexture(1, m_pPixelTexture);
        pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
        pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
    }

    pDevice->SetMaterial(&m_Mat);


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
                    pDevice->SetIndices( m_pppIB[i][j] );
                    pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_NumVertices, 0, m_NumPrimitives );
                }
            }
            else
            {
                //DEBUG:
                /*TCHAR msg[256];
                ::_stprintf_s(msg, _T("Culled terrain patch: %d : %d\n"), i, j);
                ::OutputDebugString(msg);*/
            }
        }
    }

    if(m_pPixelTexture)
    {
        pDevice->SetTexture(1, NULL);
    }

    m_MiniMap.render(pDevice);
}

HRESULT UITerrain::create(LPDIRECT3DDEVICE9 pDevice)
{
    //Get rid of old instance, if it exists
    if(pInstance)
    {        
        delete pInstance;
        pInstance = NULL;
    }

    pInstance = new UITerrain();
    return pInstance->initialize(pDevice);
}

HRESULT UITerrain::initialize(LPDIRECT3DDEVICE9 pDevice)
{
    Terrain* pTerrain = Terrain::getInstance();    
    
    const unsigned char*const* ppTerrainVertexData = pTerrain->getTerrainVertexHeightData();        
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
                                        D3DPOOL_MANAGED,    //Needs to be something else for dynamic buffer?
                                        &m_pppVB[y][x],
                                        NULL);
            if (FAILED(hres))
            {
                return hres;
            }

            //Total amount of indices, 6 per tile
            int indAmount = PATCHSIZE * PATCHSIZE * 6;
            
            //Create buffer (this needs to be dynamic too? Or is it already?)
            pDevice->CreateIndexBuffer(indAmount * sizeof(USHORT),
                D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16,
                D3DPOOL_MANAGED,
                &m_pppIB[y][x],
                NULL);
        }
    }

    //Setting detail level creates the contents of the index- and vertexbuffers
    setDetailLevel(m_DetailLevel);

    //TODO: Multiple different "ground" textures?
    hres = D3DXCreateTextureFromFile(pDevice, _T("grass01.png"), &m_pTexture);
    if(FAILED(hres))
    {
        return hres;
    }

    //Create colormap
    hres = createColorMapTexture(pDevice);

    if(FAILED(hres))
    {
        return hres;
    }

    //Create minimap and set it to use the colormap-texture
    m_MiniMap.create();
    m_MiniMap.setTexture(m_pPixelTexture);

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

    hres = D3DXCreateTexture(pDevice, terraSize, terraSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pPixelTexture);

    if (FAILED(hres))
    {
        return hres;
    }

    D3DLOCKED_RECT lockedRect;  
  
    hres = m_pPixelTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
    
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

    Terrain* pTerrain = Terrain::getInstance();    
    unsigned short terraSize = pTerrain->getSize();

    hres = D3DXCreateTexture(pDevice, terraSize, terraSize, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pPixelTexture);

    if (FAILED(hres))
    {
        return hres;
    }

    unsigned const char* const* ppVData = pTerrain->getTerrainVertexHeightData();

    D3DLOCKED_RECT lockedRect;  
  
    hres = m_pPixelTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);
    
    if(!FAILED(hres))
    {
        for(int y = 0; y < terraSize; y++)
        {
            for(int x = 0; x < terraSize; x++)
            {
                int i = y * (lockedRect.Pitch / 4) + x;
                
                //Create colordata based on the heightdata
                if(ppVData[y][x] < pTerrain->getWaterLevel())
                {
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (ppVData[y][x] << 7) + (ppVData[y][x]);
                }
                else if(ppVData[y][x] > 192)
                {
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (ppVData[y][x] << 16) + (ppVData[y][x] << 8) + ppVData[y][x];
                }
                else if(ppVData[y][x] > 128)
                {    
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (ppVData[y][x] << 8) + (ppVData[y][x] >> 1);// + (ppVData[y][x] >> 1);
                }
                else
                {    
                    ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (ppVData[y][x] << 8);// + (ppVData[y][x] >> 1);
                }
            }
        }
        
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

    if(x > 0 && y > 0 && x < m_Size && y < m_Size)
    {
        //Calculate sum of touching triangles' normals and normalize
        normal += m_pppTriangleNormals[y][x-1][0];
        normal += m_pppTriangleNormals[y][x-1][1];
        normal += m_pppTriangleNormals[y][x][0];
        normal += m_pppTriangleNormals[y-1][x][0];
        normal += m_pppTriangleNormals[y-1][x][1];
        normal += m_pppTriangleNormals[y-1][x-1][1];
    }
    else
    {
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

D3DXVECTOR3 UITerrain::getNormalAt(float x, float y, int tilesX, int tilesY)
{
    
    D3DXVECTOR3 result(0, 0, 0);
    
    if(x < 0 || y < 0 || x + tilesX > m_Size || y + tilesY > m_Size)
    {
        return D3DXVECTOR3(0, 0, -1);
    }

    /*
    for(int ix=0; ix<tilesX; ix++) {
        for(int iy=0; iy<tilesY; iy++) {
            result += calculateNormalForVertex((unsigned short)x+ix, (unsigned short)y+iy);
        }
    }

    D3DXVec3Normalize(&result, &result);*/
    
    int detail = 1 << m_DetailLevel;
    float inverseFactor = 1.0f / detail;
    
    int iX = (int)(x * inverseFactor);
    int iY = (int)(y * inverseFactor);
    
    tilesX = (int)((float)tilesX * inverseFactor);
    tilesY = (int)((float)tilesY * inverseFactor);

    if(tilesX + tilesY > 0)
    {
        for(int i = 0; i < tilesX; i++) 
        {
            for(int j = 0; j < tilesY; j++) 
            {
                result += m_pppTriangleNormals[iY + i][iX + j][0];
                result += m_pppTriangleNormals[iY + i][iX + j][1];
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
    //The result will be the coordinate of the detail level-quad (0, 1, 2, 3
    int iX = (int)(x * inverseFactor);
    int iY = (int)(y * inverseFactor);
    float fractX = x - (iX * detail); //0...detail, coordinate within detail level-quad
    float fractY = y - (iY * detail); //0...detail

    //Vertex heights are got from nearest vertex that is used on this detail-level
    //Coordinate of the "real" quad (0, detail, detail * 2, detail * 3...) in model-side
    int verX = iX * detail;//(int)(x - fractX);
    int verY = iY * detail;//(int)(y - fractY);

    float result = 0.0f;
    bool upperRight = false;
    const unsigned char* const * ppVertexHeightData = Terrain::getInstance()->getTerrainVertexHeightData();

    //See which side of the terrain quad the point is
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
    
    //Somethings need to be done on larger than 1x1 quads...
    //if(detail > 1)
    {
        //Upper right triangles' height point is chosen from
        //upper left:
        //this one --> +--+
        //             |\ |
        //             | \|
        //             +--+
        //So the y-fraction needs to be adjusted to negative for upper right triangles
        if(upperRight)
        {
            fractY -= detail;
        }
        //fractX *= inverseFactor;
        //fractY *= inverseFactor;
        //normal *= inverseFactor;
    }

    //P.z = (N.x(P.x-V0.x) + N.y(P.y-V0.y))/-N.z + V0.z 
    //result = (((normal.x * (x - verX) + normal.y * (y - verY)) / -normal.z) - height);    
    result = (((normal.x * (fractX) + normal.y * (fractY)) / -normal.z) - height);    

    return result;
    
}

void UITerrain::setDetailLevel(unsigned char detailLevel)
{
    m_DetailLevel = detailLevel;

    //Detail-factor, 2^m_DetailLevel (=1,2,4,8...)
    unsigned char detail = 1 << m_DetailLevel;

    //Recalculate triangle normals for new detail-level
    calculateTriangleNormals();

    m_NumPrimitives = (PATCHSIZE * PATCHSIZE * 2) / (detail * detail);
    unsigned short vertexSize = ((PATCHSIZE) / detail) + 1;
    VERTEX2UV* pVertices = NULL;

    unsigned const char* const * ppTerrainVertexData = Terrain::getInstance()->getTerrainVertexHeightData();    

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
                    for(int j = 0; j < vertexSize; j++)
                    {   
                        //Location in 1D-buffer from 2D-values
                        int loc = i * vertexSize + j;
                        
                        int offsetX = (j * detail) + (x * PATCHSIZE);
                        int offsetY = (i * detail) + (y * PATCHSIZE);

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
                        D3DXVECTOR3 normal = getNormalAt((float)offsetX, (float)offsetY, detail, detail); //calculateNormalForVertex(offsetX, offsetY);

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

    if(m_DetailLevel == 0)
    {
        return ppTerrainVertexData[y][x] * HEIGHTFACTOR;
    }

    unsigned char detail = 1 << m_DetailLevel;
    unsigned char halfDetail = detail >> 1;

    float result = 0.0f;

    int steps = 0;
    unsigned short offX;
    unsigned short offY;

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