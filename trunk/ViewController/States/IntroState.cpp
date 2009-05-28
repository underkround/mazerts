#include "IntroState.h"

//#include "../../Model/Terrain/Terrain.h"
//#include "../../Model/Terrain/AntinTerrainGenerator.h"
//#include "../../Model/Terrain/ImageTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
//#include "../../Model/Asset/AssetCollection.h"
//#include "../../Model/Asset/AssetFactory.h"
#include "../../Model/Weapon/ProjectileCollection.h"
#include "../../Model/Weapon/ExplosionCollection.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"
#include "../../Model/Common/Config.h"

// player manager
//#include "../../Model/Player/PlayerManager.h"
//#include "../../Model/Player/Fog.h"

// Camera related
#include "../Camera/SphereCamera.h"

#include "../Sound/SoundManager.h"

#include "../Asset/ParticleFactory.h"

// Controllers
#include "../Controller/UIAssetController.h"
#include "../Controller/CameraController.h"
#include "../Controller/SoundController.h"

// Definition things
#include "../../Model/Defs/DefManager.h"

#include "../UIComponent/UIContainer.h"
#include "../UIComponent/UIComponent.h"
#include "../UIComponent/DummyComponent.h"

#include "../App/TheApp.h"


//DEBUG
#include "../3DDebug/UI3DDebug.h"

// Components
#include "../UIComponent/RootContainer.h"


// TODO - for testing, remove
#include "../UIComponent/GridLayout.h"
#include "../App/GameConsole.h"

// Game initializer
#include "../../Model/RtsInitializer.h"


#include "../../Model/Console.h"

// for condition checking
#include "../../Model/Asset/Building.h"


#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 0.01f

IntroState::IntroState()
{
    loadConfigurations();

    m_pManager = NULL;
    m_pUITerrain = NULL;
    m_pApp = NULL;
    m_Created = false;
    m_Timer = -0.5f;
    m_pLogo = NULL;

    // default settings
    m_KeyGenerateNewTerrain     = 57;
    m_KeyGeneratePassability    = 2;
    m_KeyToggleWireframe        = 68;
    m_KeyTerrainDetailUp        = 49;
    m_KeyTerrainDetailDown      = 50;
    m_pMenuGui = NULL;
}

IntroState::~IntroState()
{
}

HRESULT IntroState::create(CTheApp* pApplication)
{
    HRESULT hres;
    m_pApp = pApplication;

    LPDIRECT3DDEVICE9 pDevice = m_pApp->GetDevice();

    if(!pDevice)
        return ERROR_DEVICE_NOT_AVAILABLE;

    m_pDevice = pDevice;

    //Prepare manager
    UI3DObjectManager::create(pDevice);
    m_pManager = UI3DObjectManager::getInstance();

    // Get the scenario file to load
    RtsInitializer::loadScenario("intro.ini");

    // Initialize the game (model-side)
    if(!RtsInitializer::initializeScenario())
    {
        return S_FALSE;
    }

    setCurrentPlayer(PlayerManager::getPlayer(1));

    //Initialize particlefactory
    ParticleFactory::create(pDevice);

    // UIComponents
    m_pRootContainer = RootContainer::getInstance();
    m_pRootContainer->create(pDevice, m_pApp);
    //m_pMenuGui = new MenuGui(this);

    //UI-Terrain
    m_pUITerrain = UITerrain::getInstance();
    hres = m_pUITerrain->create(pDevice, getCurrentPlayer(), false);
    if(FAILED(hres))
    {
        return hres;
    }

    hres = Cursor::getInstance()->create(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }

    // Camera
    Camera::create(pDevice);
    // configure the default (SphereCamera) camera
    Camera::getCurrent()->setPosition(64.0f, 64.0f, 0.0f);
    m_UnitCamera = NULL;

    // random unit
    /*
    ListNode<UIAsset*>* pNode = UI3DObjectManager::getInstance()->getAssetList()->headNode();
    UIUnit* pUnit = NULL;
    while (pNode)
    {
        if (pNode->item->getAsset()->getAssetType() == IAsset::UNIT)
        {
            if (!pUnit || IApplication::RandInt(0, 1)) pUnit = (UIUnit*)pNode->item;
        }
        pNode = pNode->next;
    }
    
    if (pUnit)
    {
        m_UnitCamera = new UnitCamera();
        m_UnitCamera->attach(pUnit);
        Camera::pushTop(m_UnitCamera);
    }
    */

    m_pLogo = new BasicButton(512, 128, 3, this);
    m_pLogo->setBackgroundTexture(RootContainer::getInstance()->getIconTexture(1010));
    m_pLogo->setBackgroundTextureClicked(RootContainer::getInstance()->getIconTexture(1010));
    m_pLogo->setPosition((RootContainer::getInstance()->getWidth() - 512) >> 1, (RootContainer::getInstance()->getHeight() - 128) >> 1);
    RootContainer::getInstance()->addComponent(m_pLogo);

    // Controllers
    m_UIControllers.pushHead(new SoundController());
//    m_UIControllers.pushHead(new CameraController());

    // Load configurations for the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfigurations();
        node = node->next;
    }

    // antialias
    if (Config::getInstance()->getValueAsBool("antialias", false)) 
    {
        pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
    } 

    m_Created = true;
    m_NotFinished = true;
    return S_OK;
}


void IntroState::release()
{
    // Release the game (model-side)
    RtsInitializer::release();

    //Release controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->release();
        delete node->item;
        node = m_UIControllers.removeGetNext(node);
    }

    if(m_pRootContainer)
    {
        m_pRootContainer->release();
    }

    if (m_pMenuGui)
    {
        delete m_pMenuGui;
        m_pMenuGui = NULL;
    }
    if (m_pLogo)
    {
        m_pLogo->release();
        delete m_pLogo;
        m_pLogo = NULL;
    }

    if (m_UnitCamera)
    {
        Camera::pop(m_UnitCamera);
        delete m_UnitCamera;
        m_UnitCamera = NULL;
    }

    //Release anything else left (effect-objects, debug-objects...)
    m_pManager->release();

    m_pUITerrain->release();
}


bool IntroState::update(const float frameTime)
{
    updateControls(frameTime);

    //Updating the actual game logic
    AssetCollection::updateBuildings(frameTime);
    AssetCollection::updateUnits(frameTime);
    ProjectileCollection::update(frameTime);
    ExplosionCollection::update(frameTime);

    m_pManager->getRootObject()->Update(frameTime);

    //Update UITerrain & minimap
    m_pUITerrain->update();

    //CURSOR-TEST, REMOVE
    /*
    if(MouseState::mouseButton[0])
    {
        Cursor::getInstance()->setType(Cursor::CURS_ATTACK);
        Cursor::getInstance()->setTooltip(_T("0"), 2, false);
    }
    else if(MouseState::mouseButton[1])    
    {
        Cursor::getInstance()->setType(Cursor::CURS_SELECT);
        Cursor::getInstance()->setTooltip(_T("1"), 2, false);
    }
    else
    {
        Cursor::getInstance()->setType(Cursor::CURS_NORMAL);
    }
    if(MouseState::mouseButtonReleasedBits)
    {
        Cursor::getInstance()->clearTooltip();
    }
    */

    //Update cursor position and texture
    Cursor::getInstance()->update(frameTime);

    // Update UIComponents
    //m_pRootContainer->update(frameTime);

    // Update AI
    CTimer aiTimer;
    aiTimer.Create();
    aiTimer.BeginTimer();
    PlayerManager::getInstance()->update(frameTime);
    aiTimer.EndTimer();
    float ms = aiTimer.GetElapsedSeconds() * 1000.0f;
    if (ms > 0.05f)
    {
        char* msg = new char[128];
        sprintf_s(msg, 128, "--- AI UPDATING TOOK %f ms", ms);
        Console::debug(msg);
    }

    // update fog
    m_pCurrentPlayer->getFog()->update(frameTime);
    UITerrain::getInstance()->updateFog(m_pDevice);

    m_Timer += frameTime * 0.2f;
    if (m_Timer > 1.0f) {
        m_Timer = 1.0f;
        if (m_pLogo)
        {
            m_pLogo->release();
            delete m_pLogo;
            m_pLogo = NULL;
        }
        if (!m_pMenuGui)
            m_pMenuGui = new MenuGui(this);
    }
    return m_NotFinished;
}


void IntroState::prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime)
{
    //Updates view-matrix and frustum, if necessary
    Camera::getCurrent()->update();

    //Update terrain detail level, if necessary
    float height = Camera::getCurrent()->getPosition().z;
    int detailLevel = m_pUITerrain->getDetailLevel();

    //Light is here for testing (and doesn't need to be set every frame, as it doesn't move anyway)
    D3DLIGHT9 light;
    //light.Type = D3DLIGHT_POINT;
    //light.Position = D3DXVECTOR3(300.0f, 100.0f, -100.0f);    
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Direction = D3DXVECTOR3(0.5777f, 0.5777f, 0.5777f);
    //light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    //light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    //light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    float value = m_Timer;
    if (value < 0) value = 0;
    light.Diffuse = D3DXCOLOR(value, value, value, value);
    light.Specular = D3DXCOLOR(value, value, value, value);
    light.Ambient = D3DXCOLOR(value / 5, value / 5, value / 5, value);
    light.Range = 10000.0f;    
    light.Attenuation0 = 0.001f;
    light.Attenuation1 = 0.0001f;
    light.Attenuation2 = 0.00001f;

    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, TRUE);
}


void IntroState::render(const LPDIRECT3DDEVICE9 pDevice)
{
    //Terrain needs normal backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pUITerrain->render(pDevice);

    //Antsys models need reverse backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    pDevice->SetTexture(0, NULL);
    C3DObject::setCurrentTexture(NULL);
    m_pManager->getRootObject()->Render(pDevice);

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    //if (m_Antialias) pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
    m_pRootContainer->render(pDevice);
    //if (m_Antialias) pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

    //Draw cursor
    Cursor::getInstance()->render(pDevice);
}


void IntroState::updateControls(const float frameTime)
{
    /*
     * Controls that should always be accessed (should not be stealed by
     * components) should go here.
     */

    if (KeyboardState::keyReleased[0x39] || KeyboardState::keyReleased[0x1c])
    {
        m_Timer = 1.0f;
    }

    // reload def-files on fly
    // reload config files on the fly
    if(KeyboardState::keyReleased[32]) // key: D
    {
        DefManager::getInstance()->loadConfigurations();
        loadConfigurations();
    }

    /*
     * Controls that should not be run if one of the components steals
     * the input (mouse stuff, text input) should go below.
     *
     * Update components. Components can steal the input (at least the mouse etc)
     * so if one of they do, we'll exit here
     */
    int stealFlags = m_pRootContainer->updateControls(frameTime);

    if(stealFlags)
        return;

    // Update normal controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->updateControls(frameTime);
        node = node->next;
    }

    // Terrain
    if(KeyboardState::keyReleased[m_KeyToggleWireframe])
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

    if(KeyboardState::keyReleased[m_KeyTerrainDetailUp])
    {
        unsigned char detail = m_pUITerrain->getDetailLevel();
        detail++;
        if(detail < 4)
        {
            m_pUITerrain->setDetailLevel(detail);
        }
    }

    if(KeyboardState::keyReleased[m_KeyTerrainDetailDown])
    {
        unsigned char detail = m_pUITerrain->getDetailLevel();
        detail--;
        //Wrapping
        if(detail < 4)
        {
            m_pUITerrain->setDetailLevel(detail);
        }
    }

    if(KeyboardState::keyReleased[m_KeyTerrainRedraw])
    {
        redrawTerrain();
    }
}

void IntroState::loadConfigurations()
{
    /*
     * If you want some confingurations (other than def:s) to be reloaded
     * at runtime, declare it here.
     * This method is run on startup, and possibly again in runtime with
     * hotkey.
     * Make this the main place to call all necessary updates?
     */

    Config & c = * Config::getInstance();
    c.setFilename("../data/controls.ini");
    c.readFile();

    // optional local override -file (not in source control)
    c.setFilename("../data/local.ini");
    c.readFile();

    c.updateInt("key generate new terrain",         m_KeyGenerateNewTerrain);
    c.updateInt("key generate passability terrain", m_KeyGeneratePassability);
    c.updateInt("key toggle wireframe",             m_KeyToggleWireframe);
    c.updateInt("key terrain detail up",            m_KeyTerrainDetailUp);
    c.updateInt("key terrain detail down",          m_KeyTerrainDetailDown);
    c.updateInt("key terrain redraw",               m_KeyTerrainRedraw);

    c.getValueAsFloat("condition update interval", 5.0f);

    m_pRootContainer->loadConfigurations();

    // update configurations of the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfigurations();
        node = node->next;
    }
}

void IntroState::redrawTerrain()
{
        UITerrain*ut = UITerrain::getInstance();
        PathFinderMaster* pf = PathFinderMaster::getInstance();
        pf->stop();
        pf->wait();
        ut->create(m_pDevice, getCurrentPlayer(), false);
        pf->start();
}

void IntroState::onButtonClick(BasicButton* pSrc)
{
    switch (pSrc->getId())
    {
    case 0:
        m_NotFinished = false;
        break;
    case 1:
        m_pApp->credits();
        break;
    case 2:
        m_pApp->Close();
        break;
    case 3:
        m_Timer = 1.0f;
        break;
    }
}


void IntroState::onButtonAltClick(BasicButton* pSrc)
{
}
