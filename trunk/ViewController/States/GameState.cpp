#include "GameState.h"

#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/AntinTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../../Model/Asset/AssetFactory.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"
#include "../../Model/Common/Config.h"

// Camera related
#include "../Camera/SphereCamera.h"

#include "../Sound/SoundManager.h"

// Controllers
#include "../Controller/UIAssetController.h"
#include "../Controller/CameraController.h"
#include "../Controller/SoundController.h"

//DEBUG
#include "../3DDebug/UI3DDebug.h"


#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 0.01f

GameState::GameState()
{
    loadConfiguration();

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

    //Prepare manager
    UI3DObjectManager::create(pDevice);
    m_pManager = UI3DObjectManager::getInstance();

    //Model-terrain
    Terrain* pTerrain = Terrain::getInstance();
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512);    
    pTerrain->initialize(pGenerator);

    pTerrain->setWaterLevel(50); // TODO: imho this is for the terrain generator to decide! move it there?

    // initialize asset collection
    AssetCollection::create(pTerrain->getSize());

    //TEST
    for(int i = 0; i < 50; i++)
    {
        AssetFactory::createUnit(NULL, 0, (i * 4), ((i % 5) * 4));
    }


    //UI-Terrain
    hres = UITerrain::create(pDevice);
    if(FAILED(hres))
    {
        return hres;
    }

    m_pUITerrain = UITerrain::getInstance();                       

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


     //Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    //Camera
    Camera::create(pDevice);
    m_pCamera = new SphereCamera();
    m_pCamera->setPosition(127.0f, 127.0f, -200.0f);

    // Set default camera to the soundmanager
    SoundManager::setDefaultCamera(m_pCamera);

    // Controllers
    m_UIControllers.pushHead(new UIAssetController(pDevice, m_pCamera, &m_Selector));
    m_UIControllers.pushHead(new SoundController(m_pCamera));
    m_UIControllers.pushHead(new CameraController(m_pCamera));
    // Load configurations for the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfiguration(true);
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

    // Remove our camera from sound manager
    SoundManager::setDefaultCamera(NULL);

    //The children of the Object Manager root object are listeners to model-side
    //AssetCollection, and will be released via listener-interface, but they have
    //to be released first (order matters!)
    AssetCollection::releaseAll();

    //Release anything else left (effect-objects, debug-objects...)
    m_pManager->release();
    
    m_pUITerrain->release();
}


bool GameState::update(const float frameTime)
{
    updateControls(frameTime);

    //Updating the actual game logic    
    AssetCollection::updateUnits(frameTime);
    m_pManager->getRootObject()->Update(frameTime);

    //Update minimap
    m_pUITerrain->getMiniMap()->updateUnits(m_pManager->getUnitList(), frameTime);
    m_pUITerrain->getMiniMap()->updateCamera(m_pDevice);

    //CURSOR-TEST, REMOVE
    if(MouseState::mouseButton[0])
    {
       Cursor::getInstance()->setType(Cursor::CURS_ATTACK);
    }
    else if(MouseState::mouseButton[1])    
    {
        Cursor::getInstance()->setType(Cursor::CURS_SELECT);
    }
    else
    {
        Cursor::getInstance()->setType(Cursor::CURS_NORMAL);
    }

    //Update cursor position and texture
    Cursor::getInstance()->update();

    //Keep running
    return true;
}


void GameState::prepareForRender(const LPDIRECT3DDEVICE9 pDevice, const float frameTime)
{
    //Updates view-matrix and frustum, if necessary
    m_pCamera->update();

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
    //pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pManager->getRootObject()->Render(pDevice);

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_Selector.render(pDevice);    

    //Draw cursor
    Cursor::getInstance()->render(pDevice);
}


void GameState::updateControls(const float frameTime)
{
    // Controllers
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
        UITerrain::create(m_pDevice);
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

/*
    //Terrain picking test
    //if(MouseState::mouseButton[m_KeyMousePickButton])
    if(MouseState::mouseButtonReleased[m_KeyMousePickButton])
    {
        D3DXMATRIX matProj;
        m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

        D3DXMATRIX matView;
        m_pDevice->GetTransform(D3DTS_VIEW, &matView);

        D3DXVECTOR3 rayOrigin,rayDir;

        MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

        UIUnit* pUnit = UI3DObjectManager::pickUnit(rayOrigin, rayDir);

        // if unit selected, give target to it
        if(m_tmpSelectedUnit)
        {
            // TODO: remove after testing single unit moving
            D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
            if(pUnit)
            {
                m_tmpSelectedUnit->getUnit()->getMovingLogic()->setTarget(new Target(pUnit->getUnit()));
                m_tmpSelectedUnit = NULL;
            }
            else if(hitSquare)
            {
                unsigned short targetX = (unsigned short)hitSquare->x;
                unsigned short targetY = (unsigned short)hitSquare->y;
                m_tmpSelectedUnit->getUnit()->getMovingLogic()->setTarget(new Target(targetX, targetY, true));
                // clear selection
                m_tmpSelectedUnit = NULL;
                //m_Selector.setPoint(D3DXVECTOR2(hitSquare->x, hitSquare->y));    
            }

            delete hitSquare;
        }

        // try to pick unit to control
        else if(pUnit)
        {
            D3DXMATRIX pMat = pUnit->GetMatrix();
            C3DObject* pObj = (C3DObject*)UI3DDebug::addSphere(0, 2.0f, 0.0f, 1.0f, 1.0f);
            //Debug-object, added automatically to root
            m_pManager->getRootObject()->RemoveChild(pObj);
            pUnit->AddChild(pObj);
            SoundManager::playSound(SoundManager::READY, 0.1f, (D3DXVECTOR3)&pUnit->GetMatrix()._41, m_pCamera);

            m_tmpSelectedUnit = pUnit; // TODO: remove after testing single unit moving
        }
    }
    else if(MouseState::mouseButtonReleased[m_KeyMousePickButton])
    {       
        m_Selector.buttonUp();
    }
*/

}

void GameState::loadConfiguration()
{
    Config & c = * Config::getInstance();
    c.setFilename("controls.ini");
    c.readFile();

    c.updateInt("key generate new terrain",         m_KeyGenerateNewTerrain);
    c.updateInt("key generate passability terrain", m_KeyGeneratePassability);
    c.updateInt("key toggle wireframe",             m_KeyToggleWireframe);
    c.updateInt("key terrain detail up",            m_KeyTerrainDetailUp);
    c.updateInt("key terrain detail down",          m_KeyTerrainDetailDown);

    // update configurations of the controllers
    ListNode<IUIController*>* node = m_UIControllers.headNode();
    while(node)
    {
        node->item->loadConfiguration(true);
        node = node->next;
    }
}
