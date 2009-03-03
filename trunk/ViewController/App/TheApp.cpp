/**
 * TheApp.cpp source file
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
            DrawTextRow(_T("Use LEFT, RIGHT, UP, DOWN, A and Z to move camera"), 0xFFFFFFFF);
            DrawTextRow(_T("Use LEFT, RIGHT, UP, DOWN, A and Z to move camera"), 0xFFFFFFFF);
            DrawTextRow(_T("Press ESC to quit"), 0xFFFFFFFF);
            DrawTextRow(_T("Use space to generate new terrain"), 0xFFFFFFFF);
            DrawTextRow(_T("1 replaces the texture with passability data"), 0xFFFFFFFF);
            DrawTextRow(_T("TAB switches fillmode (solid/wireframe)"), 0xFFFFFFFF);
            DrawTextRow(_T("Enter forces the unit to get a new path"), 0xFFFFFFFF);
            DrawTextRow(_T("F1 hides/shows this help message"), 0xFFFFFFFF);            
            DrawTextRow(_T("M & N change terrain detail level"), 0xFFFFFFFF);            

        }

        TCHAR text[100];
        _stprintf_s(text, _T("FPS: %.2f"), (1.0f / GetFrameTime()));
        DrawText(500, 0, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Detail level: %d"), pUITerrain->getDetailLevel());
        DrawText(500, 40, text, 0xFFFFFFFF);

		UpdateKeyboard();
		UpdateMouse();

        m_TextRow = 0;
        EndText();

        pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        pUITerrain->render(pDevice);
                   
        m_pManager->getRootObject()->Render(pDevice);

        pDevice->EndScene();
    }
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
    const float camspeed = 20.0f;

    // read keys to rotate the quad
    if (dwKey == VK_LEFT)
    {
        m_fX -= camspeed * GetFrameTime();
    }
    else if (dwKey == VK_RIGHT)
    {
        m_fX += camspeed * GetFrameTime();
    }
    
    if (dwKey == VK_UP)
    {
        m_fY += camspeed * GetFrameTime();
    }
    else if (dwKey == VK_DOWN)
    {
        m_fY -= camspeed * GetFrameTime();
    }
    
    if (dwKey == 'A')
    {
        m_fZ += camspeed * GetFrameTime();
    }
    else if (dwKey == 'Z')
    {
        m_fZ -= camspeed * GetFrameTime();
    }

    if(dwKey == VK_SPACE)
    {
        Terrain::getInstance()->initialize();
        UITerrain::create(GetDevice());
        pUITerrain = UITerrain::getInstance();
    }
    if(dwKey == '1')
    {
        pUITerrain->createPassabilityTexture(GetDevice());
    }


    if(dwKey == VK_F1)
    {
        m_Help = !m_Help;
    }

    if(dwKey == VK_TAB)
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
        if(dwKey == 'N')
        {
            unsigned char detail = pUITerrain->getDetailLevel();
            detail++;
            if(detail < 4)
            {
                pUITerrain->setDetailLevel(detail);
            }
        }

        if(dwKey == 'M')
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
	TCHAR msg[256];
 
	if(SUCCEEDED(m_Keyboard.Update()))
	{
		for(i=0; i<m_Keyboard.GetButtonCount(); i++)
		{
			if(m_Keyboard.GetButton(i))
			{
                #ifdef _DEBUG
				_stprintf_s(msg, _T("KEYBOARD: %d DOWN"), i);
				DrawText(	0,
						m_TextRow,
						msg,
						D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f));
				m_TextRow += GetTextHeight();
                #endif
			}
		}
	}
	m_TextRow += GetTextHeight();
}
 
void CTheApp::UpdateMouse(void)
{
	//read the mouse data
	DWORD i;
	TCHAR msg[256];
 
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
	}
}