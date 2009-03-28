/**
 * TheApp.cpp source file
 * 
 * Concrete game application class
 */

#include "TheApp.h"
#include "../Input/Input.h"
#include "../../Model/Common/Config.h"

//DEBUG-stuff
#include "../3DDebug/UI3DDebug.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../States/GameState.h"
#include "../Sound/SoundManager.h"

CTheApp::CTheApp(void)
{
    m_Help = true;

    m_TextRow = 0;
    m_pCurrentState = NULL;
    m_pStates = NULL;
    handleConfig();
    m_CatchInput = false;
    ::CoInitialize(NULL);
}


CTheApp::~CTheApp(void)
{
}


HRESULT CTheApp::OnCreate(void)
{
    HRESULT hres;

    //Initialize input-systems
    hres = Input::create(this);    
    if(FAILED(hres))
    {
        ::MessageBox( GetWindow(), _T("failed to init input engine."), _T("error"), MB_OK);
    }

    //Initialize sound system
    hres = SoundManager::create(this);
    if(FAILED(hres))
    {
        ::MessageBox( GetWindow(), _T("failed to init sound engine."), _T("error"), MB_OK);
        return hres;
    }

    //Set device to debug-helper
    UI3DDebug::setDevice(GetDevice());

    //Fill starting states to list (TODO: should be Intro & menu, but for now just GameState is done)
    m_pStates = new DoubleLinkedList<IState*>();
    m_pStates->pushTail(new GameState());

    //Get first state
    hres = nextState();

    if(FAILED(hres))
    {
        return hres;
    }

    return S_OK;
}


void CTheApp::OnRelease(void)
{
    //Destroy current state
    if(m_pCurrentState)
    {
        m_pCurrentState->release();
        delete m_pCurrentState;
    }

    //Clear states list
    ListNode<IState*>* node = m_pStates->headNode();    

    while(node)
    {
        IState* pChild = node->item;
        pChild->release();
        delete pChild;

        node = node->next;
    }

    m_pStates->release();

    Input::release();
    SoundManager::release();
}


void CTheApp::OnFlip(void)
{
    LPDIRECT3DDEVICE9 pDevice = GetDevice();

    // clear the background & z-buffer
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF444444, 1.0f, 0);

    
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
    const float frameTime = GetFrameTime();


    //Input updates
    if(!m_CatchInput) {
        Input::update();
    }

    //Sound updates
    SoundManager::update(frameTime);

    
    timer1->BeginTimer();
    //State update
    if(m_pCurrentState->update(frameTime) == false)
    {
        HRESULT hres = nextState();

        if(FAILED(hres))
        {
            ::MessageBox(GetWindow(), _T("nextState returned error code on TheApp.cpp OnFlip"), _T("FATAL ERROR"), MB_OK);
            Close();
        }

        return;
    }
    
    timer1->EndTimer();


    //Tell state to prepare for rendering
    m_pCurrentState->prepareForRender(pDevice, frameTime);
    
    //Rendering operations
    timer2->BeginTimer();
    if (SUCCEEDED(pDevice->BeginScene()))
    {
        //Text drawing
        BeginText();

       
        if(m_Help)
        {
            DrawTextRow(_T("Drag with right mouse button pressed to pan view"), 0xFFFFFFFF);
            DrawTextRow(_T("Drag with third mouse button pressed to rotate view"), 0xFFFFFFFF);
            DrawTextRow(_T("Mouse scroll wheel zooms"), 0xFFFFFFFF);
            DrawTextRow(_T("Use LEFT, RIGHT, UP, DOWN, A and Z to move camera"), 0xFFFFFFFF);
            DrawTextRow(_T("Use space to generate new terrain"), 0xFFFFFFFF);
            DrawTextRow(_T("1 adds the texture with passability data"), 0xFFFFFFFF);
            DrawTextRow(_T("TAB switches fillmode (solid/wireframe)"), 0xFFFFFFFF);
            DrawTextRow(_T("M & N change terrain detail level"), 0xFFFFFFFF);
            DrawTextRow(_T("F5 toggles sound, f6 toggles music, numpad +/- changes master volume"), 0xFFFFFFFF);
            DrawTextRow(_T("F1 hides/shows this help message"), 0xFFFFFFFF);
        }
        if(m_CatchInput)
        {
            GameConsole & co = *GameConsole::getInstance();
            DrawText(100, 400, _T("konsoli päällä"), 0xFFFFFFFF);
            TCHAR msg[128];
			_stprintf_s(msg, _T("%s"), (LPCTSTR)co.output());
            
            DrawText(100, 450, msg, 0xFFFFFFFF);
        }

        TCHAR text[100];
        _stprintf_s(text, _T("FPS: %.2f  Frametime: %.2fms"), (1.0f / GetFrameTime()), GetFrameTime() * 1000.0f);
        DrawText(500, 0, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Update: %.2fms  Render: %.2fms"), timer1->GetElapsedSeconds() * 1000.0f, time * 1000.0f);
        DrawText(500, 20, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Pathfinders  Running:%d  Waiting: %d"), PathFinderMaster::getRunningAmount(), PathFinderMaster::getWaitingAmount());
        DrawText(500, 60, text, 0xFFFFFFFF);        

        m_TextRow = 0;
        EndText();

        
        //Render state
        m_pCurrentState->render(pDevice);


        pDevice->EndScene();
    }
    timer2->EndTimer();
    time = timer2->GetElapsedSeconds();
}

HRESULT CTheApp::nextState()
{
    if(m_pCurrentState)
    {
        //Exit current state
        m_pCurrentState->release();
        delete m_pCurrentState;
    }

    //Pop new state
    m_pCurrentState = m_pStates->popHead();

    if(!m_pCurrentState->isCreated())
    {
        HRESULT hres = m_pCurrentState->create(this);
        if(FAILED(hres))
        {
            return hres;
        }
    }

    return S_OK;
}


HRESULT CTheApp::pushState(IState* pState)
{
    m_pStates->pushHead(m_pCurrentState);

    m_pCurrentState = pState;

    if(!m_pCurrentState->isCreated())
    {
        HRESULT hres = m_pCurrentState->create(this);
        
        if(FAILED(hres))
        {
            return hres;
        }
    }
    
    return S_OK;
}


void CTheApp::DrawTextRow(LPCTSTR text, DWORD color)
{
    DrawText(0, m_TextRow, text, color);
    m_TextRow += GetTextHeight();
}


void CTheApp::OnKeyDown(DWORD dwKey)
{
    if(dwKey == VK_ESCAPE)
    {
        Close();
    }

    if(dwKey == VK_F1)
    {
        m_Help = !m_Help;
    }

    if(dwKey == VK_F2)
    {
        if(m_CatchInput) {
            m_CatchInput = false;
        }
        else {
            m_CatchInput = true;
        }
    }
}

void CTheApp::OnChar(TCHAR c)
{
    if(m_CatchInput)
    {
        GameConsole & co = *GameConsole::getInstance();
        co.input(c);
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

void CTheApp::handleConfig()
{
    Config& c = *Config::getInstance();
    c.setFilename("../data/config.ini");
    c.readFile();
    c.setFilename("../data/meshes.ini");
    c.readFile();
    c.setFilename("../data/controls.ini");
    c.readFile();

    bool sounds = c.getValueAsBool("sound enabled", true);
    bool music  = c.getValueAsBool("music enabled", true);
    int svol    = c.getValueAsInt("master volume",  0);
    int mvol    = c.getValueAsInt("music volume",   0);
    SoundManager::setSoundsEnabled(sounds);
    SoundManager::setMusicEnabled(music);
    SoundManager::setMasterVolume(svol);
    SoundManager::setMusicVolume(mvol);
}
