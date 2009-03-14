/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "UIAssetController.h"

#include "../../Model/Common/Config.h"

#include "../Input/MouseState.h"
#include "../Input/KeyboardState.h"
#include "../Terrain/TerrainIntersection.h"
#include "../Unit/UI3DObjectManager.h"


UIAssetController::UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Camera* pCamera, Selector* pSelector)
{
    m_pUnitCommandDispatcher = new UnitCommandDispatcher(NULL); // TODO: use actual player-object
    m_pDevice = pDevice;
    m_pCamera = pCamera;
    m_pSelector = pSelector;
}


UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::loadConfiguration()
{
    Config & c = * Config::getInstance();
    // Assume that the caller of this method (GameState) has loaded the file
    //c.setFilename("controls.ini");
    //c.readFile();
    m_KeyMouseDragButton = c.getValueAsInt("mouse drag button");
    m_KeyMousePickButton = c.getValueAsInt("mouse pick button");
    m_KeyMouseRotateButton = c.getValueAsInt("mouse rotate button");
}


void UIAssetController::release()
{
    if(m_pUnitCommandDispatcher)
    {
        m_pUnitCommandDispatcher->release();
        delete m_pUnitCommandDispatcher;
        m_pUnitCommandDispatcher = NULL;
    }
    m_pSelector = NULL;
}


void UIAssetController::updateControls(const float frameTime)
{
    //Terrain picking test
    //if(MouseState::mouseButton[m_KeyMousePickButton])

    // ===== Selecting

    if(MouseState::mouseButtonReleased[m_KeyMousePickButton])
    {
        D3DXMATRIX matProj;
        m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

        D3DXMATRIX matView;
        m_pDevice->GetTransform(D3DTS_VIEW, &matView);

        D3DXVECTOR3 rayOrigin, rayDir;

        MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

        UIUnit* pUnit = UI3DObjectManager::pickUnit(rayOrigin, rayDir);

        if(pUnit)
        {
            if(m_pUnitCommandDispatcher->empty())
            {
                // we don't have any units under control so use selected
                m_pUnitCommandDispatcher->addUnit(pUnit->getUnit());
                pUnit->setSelected(true);
                m_SelectedUIUnits.pushHead(pUnit);
                // we have units, dispatch target to them
            }
            else
            {
                m_pUnitCommandDispatcher->getTarget()->setTarget(pUnit->getUnit());
                m_pUnitCommandDispatcher->dispatch();
                m_pUnitCommandDispatcher->clearUnits();
                ListNode<UIUnit*>* node = m_SelectedUIUnits.headNode();
                while(node) {
                    node->item->setSelected(false);
                    node = m_SelectedUIUnits.removeGetNext(node);
                }
            }
        }
        else if(!m_pUnitCommandDispatcher->empty())
        {
            D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
            if(hitSquare)
            {
                unsigned short targetX = (unsigned short)hitSquare->x;
                unsigned short targetY = (unsigned short)hitSquare->y;
                m_pUnitCommandDispatcher->getTarget()->setTarget(targetX, targetY, false);
                m_pUnitCommandDispatcher->dispatch();
                m_pUnitCommandDispatcher->clearUnits();
                ListNode<UIUnit*>* node = m_SelectedUIUnits.headNode();
                while(node) {
                    node->item->setSelected(false);
                    node = m_SelectedUIUnits.removeGetNext(node);
                }
            }
            delete hitSquare;
        }
    }
/*
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
        m_pSelector->buttonUp();
    }
*/
}
