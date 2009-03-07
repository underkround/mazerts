/**
 * TheApp.cpp source file
 * 
 * Concrete game application class
 */


#include "TheApp.h"
#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/AntinTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../../Model/Asset/AssetFactory.h"

//Camera related
#include "../Culling/FrustumCull.h"

//DEBUG STUFF
#include "../3DDebug/UI3DDebug.h"
#include "../Terrain/TerrainIntersection.h"


#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 2.0f

CTheApp::CTheApp(void)
{
    m_Help = true;

    m_fX = 127.0f;
    m_fY = -70.0f;
    m_fZ = -170.0f;

    m_TextRow = 0;
	m_iMouseX = 0;
	m_iMouseY = 0;
	m_iMouseZ = 0;

    m_pManager = NULL;
}


CTheApp::~CTheApp(void)
{
}


HRESULT CTheApp::OnCreate(void)
{
    HRESULT hres;

    //initialize input engine
    hres = m_InputEngine.Create(GetWindow());
	if(FAILED(hres))
	{
		::MessageBox( GetWindow(), _T("failed to init input engine."), _T("error"), MB_OK);
	}

	//enumerate attached input devices
	m_InputEngine.GetKeyboards(m_arrInputDevices);
	m_InputEngine.GetMice(m_arrInputDevices);

	hres = m_Keyboard.Create(m_InputEngine, NULL, 0);
	hres = m_Mouse.Create(m_InputEngine, NULL, 0);
 
	//init mouse coordinates
	if(IsWindowed())
	{
		POINT mousepos;
		::GetCursorPos(&mousepos);
		::ScreenToClient(GetWindow(), &mousepos);
		m_iMouseX = mousepos.x;
		m_iMouseY = mousepos.y;
	}

    //Set device to debug-helper
    UI3DDebug::setDevice(GetDevice());

    //Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    //Prepare manager and set scale
    UI3DObjectManager::setDevice(GetDevice());
    m_pManager = UI3DObjectManager::getInstance();

    //TEST
    for(int i = 0; i < 100; i++)
    {
        AssetFactory::createUnit(NULL, 0, RandInt(0, 200), RandInt(0, 200));
    }

    Terrain* pTerrain = Terrain::getInstance();
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512);
    pTerrain->initialize(pGenerator);

    pTerrain->setWaterLevel(50);


    UITerrain::create(GetDevice());
    m_pUITerrain = UITerrain::getInstance();
                              
    return S_OK;
}


void CTheApp::OnRelease(void)
{
    PathFinderMaster::cancelAll();
    //Tell the thread to stop and wait for it to finish
    PathFinderMaster::getInstance()->stop();
    PathFinderMaster::getInstance()->wait();

    // clear the device array
	m_arrInputDevices.clear();
    // release input devices
	m_Mouse.Release();
	m_Keyboard.Release();
 	// release the input engine
	m_InputEngine.Release();
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

    
    //Camera stuff, separate to own method(s)/class
    D3DXMATRIX view;
    D3DXMatrixLookAtLH( &view,
                        &D3DXVECTOR3(m_fX, m_fY, m_fZ),
                        &D3DXVECTOR3(m_fX, m_fY+100.0f, m_fZ + m_iMouseY),//100.0f),
                        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    pDevice->SetTransform(D3DTS_VIEW, &view);

    D3DXMATRIX proj;
    pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    FrustumCull::rebuildFrustum(&view, &proj);


    //Timers for profiling
    static bool timersCreated = false;
    static CTimer* timer1;
    static CTimer* timer2;
    static float time = 0.0f;

    if(!timersCreated)
    {    
        timer1 = new CTimer();
        timer1->Create();
        timer2 = new CTimer();
        timer2->Create();
        timersCreated = true;
    }

    //Deltatime
    float frameTime = GetFrameTime();


    //Input updates
    UpdateKeyboard();
    timer1->BeginTimer();
	UpdateMouse();
    timer1->EndTimer();

    //timer1->BeginTimer();

    //Updating the actual game logic    
    AssetCollection::updateUnits(frameTime);
    m_pManager->getRootObject()->Update(frameTime);
    //End of logic update

    //timer1->EndTimer();

    //Light is here for testing (and doesn't need to be set every frame, as it doesn't move anyway)
    D3DLIGHT9 light;
    light.Type = D3DLIGHT_POINT;
    light.Position = D3DXVECTOR3(100.0f, 100.0f, -100.0f);    
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    light.Range = 10000.0f;    
    light.Attenuation0 = 0.001f;
    light.Attenuation1 = 0.0001f;
    light.Attenuation2 = 0.00001f;

    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, TRUE);


    
    //Rendering operations
    timer2->BeginTimer();
    if (SUCCEEDED(pDevice->BeginScene()))
    {
        //Text drawing
        BeginText();

		DrawText(   m_iMouseX,
				    m_iMouseY,
				    _T("X"),
				    0xFFFFFFFF);

        
        if(m_Help)
        {
            DrawTextRow(_T("Drag with right mouse button pressed to pan view"), 0xFFFFFFFF);
            DrawTextRow(_T("Mouse scroll wheel zooms"), 0xFFFFFFFF);
            DrawTextRow(_T("Use LEFT, RIGHT, UP, DOWN, A and Z to move camera"), 0xFFFFFFFF);
            DrawTextRow(_T("Use space to generate new terrain"), 0xFFFFFFFF);
            DrawTextRow(_T("1 adds the texture with passability data"), 0xFFFFFFFF);
            DrawTextRow(_T("TAB switches fillmode (solid/wireframe)"), 0xFFFFFFFF);            
            DrawTextRow(_T("M & N change terrain detail level"), 0xFFFFFFFF);
            DrawTextRow(_T("F1 hides/shows this help message"), 0xFFFFFFFF);                        
        }

        TCHAR text[100];
        _stprintf_s(text, _T("FPS: %.2f"), (1.0f / GetFrameTime()));
        DrawText(500, 0, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Update: %.2fms  Render: %.2fms"), timer1->GetElapsedSeconds() * 1000.0f, time * 1000.0f);
        DrawText(500, 20, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Detail level: %d"), m_pUITerrain->getDetailLevel());
        DrawText(500, 40, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Pathfinders  Running:%d  Waiting: %d"), PathFinderMaster::getRunningAmount(), PathFinderMaster::getWaitingAmount());
        DrawText(500, 60, text, 0xFFFFFFFF);        

        m_TextRow = 0;
        EndText();

        
        //Terrain needs normal backface-culling
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);        
        m_pUITerrain->render(pDevice);
          
        //Antsys models need reverse backface-culling
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        m_pManager->getRootObject()->Render(pDevice);

        pDevice->EndScene();
    }
    timer2->EndTimer();
    time = timer2->GetElapsedSeconds();
}

void CTheApp::DrawTextRow(LPCTSTR text, DWORD color)
{
    DrawText(0, m_TextRow, text, color);
    m_TextRow += GetTextHeight();
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

void CTheApp::UpdateKeyboard(void)
{
	//read the keyboard data
	DWORD i;
	//TCHAR msg[256];
 
	if(SUCCEEDED(m_Keyboard.Update()))
	{
		//for(i=0; i<m_Keyboard.GetButtonCount(); i++)
		{

            if (m_Keyboard.GetButton(203))
            {
                m_fX -= KEYBOARD_CAMSPEED * GetFrameTime();
            }
            else if (m_Keyboard.GetButton(205))
            {
                m_fX += KEYBOARD_CAMSPEED * GetFrameTime();
            }
            
            if (m_Keyboard.GetButton(200))
            {
                m_fY += KEYBOARD_CAMSPEED * GetFrameTime();
            }
            else if (m_Keyboard.GetButton(208))
            {
                m_fY -= KEYBOARD_CAMSPEED * GetFrameTime();
            }
            
            if (m_Keyboard.GetButton(30))
            {
                m_fZ += KEYBOARD_CAMSPEED * GetFrameTime();
            }
            else if (m_Keyboard.GetButton(44))
            {
                m_fZ -= KEYBOARD_CAMSPEED * GetFrameTime();
            }

            if(m_Keyboard.GetButton(57))
            {
                //The Right Way(tm) to create new terrain
                PathFinderMaster* pMaster = PathFinderMaster::getInstance();
                pMaster->stop();
                pMaster->wait();
                
                Terrain::getInstance()->initialize();
                UITerrain::create(GetDevice());
                m_pUITerrain = UITerrain::getInstance();
                
                pMaster->start();
            }

            if(m_Keyboard.GetButton(2))
            {
                m_pUITerrain->createPassabilityTexture(GetDevice());
            }


            if(m_Keyboard.GetButton(59))
            {
                m_Help = !m_Help;
            }

            if(m_Keyboard.GetButton(15))
            {
                D3DFILLMODE f = m_pUITerrain->getFillMode();
                if(f == D3DFILL_SOLID)
                {
                    m_pUITerrain->setFillMode(D3DFILL_WIREFRAME);
                }
                else
                {
                    m_pUITerrain->setFillMode(D3DFILL_SOLID);
                }

            }

            if(m_Keyboard.GetButton(49))
            {
                unsigned char detail = m_pUITerrain->getDetailLevel();
                detail++;
                if(detail < 4)
                {
                    m_pUITerrain->setDetailLevel2(detail);
                }
            }

            if(m_Keyboard.GetButton(50))
            {
                unsigned char detail = m_pUITerrain->getDetailLevel();
                detail--;
                //Wrapping
                if(detail < 4)
                {
                    m_pUITerrain->setDetailLevel2(detail);
                }
            }

			/*if(m_Keyboard.GetButton(i))
			{
                #ifdef _DEBUG
				_stprintf_s(msg, _T("KEYBOARD: %d DOWN"), i);
				DrawText(	0,
						m_TextRow,
						msg,
						D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));
				m_TextRow += GetTextHeight();
                #endif
		    }*/
		}
	}
	m_TextRow += GetTextHeight();
}
 
void CTheApp::UpdateMouse(void)
{
	//read the mouse data
	//DWORD i;
	//TCHAR msg[256];

	if(SUCCEEDED(m_Mouse.Update()))
	{
		//please note that we track mouse position by reading it's changes, not absolute coordinates
		m_iMouseX += m_Mouse.GetState().lX;
		m_iMouseY += m_Mouse.GetState().lY;
		m_iMouseZ += m_Mouse.GetState().lZ;
 
		// keep mouse positions inside our window
		if(m_iMouseX <0) {
			m_iMouseX = 0;
		}
		if(m_iMouseY <0) {
			m_iMouseY = 0;
		}
		const int iMaxX = GetWindowRect().right;
		if(m_iMouseX > iMaxX) {
			m_iMouseX = iMaxX;
		}
		const int iMaxY = GetWindowRect().bottom;
		if(m_iMouseY > iMaxY) {
			m_iMouseY = iMaxY;
		}

        //mouse zoom
        m_fZ += MOUSE_CAMSPEED * GetFrameTime() * m_Mouse.GetState().lZ;

        //mouse dragging
        if(m_Mouse.GetButton(1) == 1)
        {
            m_fX -= GetFrameTime() * m_Mouse.GetState().lX * MOUSE_CAMSPEED;
            m_fY += GetFrameTime() * m_Mouse.GetState().lY * MOUSE_CAMSPEED;
            //TODO: move faster when farther away from terrain
        }

        //Terrain picking test
        if(m_Mouse.GetButton(0))
        {
            float w = (float)GetWindowRect().right;
            float h = (float)GetWindowRect().bottom;

            D3DXMATRIX matProj;
            GetDevice()->GetTransform(D3DTS_PROJECTION, &matProj);

            D3DXVECTOR3 v;
            v.x =  ( ( ( 2.0f * m_iMouseX ) / w  ) - 1 ) / matProj._11;
            v.y = -( ( ( 2.0f * m_iMouseY ) / h ) - 1 ) / matProj._22;
            v.z =  1.0f;

            D3DXMATRIX matView;
            GetDevice()->GetTransform(D3DTS_VIEW, &matView);

            D3DXMATRIX m;
            D3DXVECTOR3 rayOrigin,rayDir;

            D3DXMatrixInverse( &m, NULL, &matView );

            //Transform the screen space pick ray into 3D space
            rayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
            rayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
            rayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
            rayOrigin.x = m._41;
            rayOrigin.y = m._42;
            rayOrigin.z = m._43;

            UIUnit* pUnit = UI3DObjectManager::pickUnit(rayOrigin, rayDir);

            if(pUnit)
            {
                D3DXMATRIX pMat = pUnit->GetMatrix();
                C3DObject* pObj = (C3DObject*)UI3DDebug::addSphere(0, 10.0f, 0, 25.0f, 10.0f);
                //Debug-object, added automatically to root
                m_pManager->getRootObject()->RemoveChild(pObj);
                pUnit->AddChild(pObj);
            }
            else
            {
                D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
                if(hitSquare)
                {
                    delete hitSquare;
                }
            }
        }

 
        /*
        #ifdef _DEBUG
		//print mouse coordinate changes
		_stprintf_s(	msg,
				_T("MOUSE CHANGE: x:%d y:%d z:§%d"),
				m_Mouse.GetState().lX,
				m_Mouse.GetState().lY,
				m_Mouse.GetState().lZ);
		DrawTextRow( msg, D3DXCOLOR(1.0f, 0.25f, 0.60f, 1.0f) );
 
		//print mouse coordinates
		_stprintf_s(	msg,
				_T("MOUSE COORDS: x:%d y:%d z:%d"),
				m_iMouseX,
				m_iMouseY,
                m_iMouseZ);
		DrawTextRow( msg, D3DXCOLOR(1.0f, 0.25f, 0.60f, 1.0f) );
 
		//print "virtual" mouse cursor 
		//(it's not in sync with windows cursor because windows adds it's own sensitivity
		// modifiers to mouse, use getmousepos if you want some values)
		DrawText(   m_iMouseX,
				    m_iMouseY,
				    _T("M"),
				    D3DXCOLOR(0.8f, 0.4f, 0.4f, 0.5f));
 
		for(i=0; i<m_Mouse.GetButtonCount(); i++)
		{
			if(m_Mouse.GetButton(i))
			{
				//print the mosue button index
				_stprintf_s(msg, _T("MOUSE: %d DOWN"), i);
				DrawTextRow( msg, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f) );
			}
		}
        #endif
        */
	}
}