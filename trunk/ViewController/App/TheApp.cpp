/**
 * TheApp.cpp source file
 * Implementation for the CTimer class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 29.1.2009
 * 
 * Concrete game application class
 */

#include "TheApp.h"
#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/RandomTerrainGenerator.h"

#include "../../Model/PathFinding/PathFinderMaster.h"

CTheApp::CTheApp(void)
{
    m_Help = true;

    m_fX = 10.0f;
    m_fY = 10.0f;
    m_fZ = -40.0f;    

    m_TestCube = NULL;
    m_TargetSphere = NULL;    

    m_pMat = NULL;
}


CTheApp::~CTheApp(void)
{
}


HRESULT CTheApp::OnCreate(void)
{
    PathFinderMaster::getInstance()->start();

    Terrain* pTerrain = Terrain::getInstance();
    RandomTerrainGenerator* pGenerator = new RandomTerrainGenerator(100, pTerrain->getSize());
    pTerrain->initialize(pGenerator);

    UITerrain::create(GetDevice());
    pUITerrain = UITerrain::getInstance();

    //D3DXCreateBox(GetDevice(), 1.5f, 1.25f, 0.3f, &m_TestCube, NULL);
    //D3DXCreateTeapot(GetDevice(), &m_TestCube, NULL);
    DWORD numMaterials;
    
    LPD3DXBUFFER buf;

    D3DXLoadMeshFromX(_T("supertank.X"), D3DXMESH_SYSTEMMEM,
               GetDevice(), NULL, &buf, NULL, &numMaterials, &m_TestCube );        

    D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)buf->GetBufferPointer(); 

    m_pMat = new D3DMATERIAL9[numMaterials];

    for(DWORD i = 0; i < numMaterials; i++)
    {
        m_pMat[i] = pMaterials[i].MatD3D; 
    }    

    D3DXCreateSphere(GetDevice(), 0.5f, 8, 8, &m_TargetSphere, NULL);
    
    //::memset(&m_Mat, 0, sizeof(D3DMATERIAL9));
    //m_Mat.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
    return S_OK;
}


void CTheApp::OnRelease(void)
{
    
}


void CTheApp::OnFlip(void)
{
    LPDIRECT3DDEVICE9 pDevice = GetDevice();

    // clear the background & z-buffer
    pDevice->Clear(    0,
                    NULL,
                    D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                    0xFF444444,
                    1.0f,
                    0);

    ReadKeys();

   // build the rotation matrix and set it to device
    D3DXMATRIX m;
    D3DXMATRIX view;
    /*D3DXMatrixRotationYawPitchRoll(&m, m_fYRotationAngle,
        m_fXRotationAngle,
        m_fZRotationAngle);*/
    
  /*  D3DXMatrixTranslation(&view, m_fYRotationAngle,
        m_fXRotationAngle,
        m_fZRotationAngle);
*/
    D3DXMatrixLookAtLH( &view,
                        &D3DXVECTOR3(m_fX, m_fY, m_fZ),
                        &D3DXVECTOR3(m_fX, m_fY+100.0f, m_fZ + 100.0f),
                        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    pDevice->SetTransform(D3DTS_VIEW, &view);


    //pDevice->SetTransform(D3DTS_WORLD, &m);



    //"Simulated" member variables of unit
    static float m_UnitX = 10.0f;
    static float m_UnitY = 10.0f;
    static int m_UnitSize = 3;
    static int m_HalfSize = (int)(0.5f * m_UnitSize);

    static unsigned short m_TargetX;
    static unsigned short m_TargetY;

    static D3DXVECTOR3 m_Direction;        

    static PathAgent* m_pAgent = NULL;
    
    enum UNITSTATE
    {
        ASKPATH,
        WAITPATH,
        MOVE
    };
    static UNITSTATE m_State = ASKPATH;

    static IPathFinder::PathNode* m_pPathNode;

    
    if(::GetAsyncKeyState(VK_RETURN))
    {            
        m_State = ASKPATH;
    }


    if(m_State == ASKPATH)
    {
        //Destroy possible old agent (the path data will be discarded along with it)
        if(m_pAgent)
        {
            m_pAgent->cancel();
            delete m_pAgent;
            m_pAgent = NULL;                
        }

        unsigned short terraSize = Terrain::getInstance()->getSize();
        m_TargetX = rand() % terraSize;
        m_TargetY = rand() % terraSize;
        
        m_pAgent = PathFinderMaster::getInstance()->findPath((unsigned short)m_UnitX, (unsigned short)m_UnitY,
                                                              m_TargetX, m_TargetY,
                                                              m_UnitSize);
        
        //If no agent is found, there is no path (goal is same as start location or goal is unpassable)
        if(m_pAgent != NULL)
        {
            m_State = WAITPATH;
        }
    }
    else if(m_State == WAITPATH)
    {
        if(m_pAgent->getState() == IPathFinder::FOUND)
        {
            m_State = MOVE;
            m_pPathNode = m_pAgent->getPathData();

            if(m_pPathNode == NULL)
            {
                m_State = ASKPATH;
            }
        }
        else if(m_pAgent->getState() != IPathFinder::NOT_FINISHED)
        {
            //Cancelled or no path, ask a new one
            m_State = ASKPATH;
        }
    }
    else if(m_State == MOVE)
    {
        //If close enough to current, get next target square
        if( fabs(m_UnitX - m_pPathNode->x) < 0.5f && fabs(m_UnitY - m_pPathNode->y) < 0.5f)
        {
            m_pPathNode = m_pPathNode->pChild;
        }

        //Did we finish the path?
        if(m_pPathNode == NULL)
        {
            m_State = ASKPATH;
        }
        else
        {
            m_Direction.x =  m_pPathNode->x - m_UnitX;
            m_Direction.y =  m_pPathNode->y - m_UnitY;
            m_Direction.z = 0;                                
            
            float currentSpeed = 2.0f * GetFrameTime();

            if(m_Direction.x > 0.0f)
            {
                if(currentSpeed < m_Direction.x)
                {
                    m_UnitX += currentSpeed;
                }
                else
                {
                    m_UnitX += m_Direction.x;
                }
            }
            else if(m_Direction.x < 0.0f)
            {
                if(currentSpeed > m_Direction.x)
                {
                    m_UnitX -= currentSpeed;
                }
                else
                {
                    m_UnitX -= m_Direction.x;
                }             
            }
            
            if(m_Direction.y > 0.0f)
            {
                if(currentSpeed < m_Direction.y)
                {
                    m_UnitY += currentSpeed;
                }
                else
                {
                    m_UnitY += m_Direction.y;
                }
            }
            else if(m_Direction.y < 0.0f)
            {
                if(currentSpeed > m_Direction.y)
                {
                    m_UnitY -= currentSpeed;
                }
                else
                {
                    m_UnitY -= m_Direction.y;
                }             
            }
            
        }

    }


    /*static float lang = 0.0f;
    lang += GetFrameTime();*/
    
    D3DLIGHT9 light;
    light.Type = D3DLIGHT_POINT;
    //light.Position = D3DXVECTOR3(Terrain::getInstance()->getSize() * 0.5f + sin(lang) * 150.0f, Terrain::getInstance()->getSize() * 0.5f + cos(lang) * 150.0f, -30.0f);    
    light.Position = D3DXVECTOR3(m_UnitX, m_UnitY, pUITerrain->calculateTriangleHeightAt(m_UnitX + m_HalfSize, m_UnitY + m_HalfSize) - 30.0f);
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    light.Range = 10000.0f;
    light.Attenuation0 = 0.01f;
    light.Attenuation1 = 0.001f;
    light.Attenuation2 = 0.0001f;

    pDevice->SetLight(1, &light);
    pDevice->LightEnable(1, TRUE);

    if (SUCCEEDED(pDevice->BeginScene()))
    {
        // all graphics rendering must happen in between
        // BeginScene and EndScene
        BeginText();
        
        if(m_Help)
        {
            DrawText(0, 0, _T("Use LEFT, RIGHT, UP, DOWN, A and Z to move camera"), 0xFFFFFFFF);
            DrawText(0, 20, _T("Press ESC to quit"), 0xFFFFFFFF);
            DrawText(0, 40, _T("Use space to generate new terrain"), 0xFFFFFFFF);
            DrawText(0, 60, _T("1 replaces the texture with passability data"), 0xFFFFFFFF);
            DrawText(0, 80, _T("TAB switches fillmode (solid/wireframe)"), 0xFFFFFFFF);
            DrawText(0, 100, _T("Enter forces the unit to get a new path"), 0xFFFFFFFF);
            DrawText(0, 200, _T("F1 hides/shows this help message"), 0xFFFFFFFF);            
            DrawText(0, 220, _T("M & N change terrain detail level"), 0xFFFFFFFF);            

        }

        TCHAR text[100];
        _stprintf_s(text, _T("FPS: %.2f"), (1.0f / GetFrameTime()));
        DrawText(500, 0, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Height: %.5f"), pUITerrain->calculateTriangleHeightAt(m_UnitX + m_HalfSize, m_UnitY + m_HalfSize ));
        DrawText(500, 20, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Detail level: %d"), pUITerrain->getDetailLevel());
        DrawText(500, 40, text, 0xFFFFFFFF);



        EndText();

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        pUITerrain->render(pDevice);

        {
            D3DXMATRIX unitMatrix;
            memset(unitMatrix, 0, sizeof(D3DXMATRIX));
            D3DXMatrixIdentity(&unitMatrix);
                       
            //Terrain normal
            D3DXVECTOR3 normal = pUITerrain->getNormalAt(m_UnitX, m_UnitY, m_UnitSize, m_UnitSize);

            //Get "right"-vector from crossproduct of direction (around z-axis) and normal
            D3DXVECTOR3 right;            
            D3DXVec3Normalize(&m_Direction, &m_Direction);
            D3DXVec3Cross(&right, &normal, &m_Direction);
            //D3DXVec3Normalize(&right, &right);

            //Find new "forward"-vector based on normal and right vector
            D3DXVECTOR3 forward;
            D3DXVec3Cross(&forward, &normal, &right);
        
            D3DXMATRIX axisRot;
            D3DXMatrixIdentity(&axisRot);

            axisRot._11 = right.x;
            axisRot._21 = normal.x;
            axisRot._31 = forward.x;

            axisRot._12 = right.y;
            axisRot._22 = normal.y;
            axisRot._32 = forward.y;

            axisRot._13 = right.z;
            axisRot._23 = normal.z;
            axisRot._33 = forward.z;
            
            D3DXMATRIX scale;
            D3DXMatrixScaling(&scale, 0.05f, 0.05f, 0.05f);
            D3DXMatrixMultiply(&unitMatrix, &axisRot, &scale);                                 

            unitMatrix._41 = m_UnitX + m_HalfSize;
            unitMatrix._42 = m_UnitY + m_HalfSize;
            unitMatrix._43 = pUITerrain->calculateTriangleHeightAt(unitMatrix._41, unitMatrix._42);
            
            
            pDevice->SetTransform(D3DTS_WORLD, &unitMatrix);
            
            pDevice->SetMaterial(m_pMat);            
            
            pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); //Antin meshit tarvii toiseen suuntaan cullauksen?
            pDevice->SetTexture(0, NULL);           

            //m_TestCube->DrawSubset(0);

            for(int i = 0; i < 15; i++)
            {
                m_TestCube->DrawSubset(i);
            }

            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

            //TARGET SPHERE
            if(m_pPathNode && m_State == MOVE)
            {
                D3DXMatrixIdentity(&unitMatrix);
                
                unitMatrix._41 = m_pPathNode->x + 0.5f;
                unitMatrix._42 = m_pPathNode->y + 0.5f;            
                unitMatrix._43 = pUITerrain->calculateTriangleHeightAt(m_pPathNode->x + 0.5f, m_pPathNode->y + 0.5f);

                pDevice->SetTransform(D3DTS_WORLD, &unitMatrix);

                pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);            

                m_TargetSphere->DrawSubset(0);
            }

            //TARGET SPHERE
            D3DXMatrixIdentity(&unitMatrix);
            
            unitMatrix._41 = m_TargetX + 0.5f;
            unitMatrix._42 = m_TargetY + 0.5f;            
            unitMatrix._43 = pUITerrain->calculateTriangleHeightAt(m_TargetX + 0.5f, m_TargetY + 0.5f); 

            pDevice->SetTransform(D3DTS_WORLD, &unitMatrix);

            pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);            

            m_TargetSphere->DrawSubset(0);
        }

        pDevice->EndScene();
    }
}

void CTheApp::ReadKeys()
{
    const float camspeed = 20.0f;

    // read keys to rotate the quad
    if (::GetAsyncKeyState(VK_LEFT))
    {
        m_fX -= camspeed * GetFrameTime();
    }
    else if (::GetAsyncKeyState(VK_RIGHT))
    {
        m_fX += camspeed * GetFrameTime();
    }
    
    if (::GetAsyncKeyState(VK_UP))
    {
        m_fY += camspeed * GetFrameTime();
    }
    else if (::GetAsyncKeyState(VK_DOWN))
    {
        m_fY -= camspeed * GetFrameTime();
    }
    
    if (::GetAsyncKeyState('A'))
    {
        m_fZ += camspeed * GetFrameTime();
    }
    else if (::GetAsyncKeyState('Z'))
    {
        m_fZ -= camspeed * GetFrameTime();
    }

    if(::GetAsyncKeyState(VK_SPACE))
    {
        Terrain::getInstance()->initialize();
        UITerrain::create(GetDevice());
        pUITerrain = UITerrain::getInstance();
    }
    if(::GetAsyncKeyState('1'))
    {
        pUITerrain->createPassabilityTexture(GetDevice());
    }


    if(::GetAsyncKeyState(VK_F1))
    {
        m_Help = !m_Help;
    }

    if(::GetAsyncKeyState(VK_TAB))
    {
        D3DFILLMODE f = pUITerrain->getFillMode();
        if(f == D3DFILL_SOLID)
        {
            pUITerrain->setFillMode(D3DFILL_WIREFRAME);
        }
        else
        {
            pUITerrain->setFillMode(D3DFILL_SOLID);
        }

    }

    //Slow down changing the levels
    static int counter = 0;

    counter++;
    if(counter > 40)
    {
        if(::GetAsyncKeyState('N'))
        {
            unsigned char detail = pUITerrain->getDetailLevel();
            detail++;
            if(detail < 4)
            {
                pUITerrain->setDetailLevel(detail);
            }
        }

        if(::GetAsyncKeyState('M'))
        {
            unsigned char detail = pUITerrain->getDetailLevel();
            detail--;
            //Wrapping
            if(detail < 4)
            {
                pUITerrain->setDetailLevel(detail);
            }
        }
        counter = 0;
    }
}


void CTheApp::OnKeyDown(DWORD dwKey)
{
    if (dwKey == VK_ESCAPE)
    {
        Close();
    }
}


void CTheApp::CheckTexturingCaps(void)
{
    LPDIRECT3DDEVICE9 pDevice = GetDevice();

    D3DCAPS9 caps;
    pDevice->GetDeviceCaps(&caps);


    if ( (caps.TextureCaps & D3DPTEXTURECAPS_POW2) )
    {
        // texture size must be powers of 2
        ::OutputDebugString( _T("Texture Info: texture size must be powers of 2\r\n") );
    }

    if ( (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) )
    {
        // texture width and height must be the same
        ::OutputDebugString( _T("Texture Info: texture must be square\r\n") );
    }

    if ( (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) )
    {
        // cubemapping is supported
        ::OutputDebugString( _T("Texture Info: Cubical environment mapping is supported\r\n") );
    }



    if ( (caps.TextureOpCaps & D3DTEXOPCAPS_MODULATE) )
    {
        // modulate color operation is supported
        // D3DTEXOPCAPS_MODULATE2X
        // D3DTEXOPCAPS_MODULATE4X
        // D3DTEXOPCAPS_ADD
        // ...
    }


    // maximum texture size
    TCHAR msg[256];
    _stprintf_s(    msg,
        _T("Texture Info: Max texture size is %dx%d\r\n"),
        caps.MaxTextureWidth,
        caps.MaxTextureHeight);
    ::OutputDebugString(msg);


    // maximum single pass multitexturing levels
    _stprintf_s(    msg,
        _T("Texture Info: Max singlepass multitexturing levels: %d\r\n"),
        caps.MaxSimultaneousTextures);
    ::OutputDebugString(msg);

}

