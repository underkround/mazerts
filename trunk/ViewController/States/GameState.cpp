#include "GameState.h"

#include "../../Model/Terrain/Terrain.h"
#include "../../Model/Terrain/AntinTerrainGenerator.h"
#include "../../Model/PathFinding/PathFinderMaster.h"
#include "../../Model/Asset/AssetFactory.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"

//Camera related
#include "../Culling/FrustumCull.h"
#include "../Terrain/TerrainIntersection.h"


//DEBUG
#include "../3DDebug/UI3DDebug.h"


#define KEYBOARD_CAMSPEED 60.0f
#define MOUSE_CAMSPEED 2.0f

GameState::GameState()
{
    m_MainCameraX = 127.0f;
    m_MainCameraY = -70.0f;
    m_MainCameraZ = -170.0f;

    m_pManager = NULL;
    m_pUITerrain = NULL;
    m_pApp = NULL;
    m_Created = false;
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

     //Get the pathfinder running
    PathFinderMaster::getInstance()->start();

    //Prepare manager
    UI3DObjectManager::create(pDevice);
    m_pManager = UI3DObjectManager::getInstance();


    //TEST
    for(int i = 0; i < 100; i++)
    {
        AssetFactory::createUnit(NULL, 0, m_pApp->RandInt(0, 200), m_pApp->RandInt(0, 200));
    }

    //Model-terrain
    Terrain* pTerrain = Terrain::getInstance();
    AntinTerrainGenerator* pGenerator = new AntinTerrainGenerator(100, 512);
    pTerrain->initialize(pGenerator);

    pTerrain->setWaterLevel(50);

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

    m_Created = true;

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

bool GameState::update(float frameTime)
{
    updateControls(frameTime);

    //Updating the actual game logic    
    AssetCollection::updateUnits(frameTime);
    m_pManager->getRootObject()->Update(frameTime);

    //Keep running
    return true;
}

void GameState::prepareForRender(LPDIRECT3DDEVICE9 pDevice)
{
    //Camera stuff
    D3DXMATRIX view;
    D3DXMatrixLookAtLH( &view,
                        &D3DXVECTOR3(m_MainCameraX, m_MainCameraY, m_MainCameraZ),
                        &D3DXVECTOR3(m_MainCameraX + MouseState::mouseX * 0.5f, m_MainCameraY + 200.0f, m_MainCameraZ + MouseState::mouseY * 0.5f + 100.0f),
                        &D3DXVECTOR3(0.0f, 0.0f, -1.0f));
    pDevice->SetTransform(D3DTS_VIEW, &view);

    D3DXMATRIX proj;
    pDevice->GetTransform(D3DTS_PROJECTION, &proj);
    FrustumCull::rebuildFrustum(&view, &proj);


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
}

void GameState::render(LPDIRECT3DDEVICE9 pDevice)
{
    
    //Terrain needs normal backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);        
    m_pUITerrain->render(pDevice);

    m_Selector.render(pDevice);
      
    //Antsys models need reverse backface-culling
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pManager->getRootObject()->Render(pDevice);

}


void GameState::updateControls(float frameTime)
{
    if (KeyboardState::keyDown[203])
    {
        m_MainCameraX -= KEYBOARD_CAMSPEED * frameTime;
    }
    else if (KeyboardState::keyDown[205])
    {
        m_MainCameraX += KEYBOARD_CAMSPEED * frameTime;
    }
    
    if (KeyboardState::keyDown[200])
    {
        m_MainCameraY += KEYBOARD_CAMSPEED * frameTime;
    }
    else if (KeyboardState::keyDown[208])
    {
        m_MainCameraY -= KEYBOARD_CAMSPEED * frameTime;
    }
    
    if (KeyboardState::keyDown[30])
    {
        m_MainCameraZ += KEYBOARD_CAMSPEED * frameTime;
    }
    else if (KeyboardState::keyDown[44])
    {
        m_MainCameraZ -= KEYBOARD_CAMSPEED * frameTime;
    }

    if(KeyboardState::keyReleased[57])
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

    if(KeyboardState::keyDown[2])
    {
        m_pUITerrain->createPassabilityTexture(m_pDevice);
    }

    if(KeyboardState::keyReleased[15])
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

    if(KeyboardState::keyReleased[49])
    {
        unsigned char detail = m_pUITerrain->getDetailLevel();
        detail++;
        if(detail < 4)
        {
            m_pUITerrain->setDetailLevel2(detail);
        }
    }

    if(KeyboardState::keyReleased[50])
    {
        unsigned char detail = m_pUITerrain->getDetailLevel();
        detail--;
        //Wrapping
        if(detail < 4)
        {
            m_pUITerrain->setDetailLevel2(detail);
        }
    }

    //mouse zoom
    m_MainCameraZ += MOUSE_CAMSPEED * frameTime * MouseState::mouseZSpeed;

    //mouse dragging
    if(MouseState::mouseButton[1])
    {
        m_MainCameraX -= frameTime * MouseState::mouseXSpeed * MOUSE_CAMSPEED;
        m_MainCameraY += frameTime * MouseState::mouseYSpeed * MOUSE_CAMSPEED;
        //TODO: move faster when farther away from terrain
    }

    //Terrain picking test
    if(MouseState::mouseButton[0])
    {
        D3DXMATRIX matProj;
        m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

        D3DXMATRIX matView;
        m_pDevice->GetTransform(D3DTS_VIEW, &matView);

        D3DXVECTOR3 rayOrigin,rayDir;

        MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

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
                m_Selector.setPoint(D3DXVECTOR2(hitSquare->x, hitSquare->y));                    
                delete hitSquare;
            }
        }
    }
    else if(MouseState::mouseButtonReleased[0])
    {       
        m_Selector.buttonUp();
    }

}