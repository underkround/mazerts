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
#include "../Asset/UI3DObjectManager.h"
#include "../Sound/SoundManager.h"

#include "../Camera/UnitCamera.h"

UIAssetController::UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Selector* pSelector)
{
    m_pUnitCommandDispatcher = new UnitCommandDispatcher(NULL); // TODO: use actual player-object
    m_pDevice = pDevice;
    m_pSelector = pSelector;
    m_SelectionState = IDLE;
    m_ActionState = IDLE;
    m_TempMouseX = 0;
    m_TempMouseY = 0;

    m_pUnitCarryingCamera = NULL;

    // default setting values
    m_KeyMouseActionButton = 1;
    m_KeyMousePickButton = 0;
    m_KeyAddToSelection = 29;
    m_KeyQueueCommands = 42;
    m_MousePickDragThreshold = 4;
    m_MouseActionDragThreshold = 4;
    m_KeyPickModifier = 0;
    m_KeyActionModifier = 0;
    m_KeyActionWhileDragging = false;

    m_SelectedAssetType = NONE;

    m_KeyFirstPersonCamera = 46;
}



UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::loadConfigurations()
{
    // we trust that config files have been loaded when this is called
    Config & c = * Config::getInstance();
    // mouse buttons
    c.updateInt("mouse action button",                  m_KeyMouseActionButton);
    c.updateInt("mouse pick button",                    m_KeyMousePickButton);
    c.updateInt("add to selection modifier key",        m_KeyAddToSelection);
    c.updateInt("queue command modifier key",           m_KeyQueueCommands);
    // drag tresholds
    c.updateInt("mouse pick moving threshold",          m_MousePickDragThreshold);
    c.updateInt("mouse action moving threshold",        m_MouseActionDragThreshold);
    // keyboard modifiers
    c.updateInt("mouse pick modifier key",              m_KeyPickModifier);
    c.updateInt("mouse action modifier key",            m_KeyActionModifier);
    // weather to allow action when dragging
    c.updateBool("mouse action enabled when dragging",  m_KeyActionWhileDragging);
    // key for first person camera
    c.updateInt("key first person camera",              m_KeyFirstPersonCamera);
}


void UIAssetController::release()
{
    if(m_pUnitCarryingCamera)
        Camera::popTop();
    m_UnitCamera.detach();
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
    bool pickModifier = (!m_KeyPickModifier || KeyboardState::keyDown[m_KeyPickModifier]) ? true : false;
    bool actionModifier = (!m_KeyActionModifier || KeyboardState::keyDown[m_KeyActionModifier]) ? true : false;
    
    if(pickModifier && MouseState::mouseButton[m_KeyMousePickButton])
        onPickButton(frameTime);
    else if(pickModifier && MouseState::mouseButtonReleased[m_KeyMousePickButton])
        onPickRelease(frameTime);
    else if(MouseState::mouseButtonReleased[m_KeyMouseActionButton])
        onActionRelease(frameTime);
    else if(MouseState::mouseButton[m_KeyMouseActionButton])
        onActionButton(frameTime);

    if(KeyboardState::keyReleased[m_KeyFirstPersonCamera])
    {
        if(m_pUnitCarryingCamera)
        {
            if(Camera::countStack())
            {
                //Camera::popTop();
                Camera::pop(&m_UnitCamera);
            }
            m_pUnitCarryingCamera->RemoveChild(&m_UnitCamera);
            m_UnitCamera.detach();
            m_pUnitCarryingCamera = NULL;
        }
        else if(!m_SelectedUIAssets.empty() && m_SelectedAssetType == UNIT)
        {
            m_pUnitCarryingCamera = (UIUnit*)m_SelectedUIAssets.peekHead();
            m_UnitCamera.attach(m_pUnitCarryingCamera);
            Camera::pushTop(&m_UnitCamera);
        }
    }
}


void UIAssetController::clearSelection()
{
    m_pUnitCommandDispatcher->clearUnits();
    if(!m_SelectedUIAssets.empty())
    {
        ListNode<UIAsset*>* node = m_SelectedUIAssets.headNode();
        while(node) {
            node->item->setSelected(false);
            node = m_SelectedUIAssets.removeGetNext(node);
        }
    }
    m_SelectedAssetType = NONE;
}


void UIAssetController::onPickRelease(const float frameTime)
{
    // ===== Pick button released

    D3DXMATRIX matProj, matView;
    D3DXVECTOR3 rayOrigin, rayDir;
    m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    m_pDevice->GetTransform(D3DTS_VIEW, &matView);
    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

    switch(m_SelectionState)
    {

    default:
    case IDLE:
    case CLICK:
        {
            break;
        }

    case DRAG:
        {
            Selector::SELECTION* selection = m_pSelector->buttonUp();
            if(!selection->assets.empty())
            {
                // check if selection is buildings or units
                DoubleLinkedList<UIAsset*> templist;

                ListNode<UIAsset*>* pNode = selection->assets.headNode();
                while(pNode)
                {
                   UIAsset* pAsset = pNode->item;
                   if (pAsset->getAsset()->getAssetType() == IAsset::UNIT)
                   {
                        if (m_SelectedAssetType == BUILDING)
                            templist.release();
                        m_SelectedAssetType = UNIT;
                        templist.pushHead(pAsset);
                   }
                   else if (m_SelectedAssetType == NONE && pAsset->getAsset()->getAssetType() == IAsset::BUILDING)
                   {
                       m_SelectedAssetType = BUILDING;
                       templist.pushHead(pAsset);
                   }
                   pNode = pNode->next;
                }

                // add the selection to units under control
                pNode = templist.headNode();
                SoundManager::playSound(SOUND_YES, 0.1f, *(D3DXVECTOR3*)&pNode->item->GetMatrix()._41, false);
                while(pNode)
                {
                    UIAsset* pAsset = pNode->item;
                    if(!pAsset->isSelected())
                    {
                        if (pAsset->getAsset()->getAssetType() == IAsset::UNIT)
                            m_pUnitCommandDispatcher->addUnit((Unit*)pAsset->getAsset());
                        pAsset->setSelected(true);
                        m_SelectedUIAssets.pushHead(pAsset);
                    }
                    pNode = pNode->next;
                }
            }
            delete selection;
            break;
        }

    } // switch

    m_SelectionState = IDLE;
    m_TempMouseX = 0;
    m_TempMouseY = 0;
}


void UIAssetController::onPickButton(const float frameTime)
{
    // ===== Pick button being pressed
    D3DXMATRIX matProj, matView;
    D3DXVECTOR3 rayOrigin, rayDir;
    m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    m_pDevice->GetTransform(D3DTS_VIEW, &matView);
    MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

    bool addMode = (m_KeyAddToSelection && KeyboardState::keyDown[m_KeyAddToSelection]) ? true : false;

    switch(m_SelectionState)
    {

    default:
    case IDLE:
        {
            if(!addMode)
                clearSelection();
            m_TempMouseX = MouseState::mouseX;
            m_TempMouseY = MouseState::mouseY;
            UIAsset* pUIAsset = UI3DObjectManager::pickAsset(rayOrigin, rayDir);
            if(pUIAsset && !pUIAsset->isSelected())
            {
                pUIAsset->setSelected(true);
                SoundManager::playSound(SOUND_YES, 0.1f, *(D3DXVECTOR3*)&pUIAsset->GetMatrix()._41, false);
                if (pUIAsset->getAsset()->getAssetType() == IAsset::UNIT)
                {
                    m_pUnitCommandDispatcher->addUnit((Unit*)pUIAsset->getAsset());
                    m_SelectedAssetType = UNIT;
                }
                else if (pUIAsset->getAsset()->getAssetType() == IAsset::BUILDING)
                {
                    // TODO: Buildingdispatcher
                    m_SelectedAssetType = BUILDING;
                }
                m_SelectedUIAssets.pushHead(pUIAsset);
            }
            m_SelectionState = CLICK;
            break;
        }

    case CLICK:
        {
            if( abs(m_TempMouseX - MouseState::mouseX) > m_MousePickDragThreshold ||
                abs(m_TempMouseY - MouseState::mouseY) > m_MousePickDragThreshold)
            {
                // get the first point clicked while moved from IDLE to CLICK
                // and set it for selector as the first click-point
                MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir, m_TempMouseX, m_TempMouseY);
                D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
                if(hitSquare)
                {
                    m_pSelector->setPoint(D3DXVECTOR2(hitSquare->x, hitSquare->y));
                    delete hitSquare;
                }
                // used to track changes for unnecessary intersection updates (slow operation)
                m_TempMouseX = MouseState::mouseX;
                m_TempMouseY = MouseState::mouseY;
                // we are now dragging
                m_SelectionState = DRAG;
            }
            break;
        }

    case DRAG:
        {
            // if mouse moved, update the intersection point (slow operation)
            // TODO: does not take into account if the camera moves etc.
            if(m_TempMouseX != MouseState::mouseX || m_TempMouseY != MouseState::mouseY)
            {
                m_TempMouseX = MouseState::mouseX;
                m_TempMouseY = MouseState::mouseY;
                D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
                if(hitSquare)
                {
                    m_pSelector->setPoint(D3DXVECTOR2(hitSquare->x, hitSquare->y));
                    delete hitSquare;
                }
            }
        }

    } // switch
}


void UIAssetController::onActionButton(const float frameTime)
{
    // ===== Action key being pressed
    if(m_SelectionState == IDLE && !m_pUnitCommandDispatcher->empty())
    {
        switch(m_ActionState)
        {

        default:
        case IDLE:
            {
                m_ActionState = CLICK;
                m_TempMouseX = MouseState::mouseX;
                m_TempMouseY = MouseState::mouseY;
                break;
            }

        case CLICK:
            {
                // check if mouse has moved over the treshold while action key pressed
                if( abs(m_TempMouseX - MouseState::mouseX) > m_MouseActionDragThreshold ||
                    abs(m_TempMouseY - MouseState::mouseY) > m_MouseActionDragThreshold)
                {
                    m_ActionState = DRAG;
                }
                break;
            }

        case DRAG:
            break;

        } // switch
    }
}


void UIAssetController::onActionRelease(const float frameTime)
{
    // ===== Action key released
    if(m_SelectionState == IDLE && !m_pUnitCommandDispatcher->empty())
    {
        if(!m_KeyActionWhileDragging && m_ActionState == DRAG)
        {
            m_ActionState = IDLE;
            return; // action not allowed when dragging
        }

        D3DXMATRIX matProj, matView;
        D3DXVECTOR3 rayOrigin, rayDir;
        m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
        m_pDevice->GetTransform(D3DTS_VIEW, &matView);
        MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);

        // first check if the click hits to asset as target
        UIAsset* pUIAsset = UI3DObjectManager::pickAsset(rayOrigin, rayDir);
        if(pUIAsset)
        {
            // asset as target
            m_pUnitCommandDispatcher->getTarget()->setTarget(pUIAsset->getAsset());
            m_pUnitCommandDispatcher->dispatch(KeyboardState::keyDown[m_KeyQueueCommands]);
            SoundManager::playSound(SOUND_ATTACK, 0.1f, *((D3DXVECTOR3*)m_pUnitCommandDispatcher->getUnits()->headNode()->item->getPosition()), false);
        }

        // secondly check if the click hits to terrain as target
        else {
            // terrain coordinate as target
            D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
            if(hitSquare)
            {
                unsigned short targetX = (unsigned short)hitSquare->x;
                unsigned short targetY = (unsigned short)hitSquare->y;
                m_pUnitCommandDispatcher->getTarget()->setTarget(targetX, targetY, false);
                m_pUnitCommandDispatcher->dispatch(KeyboardState::keyDown[m_KeyQueueCommands]);
                SoundManager::playSound(SOUND_AKNOWLEDGEMENT, 0.1f, *((D3DXVECTOR3*)m_pUnitCommandDispatcher->getUnits()->headNode()->item->getPosition()), false);
                delete hitSquare;
            }
        }
    }
    m_ActionState = IDLE;
}
