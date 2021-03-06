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
#include "../States/IntroState.h"
#include "../States/ImageState.h"
#include "../Sound/SoundManager.h"

#include "../../Model/Console.h"
#include "../Asset/PlayerColors.h"

CTheApp::CTheApp(void)
{
    m_Help = false;

    m_TextRow = 0;
    m_pCurrentState = NULL;
    m_pStates = NULL;
    handleConfig();
    m_CatchInput = false;
    ::CoInitialize(NULL);
    m_DontClear = false;
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
    Config& c = *Config::getInstance();

    if(!c.getValueAsInt("skip menu", false))
    {
        m_pStates->pushTail(new ImageState(1010, 512, 128));
        m_pStates->pushTail(new IntroState());
    }
    if(!c.getValueAsInt("skip game", false))
    {
        m_pStates->pushTail(new ImageState(1017, 512, 64));
        m_pStates->pushTail(new GameState());
    }

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
        if (pChild->isCreated())
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
    //pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF444444, 1.0f, 0);
    if (!m_DontClear)
        pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0);
    
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
        Input::update(frameTime);
    }

    //Sound updates
    SoundManager::update(frameTime);

    
    timer1->BeginTimer();
    //State update
    if(!m_pCurrentState->update(frameTime))
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

        // Call those that use this for text rendering
        Cursor::getInstance()->renderText(this);
        //TextComponent::renderText();

/*
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
*/
        if(m_CatchInput)
        {
            GameConsole & co = *GameConsole::getInstance();
            DrawText(100, 400, _T("konsoli p��ll�"), 0xFFFFFFFF);
            TCHAR msg[128];
            _stprintf_s(msg, _T("%s"), (LPCTSTR)co.output());
            DrawText(100, 450, msg, 0xFFFFFFFF);
        }

        /** HAXHAX: just dump the model's console buffer to the screen */
        if(m_Help)
        {
            if(Console::getInstance()->getCurrentSize() > 0) {
                int consoleTextX = 350;
                int consoleTextY = 100;
                TCHAR msg2[128];
                _stprintf_s(msg2, _T("-- model-konsoli, rivej�: %d --"), Console::getInstance()->getCurrentSize());
                DrawText(consoleTextX, consoleTextY, msg2, 0xFFFFFFFF);
                ListNode<ConsoleMsg*>* node = Console::getInstance()->getTail();
                const int msgMaxSize = 128;
                wchar_t msgWTF_CHAR[msgMaxSize+1];
                int msgColor = 0xFFFFFFFF;
                while(node) {
                    consoleTextY += GetTextHeight();
                    switch(node->item->level) {
                        case MSG_LEVEL_DEBUG:
                            msgColor = 0xFFFFFFFF;
                            break;
                        case MSG_LEVEL_NOTIFY:
                            msgColor = 0xFFCCFF00;
                            break;
                        case MSG_LEVEL_ERROR:
                            msgColor = 0xFFFF2222;
                            break;
                    }
                    JString* msgSrc = node->item->msg;
                    int msgSize = msgSrc->GetLength();
                    if(msgSize > msgMaxSize)
                        msgSize = msgMaxSize;
                    MultiByteToWideChar(CP_ACP, 0, msgSrc->GetBuffer(), -1, msgWTF_CHAR, msgSize);
                    msgWTF_CHAR[msgSize] = '\0';
                    //DrawTextRow(msgWTF_CHAR, msgColor);
                    DrawText(consoleTextX, consoleTextY, msgWTF_CHAR, msgColor);
                    node = node->prev;
                }
            }
        }

#ifdef _DEBUG
        TCHAR text[100];
        _stprintf_s(text, _T("FPS: %.2f  Frametime: %.2fms"), (1.0f / GetFrameTime()), GetFrameTime() * 1000.0f);
        DrawText(500, 0, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Update: %.2fms  Render: %.2fms"), timer1->GetElapsedSeconds() * 1000.0f, time * 1000.0f);
        DrawText(500, 20, text, 0xFFFFFFFF);

        _stprintf_s(text, _T("Pathfinders  Running:%d  Waiting: %d"), PathFinderMaster::getRunningAmount(), PathFinderMaster::getWaitingAmount());
        DrawText(500, 40, text, 0xFFFFFFFF);        

        for(int i=1; i <= PlayerManager::getPlayerCount();++i)
        {
            if (PlayerManager::isCreated()) {
                const int consumed = PlayerManager::getInstance()->getPlayer(i)->getEnergyConsumed();
                const int produced = PlayerManager::getInstance()->getPlayer(i)->getEnergyProduced();
                const int total = produced - consumed;
                char sign = '+';
                if (total <= 0) sign = ' ';
                _stprintf_s(text, _T("Player %d ore: %d  Energy: %c%d (%d/%d)"), i, PlayerManager::getInstance()->getPlayer(i)->getOre(), sign, total, consumed, produced);
                DrawText(500, (60+i*10), text, PLAYERCOLORS[i]);
            }
        }
#endif
#ifndef _DEBUG
            if (PlayerManager::isCreated())
            {
                TCHAR text[100];
                const int consumed = PlayerManager::getInstance()->getPlayer(1)->getEnergyConsumed();
                const int produced = PlayerManager::getInstance()->getPlayer(1)->getEnergyProduced();
                const int total = produced - consumed;
                char sign = '+';
                if (total <= 0) sign = ' ';
                _stprintf_s(text, _T("Ore: %d    Energy: %c%d (%d/%d)"), PlayerManager::getInstance()->getPlayer(1)->getOre(), sign, total, consumed, produced);
                DrawText(500, (20), text, PLAYERCOLORS[1]);
            }
#endif

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
    c.setFilename("../data/meshes.ini");
    c.readFile();
    c.setFilename("../data/controls.ini");
    c.readFile();

    // read & set mouse buttons
    int tmp = c.getValueAsInt("mouse first button", -1);
    if(tmp >= 0)
        MouseState::mouseFirstButton = tmp;
    tmp = c.getValueAsInt("mouse second button", -1);
    if(tmp >= 1)
        MouseState::mouseSecondButton = tmp;

    //NOTE! values used here are loaded in program Main() method
    bool sounds = c.getValueAsBool("sound enabled", true);
    bool music  = c.getValueAsBool("music enabled", true);
    int svol    = c.getValueAsInt("master volume",  0);
    int mvol    = c.getValueAsInt("music volume",   0);
    SoundManager::setSoundsEnabled(sounds);
    SoundManager::setMusicEnabled(music);
    SoundManager::setMasterVolume(svol);
    SoundManager::setMusicVolume(mvol);
}

void CTheApp::gameover()
{
    Close();
    //m_pStates->pushTail(new ImageState(1010, 512, 128));
    //m_pStates->pushTail(new IntroState());
}
