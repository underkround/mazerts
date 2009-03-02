#include <tchar.h>

#include "UITerrain.h"
#include "../../Model/Terrain/Terrain.h"
#include "../App/vertices.h"

const float UITerrain::HEIGHTFACTOR = 0.1f;
UITerrain* UITerrain::pInstance = NULL;

UITerrain* UITerrain::getInstance()
{
    return pInstance;
}

UITerrain::UITerrain()
{
    m_pIB = NULL;
    m_pVB = NULL;
    m_pTexture = NULL;
	m_pPixelTexture = NULL;
    m_Patches = 0;
    m_NumPrimitives = 0;
    m_NumVertices = 0;
    m_DetailLevel = 0;

#ifdef HEIGHTMAPCOLORING
    pVertices = NULL;
    pIndices = NULL;
#endif

    D3DXMatrixIdentity(&m_World);

    m_FillMode = D3DFILL_SOLID;
    
    m_TextureRepeat = 8.0f;
    
    ::memset(&m_Mat, 0, sizeof(D3DMATERIAL9));
    m_Mat.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    
}

UITerrain::~UITerrain(void)
{
    release();
}

void UITerrain::release()
{
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
            if(m_pIB[i][j])
            {
                m_pIB[i][j]->Release();
                m_pIB[i][j] = NULL;
            }

            if (m_pVB[i][j])
            {
                m_pVB[i][j]->Release();
                m_pVB[i][j] = NULL;
            }
        }
        delete [] m_pVB[i];
        delete [] m_pIB[i];
    }
    delete [] m_pVB;
    delete [] m_pIB;
}

void UITerrain::render(LPDIRECT3DDEVICE9 pDevice)
{

    pDevice->SetTransform(D3DTS_WORLD, &m_World);

#ifndef HEIGHTMAPCOLORING

    pDevice->SetFVF( VERTEX2UV::GetFVF() );
    pDevice->SetRenderState(D3DRS_FILLMODE, m_FillMode);

    for(int i = 0; i < m_Patches; i++)
    {
        for(int j = 0; j < m_Patches; j++)
        {
            //TEST
            //if((i + j) % 2 == 1)
            {
            pDevice->SetStreamSource( 0, m_pVB[i][j], 0, sizeof(VERTEX2UV) );        
    
    
            if(m_pTexture)
            {
                // set the texturing parameters
                pDevice->SetTexture(0, m_pTexture);
                //pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                //pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
                //pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
            }

			if(m_pPixelTexture)
			{
				pDevice->SetTexture(1, m_pPixelTexture);
				pDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
				pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
                pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
                pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
			}

            //pDevice->SetMaterial(&m_Mat);

            if ( m_pIB[i][j] )
            {
                pDevice->SetIndices( m_pIB[i][j] );
                pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_NumVertices, 0, m_NumPrimitives );
            }
            }
        }

    }


#else
    //Use height-based vertexcolors, for debugging
    pDevice->SetFVF( LITVERTEX::GetFVF() );

    // set the material
    D3DMATERIAL9 material;
    ::memset(&material, 0, sizeof(D3DMATERIAL9));
    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    pDevice->SetMaterial(&material);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
            0, m_numVertices, m_numPrimitives, 
            pIndices, D3DFMT_INDEX16, pVertices, sizeof(LITVERTEX));
    /*if ( m_pIB )
    {
        
        pDevice->SetIndices( m_pIB );
        pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_numVertices, 0, m_numPrimitives );
    } */
#endif
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

    //Calculate trianglenormals
    calculateTriangleNormals();
    

    //Vertexbuffer (STATIC!)
    //TODO: Find out how to use dynamic buffers
    HRESULT hres;

    m_pVB = new LPDIRECT3DVERTEXBUFFER9*[m_Patches];
    m_pIB = new LPDIRECT3DINDEXBUFFER9*[m_Patches];

    for(int y = 0; y < m_Patches; y++)
    {        
        m_pVB[y] = new LPDIRECT3DVERTEXBUFFER9[m_Patches];
        m_pIB[y] = new LPDIRECT3DINDEXBUFFER9[m_Patches];

        for(int x = 0; x < m_Patches; x++)
        {
//            m_pVB[i] = new IDirect3DVertexBuffer9;

#ifndef HEIGHTMAPCOLORING
            hres = pDevice->CreateVertexBuffer(    m_NumVertices * sizeof(VERTEX2UV),
                                        0,
                                        VERTEX2UV::GetFVF(),
#else
    hres = pDevice->CreateVertexBuffer(    m_NumVertices * sizeof(LITVERTEX),
                                        0,
                                        LITVERTEX::GetFVF(),
#endif
                                        D3DPOOL_MANAGED,    //Needs to be something else for dynamic?
                                        &m_pVB[y][x],
                                        NULL);
            if (FAILED(hres))
            {
                return hres;
            }

            //Pointer to vertex-data
#ifndef HEIGHTMAPCOLORING
            VERTEX2UV* pVertices = NULL;
#else
            pVertices = NULL;
#endif

            //Fill vertex-data
            m_pVB[y][x]->Lock(0, 0, (void**)&pVertices, 0);
            {
                for(int i = 0; i < vertexSize; i++)
                {
                    for(int j = 0; j < vertexSize; j++)
                    {   
                        //Location in 1D-buffer from 2D-values
                        int loc = i * vertexSize + j;
                        
                        int offsetX = j + (x * PATCHSIZE);
                        int offsetY = i + (y * PATCHSIZE);

                        pVertices[loc].x = (float)offsetX;
                        pVertices[loc].y = (float)offsetY;
                        pVertices[loc].z = -(float)ppTerrainVertexData[offsetY][offsetX] * HEIGHTFACTOR;
                        
#ifndef HEIGHTMAPCOLORING
                        //If using VERTEX2UV, calculate normals
                        D3DXVECTOR3 normal = calculateNormalForVertex(offsetX, offsetY);

                        pVertices[loc].nx = normal.x;
                        pVertices[loc].ny = normal.y;
                        pVertices[loc].nz = normal.z;
#else                
                        //If using LITVERTEX, set color
                        unsigned short h = ppTerrainVertexData[i][j];
                        unsigned short r = h;
                        unsigned short g = h % 128;
                        unsigned short b = 255 - h;

                        pVertices[loc].dwColor = D3DCOLOR_XRGB(r, g, b);
#endif
 
                        //Repeating texture
                        pVertices[loc].tu = (m_TextureRepeat / m_Size) * j;
                        pVertices[loc].tv = (m_TextureRepeat / m_Size) * i;
						pVertices[loc].tu2 = (1.0f / m_Size) * offsetX;
						pVertices[loc].tv2 = (1.0f / m_Size) * offsetY;
                    }
                }
            }
            m_pVB[y][x]->Unlock();

            //Number of needed triangles, 2 per tile
            m_NumPrimitives = PATCHSIZE * PATCHSIZE * 2;

            //Total amount of indices, 6 per tile
            int indAmount = PATCHSIZE * PATCHSIZE * 6;
            
            //Create buffer (this needs to be dynamic too? Or is it already?)
            pDevice->CreateIndexBuffer(indAmount * sizeof(USHORT),
                D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16,
                D3DPOOL_MANAGED,
                &m_pIB[y][x],
                NULL);

#ifndef HEIGHTMAPCOLORING
            USHORT* pIndices = NULL;
#else
            pIndices = NULL;
#endif

            
            //Index-array index
            int ind = 0;
            //Vertice index
            int vIndex = 0;

            //Fill index-data
            m_pIB[y][x]->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
            {
                for(int i = 0; i < PATCHSIZE; i++)
                {
                    for(int j = 0; j < PATCHSIZE; j++)
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
            m_pIB[y][x]->Unlock();

#ifndef HEIGHTMAPCOLORING
            hres = D3DXCreateTextureFromFile(    pDevice,
                                        _T("grass01.png"),
                                        &m_pTexture);
    
#endif
        }
    }

    return S_OK;
}

HRESULT UITerrain::createPassabilityTexture(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT hres;

    if(m_pPixelTexture)
    {
        m_pPixelTexture->Release();
    }

    Terrain* pTerrain = Terrain::getInstance();    
    unsigned short terraSize = pTerrain->getSize() + 1;

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
        
        /*int sizeARGB = terraSize * terraSize * (lockedRect.Pitch / terraSize / 4);//64bit values... wut?
        for(int i = 0; i < sizeARGB; i++)*/
        {
            /*if((i % 4) == 0)
            {
                ((unsigned char*)lockedRect.pBits)[i] = 255;
            }
            else
            {
                ((unsigned char*)lockedRect.pBits)[i] = 0;
            }*/            
            /*if(pTerrain->isPassable(i % terraSize, i / terraSize, 1))
            {
                ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (255 << 16) + (i % 255 << 8) + (i % 255);
            }
            else
            {
                ((unsigned int*)lockedRect.pBits)[i] = (255 << 24) + (255 << 16); //+ (i % 255 << 8) + (i % 255);
            } */
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
    normal.z = -normal.z;

    return normal;
}

D3DXVECTOR3 UITerrain::getNormalAt(float x, float y, int tilesX, int tilesY)
{
    //D3DXVECTOR3 result = calculateNormalForVertex((unsigned short)x, (unsigned short)y);
    D3DXVECTOR3 result;
    result.x = result.y = result.z = 0;
    for(int ix=0; ix<tilesX; ix++) {
        for(int iy=0; iy<tilesY; iy++) {
            result += calculateNormalForVertex((unsigned short)x+ix, (unsigned short)y+iy);
        }
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

    unsigned char const * const * ppVertexHeightData = Terrain::getInstance()->getTerrainVertexHeightData();

    m_pppTriangleNormals = new D3DXVECTOR3**[m_Size];
    for(int i = 0; i < m_Size; i++)
    {
        m_pppTriangleNormals[i] = new D3DXVECTOR3*[m_Size];

        for(int j = 0; j < m_Size; j++)
        {
            m_pppTriangleNormals[i][j] = new D3DXVECTOR3[2];
        }
    }

    for(int i = 0; i < m_Size; i++)    
    {
        for(int j = 0; j < m_Size; j++)        
        {
            //Lower left
            v0.x = (float)j + 1.0f;
            v0.y = (float)i;
            v0.z = (float)ppVertexHeightData[i][j + 1] * HEIGHTFACTOR;

            v1.x = (float)j;
            v1.y = (float)i;
            v1.z = (float)ppVertexHeightData[i][j] * HEIGHTFACTOR;

            v2.x = (float)j;
            v2.y = (float)i + 1.0f;
            v2.z = (float)ppVertexHeightData[i + 1][j] * HEIGHTFACTOR;

            D3DXVec3Subtract(&sub1, &v1, &v0);
            D3DXVec3Subtract(&sub2, &v2, &v0);

            D3DXVec3Cross(&m_pppTriangleNormals[i][j][0], &sub2, &sub1);            
            D3DXVec3Normalize(&m_pppTriangleNormals[i][j][0], &m_pppTriangleNormals[i][j][0]);

            //Upper right
            v0.x = (float)j;
            v0.y = (float)i + 1.0f;
            v0.z = (float)ppVertexHeightData[i + 1][j] * HEIGHTFACTOR;

            v1.x = (float)j + 1.0f;
            v1.y = (float)i + 1.0f;
            v1.z = (float)ppVertexHeightData[i + 1][j + 1] * HEIGHTFACTOR;

            v2.x = (float)j + 1.0f;
            v2.y = (float)i;
            v2.z = (float)ppVertexHeightData[i][j + 1] * HEIGHTFACTOR;

            D3DXVec3Subtract(&sub1, &v1, &v0);
            D3DXVec3Subtract(&sub2, &v2, &v0);
            D3DXVec3Cross(&m_pppTriangleNormals[i][j][1], &sub2, &sub1);   
            D3DXVec3Normalize(&m_pppTriangleNormals[i][j][1], &m_pppTriangleNormals[i][j][1]);
        }
    }

}



float UITerrain::calculateTriangleHeightAt(float x, float y)
{
    D3DXVECTOR3 v0;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVECTOR3 normal;

    //Get the integer locations and fractional part of the float
    unsigned short iX = (unsigned short)x;
    unsigned short iY = (unsigned short)y;
    float fractX = x - iX; //0...1.0f
    float fractY = y - iY; //0...1.0f
    
    float result = 0.0f;

    const unsigned char* const * ppVertexHeightData = Terrain::getInstance()->getTerrainVertexHeightData();

    //See which side of the terrain quad the point is
    if(fractX + fractY < 1.0f)
    {        
        //Lower left
        v0.x = iX + 1.0f;
        v0.y = iY;
        v0.z = ppVertexHeightData[iY][iX + 1] * HEIGHTFACTOR;

        v1.x = iX;
        v1.y = iY;
        v1.z = ppVertexHeightData[iY][iX] * HEIGHTFACTOR;

        v2.x = iX;
        v2.y = iY + 1.0f;
        v2.z = ppVertexHeightData[iY + 1][iX] * HEIGHTFACTOR;

        normal = m_pppTriangleNormals[iY][iX][0];
    }
    else
    {
        //Upper right
        
        v0.x = iX;
        v0.y = iY + 1.0f;
        v0.z = ppVertexHeightData[iY + 1][iX] * HEIGHTFACTOR;

        v1.x = iX + 1.0f;
        v1.y = iY + 1.0f;
        v1.z = ppVertexHeightData[iY + 1][iX + 1] * HEIGHTFACTOR;

        v2.x = iX + 1.0f;
        v2.y = iY;
        v2.z = ppVertexHeightData[iY][iX + 1] * HEIGHTFACTOR;

        normal = m_pppTriangleNormals[iY][iX][1];
    }
    
    //P.z = (N.x(P.x-V0.x) + N.y(P.y-V0.y))/-N.z + V0.z 
    result = (((normal.x * (x - v0.x) + normal.y * (y - v0.y)) / normal.z) - v0.z);

    return result;
    
}

void UITerrain::setDetailLevel(unsigned char detailLevel)
{
    m_DetailLevel = detailLevel;

    unsigned char detail = 1 << m_DetailLevel;

    USHORT* pIndices = NULL;

    int rowHop = (PATCHSIZE + 1) * detail;

    m_NumPrimitives = (PATCHSIZE * PATCHSIZE * 2) / (detail * detail);

    for(int y = 0; y < m_Patches; y++)
    {
        for(int x = 0; x < m_Patches; x++)
        {
            //Index-array index
            int ind = 0;
            //Vertice index
            int vIndex = 0;

            //Fill index-data
            m_pIB[y][x]->Lock(0, 0, (void**)&pIndices, D3DLOCK_DISCARD);
            {
                for(int i = 0; i < (PATCHSIZE / detail); i++)
                {
                    vIndex = rowHop * i;

                    for(int j = 0; j < (PATCHSIZE / detail); j++)
                    {
                        pIndices[ind++] = vIndex;
                        pIndices[ind++] = vIndex + rowHop;
                        pIndices[ind++] = vIndex + detail;
                        
                        pIndices[ind++] = vIndex + detail;  //Second triangle
                        pIndices[ind++] = vIndex + rowHop;
                        pIndices[ind++] = vIndex + rowHop + detail;
                        vIndex += detail;
                    }                               
                }
            }
            m_pIB[y][x]->Unlock();
        }
    }
}