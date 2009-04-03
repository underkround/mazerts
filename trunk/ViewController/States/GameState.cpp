#include "GameState.h"

#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/AntinTerrainGenerator.h"
#include "../../Model/Terrain/ImageTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../../Model/Asset/AssetCollection.h"
#include "../../Model/Asset/AssetFactory.h"
#include "../../Model/Weapon/ProjectileCollection.h"
#include "../../Model/Weapon/ExplosionCollection.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"
#include "../../Model/Common/Config.h"

// player manager
#include "../../Model/Player/PlayerManager.h"

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

//DEBUG
#include "../3DDebug/UI3DDebug.h"

// Components
//#include "../UIComponent/GridPanel.h"
#include "../UIComponent/RootContainer.h"

#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 0.01f

GameState::GameState()
{
    loadConfigurations();

    m_pManager = NULL;
    m_pUITerrain = NULL;
    m_pApp = NULL;
    m_Created = false;

    // default settings
    m_KeyGenerateNewTerrain     = 57;
    m_KeyGeneratePassability    = 2;
    m_KeyToggleWireframe        = 15;
    m_KeyTerrainDetailUp        = 49;
    m_KeyTerrainDetailDown      = 50;
}

GameState::~GameState()
{
}

HRESULT GameState::create(ID3DApplication* pApplication)
{
    HRESULT hres;

    m_pApp = pApplication;

    LPDIRECT3DDEVICE9 pDevice = m_pApp->GetDevice();

    if(!pDevice)
    {
        return ERROR_DEVICE_NOT_AVAILABLE;
    }

    m_pDevice = pDevice;

    // Load definition files
    DefManager::getInstance()->loadConfigurations();

    //Prepare manager
    UI3DObjectManager::create(pDevice);
    m_pManager = UI3DObjectManager::getInstance();

    //Model-terrain
    Terrain* pTerrain = Terrain::getInstance();
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512);
    //ImageTerrainGenerator* pGenerator = new ImageTerrainGenerator("../data/terrains/murgo.bmp");// :P

    // for zemm's slow computer's local override
    if(!Config::getInstance()->getValueAsBool("debug skip terrain generating", false))
        pTerrain->initialize(pGenerator);

    // initialize asset collection
    AssetCollection::create(pTerrain->getSize());
    //Initialize projectile collection
    ProjectileCollection::create();
    //Initialize explosion collection and set callback-method
    ExplosionCollection::create();
    ExplosionCollection::setCallBack(&ParticleFactory::createExplosion);

    //UI-Terrain
    m_pUITerrain = UITerrain::getInstance();
    hres = m_pUITerrain->create(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }    

    //Initialize particlefactory
    ParticleFactory::create(pDevice);

    // UIComponents
    m_pRootContainer = RootContainer::getInstance();
    m_pRootContainer->create(pDevice, m_pApp);

    // Initialize player manager
    PlayerManager::create(4);

    //TEST

    for(int i = 0; i < 50; i++)    
    {
        if(i < 13)
            AssetFactory::createUnit(PlayerManager::getPlayer(1), (rand() % 3) + 1, 40+((i / 5) * 5), 20+(i % 5) * 5);
        else if(i < 25)
            AssetFactory::createUnit(PlayerManager::getPlayer(2), (rand() % 3) + 1, 60+((i / 5) * 5), 20+(i % 5) * 5);
        else if(i < 38)
            AssetFactory::createUnit(PlayerManager::getPlayer(3), (rand() % 3) + 1, 80+((i / 5) * 5), 20+(i % 5) * 5);
        else
            AssetFactory::createUnit(PlayerManager::getPlayer(4), (rand() % 3) + 1, 100+((i / 5) * 5), 20+(i % 5) * 5);
    }

    for(int i = 0; i < 10; i++)
    {
        AssetFactory::createBuilding(PlayerManager::getPlayer(IApplication::RandInt(1, 2)), 51, 20+(i * 20), 100+(i % 5) * 10);
    }


    //Selector
    hres = m_Selector.create(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }

    hres = Cursor::getInstance()->create(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }

    // Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    // Camera
    Camera::create(pDevice);
    // configure the default (SphereCamera) camera
    Camera::getCurrent()->setPosition(127.0f, 127.0f, 0.0f);

    // Controllers
    m_UIControllers.pushHead(new UIAssetController(pDevice, &m_Selector));
    m_UIControllers.pushHead(new SoundController());
    m_UIControllers.pushHead(new CameraController());

    // Load configurations for the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfigurations();
        node = node->next;
    }

    m_Created = true;

    return S_OK;
}


void GameState::release()
{
    //Tell the PathFinder-thread to stop and wait for it to finish
    if(PathFinderMaster::getInstance()->isRunning())
    {
        PathFinderMaster::getInstance()->stop();
        PathFinderMaster::getInstance()->wait();
    }

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

    // Clear the camera-stack
    //while(Camera::releaseBack()) { }

    //The children of the Object Manager root object are listeners to model-side
    //AssetCollection, and will be released via listener-interface, but they have
    //to be released first (order matters!)
    AssetCollection::releaseAll();

    //Release anything else left (effect-objects, debug-objects...)
    m_pManager->release();

    // release player manager
    PlayerManager::release();
    
    m_pUITerrain->release();
}


bool GameState::update(const float frameTime)
{
    updateControls(frameTime);

    //Updating the actual game logic
    AssetCollection::updateBuildings(frameTime);
    AssetCollection::updateUnits(frameTime);
    ProjectileCollection::update(frameTime);
    ExplosionCollection::update(frameTime);

    m_pManager->getRootObject()->Update(frameTime);

    //Update minimap
    m_pUITerrain->getMiniMap()->updateAssets(m_pManager->getAssetList(), frameTime);
    m_pUITerrain->getMiniMap()->updateCamera(m_pDevice);

    //CURSOR-TEST, REMOVE
    if(MouseState::mouseButton[0])
    {
        Cursor::getInstance()->setType(Cursor::CURS_ATTACK);
        Cursor::getInstance()->setTooltip(_T("0"), 2);
    }
    else if(MouseState::mouseButton[1])    
    {
        Cursor::getInstance()->setType(Cursor::CURS_SELECT);
        Cursor::getInstance()->setTooltip(_T("1"), 2);
    }
    else
    {
        Cursor::getInstance()->setType(Cursor::CURS_NORMAL);
    }
    if(MouseState::mouseButtonReleasedBits)
    {
        Cursor::getInstance()->clearTooltip();
    }

    //Update cursor position and texture
    Cursor::getInstance()->update(frameTime);

    // Update UIComponents
    m_pRootContainer->update(frameTime);

    //Keep running
    return true;
}


void GameState::prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime)
{
    //Updates view-matrix and frustum, if necessary
//    m_pCamera->update();
    Camera::getCurrent()->update();

    //Update terrain detail level, if necessary
    float height = Camera::getCurrent()->getPosition().z;
    int detailLevel = m_pUITerrain->getDetailLevel();
    
    /*if(height > -200.0f)
    {        
        m_pUITerrain->setDetailLevel(0);
    }
    else if(height > -350.0f)
    {
        m_pUITerrain->setDetailLevel(1);
    }
    else if(height > -500.0f)
    {
        m_pUITerrain->setDetailLevel(2);
    }
    else if(detailLevel != 3)    
    {
        m_pUITerrain->setDetailLevel(3);
    }*/

    //Light is here for testing (and doesn't need to be set every frame, as it doesn't move anyway)
    D3DLIGHT9 light;
    //light.Type = D3DLIGHT_POINT;
    //light.Position = D3DXVECTOR3(300.0f, 100.0f, -100.0f);    
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Direction = D3DXVECTOR3(0.5777f, 0.5777f, 0.5777f);
    light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    light.Range = 10000.0f;    
    light.Attenuation0 = 0.001f;
    light.Attenuation1 = 0.0001f;
    light.Attenuation2 = 0.00001f;

    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, TRUE);
}


void GameState::render(const LPDIRECT3DDEVICE9 pDevice)
{

    //Terrain needs normal backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pUITerrain->render(pDevice);

    //Antsys models need reverse backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pManager->getRootObject()->Render(pDevice);

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_Selector.render(pDevice);    

    m_pRootContainer->render(pDevice);

    //Draw cursor
    Cursor::getInstance()->render(pDevice);
}


void GameState::updateControls(const float frameTime)
{
    /*
     * Controls that should always be accessed (should not be stealed by
     * components) should go here.
     */

    // reload def-files on fly
    // reload config files on the fly
    if(KeyboardState::keyReleased[32]) // key: D
    {
        DefManager::getInstance()->loadConfigurations();
        loadConfigurations();
    }

    // cursor tooltip test
    if(KeyboardState::keyReleased[0x25]) // key: K
    {
        Cursor::getInstance()->setTooltip(_T("tooltip with key!"), 5);
    }

    // change the components, testing stuff
    if(KeyboardState::keyReleased[0x3D]) // key: F3
    {
        //m_pRootContainer->setTransparent(!m_pRootContainer->isTransparent());
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

    if(KeyboardState::keyReleased[m_KeyGenerateNewTerrain])
    {
        //The Right Way(tm) to create new terrain
        PathFinderMaster* pMaster = PathFinderMaster::getInstance();
        pMaster->stop();
        pMaster->wait();

        Terrain::getInstance()->initialize();
        UITerrain::getInstance()->create(m_pDevice);
        m_pUITerrain = UITerrain::getInstance();

        pMaster->start();
    }

    if(KeyboardState::keyDown[m_KeyGeneratePassability])
    {
        m_pUITerrain->createPassabilityTexture(m_pDevice);
    }

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

}

void GameState::loadConfigurations()
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

    m_pRootContainer->loadConfigurations();

    // update configurations of the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfigurations();
        node = node->next;
    }
}
