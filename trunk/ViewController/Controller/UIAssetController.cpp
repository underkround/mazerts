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
#include "../Terrain/UITerrain.h"
#include "../Terrain/TerrainIntersection.h"
#include "../Asset/UI3DObjectManager.h"
#include "../Sound/SoundManager.h"

#include "../Camera/UnitCamera.h"

#include "../../Model/Asset/AssetCollection.h"

#include "../UIComponent/RootContainer.h"
#include "../UIComponent/GridLayout.h"

#include "Cursor.h" // for tooltip
#include <TCHAR.h>  // for tooltip

UIAssetController::UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Selector* pSelector, Player* pCurrentPlayer)
{
    m_pUnitCommandDispatcher = new UnitCommandDispatcher(NULL); // TODO: use actual player-object
    m_pDevice = pDevice;
    m_pSelector = pSelector;
    m_SelectionState = IDLE;
    m_ActionState = IDLE;
    m_TempMouseX = 0;
    m_TempMouseY = 0;
    m_State = STATE_ASSET_CONTROL;

    m_pUnitCarryingCamera = NULL;

    // create the buttoncontainer
    RootContainer* rc = RootContainer::getInstance();
    const int bcX = 10;
    const int bcY = 320;
    const int bcW = 160;
    const int bcH = 430;
    m_pButtonPanel = new UIContainer(bcX, bcY, bcW, bcH);
    m_pButtonPanel->setBackground(0xCC222222);
    m_pButtonPanel->setLayoutFlag(LAYOUT_HINT_NORESIZE);
    m_pButtonPanel->setLayoutManager(new GridLayout(0, 2));
    m_pButtonPanel->setTooltip("Building control panel");
    RootContainer::getInstance()->addComponent(m_pButtonPanel);

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

    m_TooltipTreshold = 0.5f;
    m_TooltipLifetime = 2.0f;
    m_TooltipShown = false;

    m_SelectedAssetType = NONE;

    m_KeyFirstPersonCamera = 46;

    AssetCollection::registerListener(this);

    m_pCurrentPlayer = pCurrentPlayer;
}


UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::changeState(State newState)
{
    // end current state
    switch(m_State) {

        case STATE_ASSET_CONTROL:
            m_SelectionState = IDLE;
            m_ActionState = IDLE;
            m_TempMouseX = 0;
            m_TempMouseY = 0;
            break;

        case STATE_BUILDING_PLACEMENT:
            m_pSelector->setState(Selector::SELECTOR_NORMAL);
            break;

    }

    // begin new state
    m_State = newState;
    switch(m_State) {

        case STATE_ASSET_CONTROL:
            m_SelectionState = IDLE;
            m_ActionState = IDLE;
            m_TempMouseX = 0;
            m_TempMouseY = 0;
            m_pSelector->setState(Selector::SELECTOR_NORMAL);
            break;

        case STATE_BUILDING_PLACEMENT:
            // check for what to build
            // @TODO: check from defManager if m_BuildTag exists and can be build
            if(m_BuildTag < 0) {
                changeState(STATE_ASSET_CONTROL);
                break;
            } else {

            }
            break;

    }
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
    // time for showing the tooltip
    c.updateFloat("tooltip treshold",                   m_TooltipTreshold);
    c.updateFloat("tooltip lifetime",                   m_TooltipLifetime);
}


void UIAssetController::release()
{
    AssetCollection::unregisterListener(this);
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


void UIAssetController::onButtonClick(BasicButton* pSrc)
{
}


void UIAssetController::onButtonAltClick(BasicButton* pSrc)
{
}


void UIAssetController::updateControls(const float frameTime)
{
    bool pickModifier = (!m_KeyPickModifier || KeyboardState::keyDown[m_KeyPickModifier]) ? true : false;
    bool actionModifier = (!m_KeyActionModifier || KeyboardState::keyDown[m_KeyActionModifier]) ? true : false;

    switch(m_State) {

        // =====
        case STATE_ASSET_CONTROL:
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
                // disable unit camera
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
                // enable unit camera
                else if(!m_SelectedUIAssets.empty() && m_SelectedAssetType == UNIT)
                {
                    m_pUnitCarryingCamera = (UIUnit*)m_SelectedUIAssets.peekHead();
                    m_UnitCamera.attach(m_pUnitCarryingCamera);
                    Camera::pushTop(&m_UnitCamera);
                }
            }
            break;

        // =====
        case STATE_BUILDING_PLACEMENT:
            // sanity check
            if(m_BuildTag < 0) {
                changeState(STATE_ASSET_CONTROL);
                break;
            }

            // update selector
            //m_pSelector->update();

            // mouse actions
            if(MouseState::mouseButtonReleased[MouseState::mouseSecondButton]) {
                // cancel build state with secondary mouse button
                changeState(STATE_ASSET_CONTROL);
                break;
            } else if(MouseState::mouseButtonReleased[MouseState::mouseFirstButton]) {
                // placement made
                if(m_pSelector->isBuildable()) {
                    // @TODO: assetfactory->create
                    changeState(STATE_ASSET_CONTROL);
                    break;
                }
                // @TODO ??
            }

            break;

    } // switch

    // if mouse idles over asset, display tooltip showing it's name
    if(!MouseState::mouseMoved && (MouseState::mouseIdle > m_TooltipTreshold))
    {
        if(!m_TooltipShown)
        {
            D3DXMATRIX matProj, matView;
            D3DXVECTOR3 rayOrigin, rayDir;
            m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
            m_pDevice->GetTransform(D3DTS_VIEW, &matView);
            MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);
            UIAsset* pUIAsset = UI3DObjectManager::pickAsset(rayOrigin, rayDir);
            if(pUIAsset)
            {
                string assetName = pUIAsset->getAsset()->getDef()->name;
                Cursor::getInstance()->setTooltip(assetName.c_str(), m_TooltipLifetime);
                m_TooltipShown = true;
            }
        }
    }
    else if(m_TooltipShown)
    {
        m_TooltipShown = false;
        Cursor::getInstance()->clearTooltip();
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
            clearSelection();
            Selector::SELECTION* selection = m_pSelector->buttonUp();
            if(!selection->assets.empty())
            {
                // check if selection is buildings or units
                DoubleLinkedList<UIAsset*> templist;

                ListNode<UIAsset*>* pNode = selection->assets.headNode();
                while(pNode)
                {
                   UIAsset* pAsset = pNode->item;
                   if (pAsset->getAsset()->getAssetType() == IAsset::UNIT && m_pCurrentPlayer == pAsset->getAsset()->getOwner())
                   {
                        if (m_SelectedAssetType != UNIT)
                            templist.release();
                        m_SelectedAssetType = UNIT;
                        templist.pushHead(pAsset);
                   }
                   else if ((m_SelectedAssetType == NONE || m_SelectedAssetType == ENEMY) && m_pCurrentPlayer == pAsset->getAsset()->getOwner() && pAsset->getAsset()->getAssetType() == IAsset::BUILDING)
                   {
                        if (m_SelectedAssetType != BUILDING)
                            templist.release();
                        m_SelectedAssetType = BUILDING;
                        templist.pushHead(pAsset);
                   }
                   else if (m_SelectedAssetType == NONE && m_pCurrentPlayer != pAsset->getAsset()->getOwner())
                   {
                       m_SelectedAssetType = ENEMY;
                       templist.pushHead(pAsset);
                   }
                   pNode = pNode->next;
                }

                // add the selection to units under control
                pNode = templist.headNode();
                if(pNode)
                {
                    if (m_SelectedAssetType != ENEMY)
                        SoundManager::playSound(SOUND_YES, 0.1f, *(D3DXVECTOR3*)&pNode->item->GetMatrix()._41, false);
                    while(pNode)
                    {
                        UIAsset* pAsset = pNode->item;
                        if(!pAsset->isSelected())
                        {
                            if (pAsset->getAsset()->getAssetType() == IAsset::UNIT && m_SelectedAssetType != ENEMY) // if you comment out the enemy check, you can move enemies
                                m_pUnitCommandDispatcher->addUnit((Unit*)pAsset->getAsset());
                            pAsset->setSelected(true);
                            m_SelectedUIAssets.pushHead(pAsset);
                        }
                        pNode = pNode->next;
                    }
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
                if (pUIAsset->getAsset()->getAssetType() == IAsset::UNIT && pUIAsset->getAsset()->getOwner() == m_pCurrentPlayer)
                {
                    m_pUnitCommandDispatcher->addUnit((Unit*)pUIAsset->getAsset());
                    m_SelectedAssetType = UNIT;
                }
                else if (pUIAsset->getAsset()->getAssetType() == IAsset::BUILDING && pUIAsset->getAsset()->getOwner() == m_pCurrentPlayer)
                {
                    // TODO: Buildingdispatcher
                    m_SelectedAssetType = BUILDING;
                }
                else
                {
                    m_SelectedAssetType = ENEMY;
                }
                if (m_SelectedAssetType != ENEMY)
                    SoundManager::playSound(SOUND_YES, 0.1f, *(D3DXVECTOR3*)&pUIAsset->GetMatrix()._41, false);
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
        
        //Check if user clicked on minimap
        MiniMap* pMiniMap = UITerrain::getInstance()->getMiniMap();
        
        if(MouseState::mouseX > pMiniMap->getPosition().x && MouseState::mouseX < (pMiniMap->getPosition().x + pMiniMap->getSize())
            && MouseState::mouseY > pMiniMap->getPosition().y && MouseState::mouseY < (pMiniMap->getPosition().y + pMiniMap->getSize()))
        {
            D3DXVECTOR2 target = pMiniMap->minimapPositionToWorld(D3DXVECTOR2((FLOAT)MouseState::mouseX, (FLOAT)MouseState::mouseY));
            m_pUnitCommandDispatcher->getTarget()->setTarget((const unsigned short)target.x, (const unsigned short)target.y, false);
            m_pUnitCommandDispatcher->dispatch(KeyboardState::keyDown[m_KeyQueueCommands]);
            return;
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



void UIAssetController::handleReleasedAsset(IAsset* instance)
{
    // check if the released asset is the one with unit camera, if any
    if(m_pUnitCarryingCamera && m_pUnitCarryingCamera->getUnit() == instance)
    {
        m_UnitCamera.detach();
        m_pUnitCarryingCamera->RemoveChild(&m_UnitCamera);
        // clear the camera stack
        if(Camera::countStack())
            Camera::pop(&m_UnitCamera);
        m_pUnitCarryingCamera = NULL;
    }

    // check if we have asset selected that is to be released
    ListNode<UIAsset*>* node = m_SelectedUIAssets.headNode();
    while(node)
    {
        if(node->item->getAsset() == instance)
        {
            // found it
            m_pUnitCommandDispatcher->removeAsset(node->item->getAsset());
            m_SelectedUIAssets.remove(node);
            return;
        }
        node = node->next;
    }
}
