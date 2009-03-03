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
#include "../../Model/Asset/AssetFactory.h"

CTheApp::CTheApp(void)
{
    m_Help = true;

    m_fX = 10.0f;
    m_fY = 10.0f;
    m_fZ = -40.0f;    

    m_pManager = NULL;    
}


CTheApp::~CTheApp(void)
{
}


HRESULT CTheApp::OnCreate(void)
{
    //Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    //Prepare manager and set scale
    UI3DObjectManager::setDevice(GetDevice());
    m_pManager = UI3DObjectManager::getInstance();

    //TEST
    for(int i = 0; i < 50; i++)
    {
        AssetFactory::createUnit(NULL, 0, RandInt(0, 200), RandInt(0, 200));
    }

    Terrain* pTerrain = Terrain::getInstance();
    RandomTerrainGenerator* pGenerator = new RandomTerrainGenerator(100, pTerrain->getSize());
    pTerrain->initialize(pGenerator);

    UITerrain::create(GetDevice());
    pUITerrain = UITerrain::getInstance();

    return S_OK;
}


void CTheApp::OnRelease(void)
{
    PathFinderMaster::cancelAll();
    PathFinderMaster::getInstance()->stop();
    Sleep(100);
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



    float frameTime = GetFrameTime();
    m_pManager->getRootObject()->Update(frameTime);
    AssetCollection::updateUnits(frameTime);



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

        _stprintf_s(text, _T("Detail level: %d"), pUITerrain->getDetailLevel());
        DrawText(500, 40, text, 0xFFFFFFFF);



        EndText();

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        pUITerrain->render(pDevice);
                   
        m_pManager->getRootObject()->Render(pDevice);

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

