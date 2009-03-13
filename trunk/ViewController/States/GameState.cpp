#include "GameState.h"

#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/AntinTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../../Model/Asset/AssetFactory.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"
#include "../../Model/Common/Config.h"

//Camera related
#include "../Camera/SphereCamera.h"
#include "../Terrain/TerrainIntersection.h"

#include "../Sound/SoundManager.h"


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

    m_tmpSelectedUnit = NULL; // TEMP
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
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512, true);
    pTerrain->initialize(pGenerator);

    pTerrain->setWaterLevel(50);

    //TEST
    for(int i = 0; i < 5; i++)
    {
        AssetFactory::createUnit(NULL, 0, m_pApp->RandInt(0, pTerrain->getSize()), m_pApp->RandInt(0, pTerrain->getSize()));
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

     //Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    //Camera
    Camera::create(pDevice);
    m_pCamera = new SphereCamera();
    m_pCamera->setPosition(127.0f, 127.0f, -200.0f);

    m_Created = true;

    // Play music
    SoundManager::playMusic(SoundManager::BACKGROUND, true);

    return S_OK;
}

void GameState::release()
{
    //Tell the PathFinder-thread to stop and wait for it to finish
    PathFinderMaster::getInstance()->stop();
    PathFinderMaster::getInstance()->wait();

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

    m_Selector.render(pDevice);
      
    //Antsys models need reverse backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    //pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pManager->getRootObject()->Render(pDevice);
}


void GameState::updateControls(const float frameTime)
{
    if (KeyboardState::keyDown[m_KeyCameraPanUp])
    {
        m_pCamera->move(KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanDown])
    {
        m_pCamera->move(-KEYBOARD_CAMSPEED * frameTime, 0, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraPanRight])
    {
        m_pCamera->move(0, KEYBOARD_CAMSPEED * frameTime, 0);
    }
    else if (KeyboardState::keyDown[m_KeyCameraPanLeft])
    {
        m_pCamera->move(0, -KEYBOARD_CAMSPEED * frameTime, 0);
    }
    
    if (KeyboardState::keyDown[m_KeyCameraZoomIn])
    {
        m_pCamera->move(0, 0, KEYBOARD_CAMSPEED * frameTime);
    }
    else if (KeyboardState::keyDown[m_KeyCameraZoomOut])
    {
        m_pCamera->move(0, 0, -KEYBOARD_CAMSPEED * frameTime);
    }

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

    //Reset camera
    if(KeyboardState::keyDown[m_KeyCameraReset])
    {
        m_pCamera->setZoom(100.0f);
        m_pCamera->setRotation(0.5f * D3DX_PI, 0.9f);
    }

    // sound things
    if (KeyboardState::keyReleased[m_KeySoundToggle])
        SoundManager::playSound(SoundManager::EXPLOSION, 0.1f, &D3DXVECTOR3(50, 100, 0), m_pCamera);
//        SoundManager::setSoundsEnabled(!SoundManager::getSoundsEnabled());
    if (KeyboardState::keyReleased[m_KeyMusicToggle])
        SoundManager::setMusicEnabled(!SoundManager::getMusicEnabled());
    if (KeyboardState::keyDown[m_KeyVolumeUp])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() + 50);
    if (KeyboardState::keyDown[m_KeyVolumeDown])
        SoundManager::setMasterVolume(SoundManager::getMasterVolume() - 50);


    //mouse zoom
    if(MouseState::mouseZSpeed)
    {
        m_pCamera->zoom(-MouseState::mouseZSpeed * m_ModifyMouseZoom);
    }

    //camera mouse panning
    if(MouseState::mouseButton[m_KeyMouseDragButton])
    {
        m_pCamera->move(MouseState::mouseYSpeed * m_ModifyMouseDragY, -MouseState::mouseXSpeed * m_ModifyMouseDragX, 0);
    }

    //Camera pitching
    if(MouseState::mouseButton[m_KeyMouseRotateButton])
    {
        m_pCamera->rotate(MouseState::mouseXSpeed * 0.05f * m_ModifyMouseRotationX, MouseState::mouseYSpeed * 0.01f * m_ModifyMouseRotationY);
    }

    if(KeyboardState::keyReleased[28])
    {
        m_pCamera->m_Test = !m_pCamera->m_Test;
    }

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
            SoundManager::playSound(SoundManager::READY, 0.1f, (D3DXVECTOR3*)&pUnit->GetMatrix()._41, m_pCamera);

            m_tmpSelectedUnit = pUnit; // TODO: remove after testing single unit moving
        }
    }
    else if(MouseState::mouseButtonReleased[m_KeyMousePickButton])
    {       
        m_Selector.buttonUp();
    }

}

void GameState::loadConfiguration()
{
    Config & c = * Config::getInstance();
    c.setFilename("controls.ini");
    c.readFile();

    m_KeyCameraPanUp = c.getValueAsInt("camera pan up");
    m_KeyCameraPanDown = c.getValueAsInt("camera pan down");
    m_KeyCameraPanRight = c.getValueAsInt("camera pan right");
    m_KeyCameraPanLeft = c.getValueAsInt("camera pan left");
    m_KeyCameraZoomIn = c.getValueAsInt("camera zoom in");
    m_KeyCameraZoomOut = c.getValueAsInt("camera zoom out");
    m_KeyGenerateNewTerrain = c.getValueAsInt("generate new terrain");
    m_KeyGeneratePassability = c.getValueAsInt("generate passability terrain");
    m_KeyToggleWireframe = c.getValueAsInt("toggle wireframe");
    m_KeyTerrainDetailUp = c.getValueAsInt("terrain detail up");
    m_KeyTerrainDetailDown = c.getValueAsInt("terrain detail down");
    m_KeyMouseDragButton = c.getValueAsInt("mouse drag button");
    m_KeyMousePickButton = c.getValueAsInt("mouse pick button");
    m_KeyMouseRotateButton = c.getValueAsInt("mouse rotate button");
    m_KeyCameraReset = c.getValueAsInt("camera reset");
    //please note the global MOUSE_CAMSPEED modifier!
    m_ModifyMouseDragX = c.getValueAsInt("modify mouse panning horizontal") * MOUSE_CAMSPEED;
    m_ModifyMouseDragY = c.getValueAsInt("modify mouse panning vertical") * MOUSE_CAMSPEED;
    m_ModifyMouseRotationX = c.getValueAsInt("modify mouse rotation horizontal") * MOUSE_CAMSPEED;
    m_ModifyMouseRotationY = c.getValueAsInt("modify mouse rotation vertical") * MOUSE_CAMSPEED;
    m_ModifyMouseZoom = c.getValueAsInt("modify mouse zoom") * MOUSE_CAMSPEED;

    // sound keys
    m_KeySoundToggle = c.getValueAsInt("toggle sound");
    m_KeyMusicToggle = c.getValueAsInt("toggle music");
    m_KeyVolumeUp = c.getValueAsInt("master volume up");
    m_KeyVolumeDown = c.getValueAsInt("master volume down");
    
}
