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
#include "../Sound/SoundManager.h"


UIAssetController::UIAssetController(const LPDIRECT3DDEVICE9 pDevice, Camera* pCamera, Selector* pSelector)
{
    m_pUnitCommandDispatcher = new UnitCommandDispatcher(NULL); // TODO: use actual player-object
    m_pDevice = pDevice;
    m_pCamera = pCamera;
    m_pSelector = pSelector;
    m_SelectionState = IDLE;
    m_ActionState = IDLE;
    m_TempMouseX = 0;
    m_TempMouseY = 0;

    // default setting values
    m_KeyMouseActionButton = 3; // 1
    m_KeyMousePickButton = 1; // 0
    m_KeyAddToSelection = 29;
    m_MousePickDragTreshold = 4;
    m_MouseActionDragTreshold = 4;
    m_KeyPickModifier = 0;
    m_KeyActionModifier = 0;
    m_KeyActionWhileDragging = false;
}


UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::loadConfiguration(const bool confFileLoaded)
{
    Config & c = * Config::getInstance();
    if(!confFileLoaded)
    {
        c.setFilename("controls.ini");
        c.readFile();
    }
    // mouse buttons
    c.updateInt("mouse action button",                  m_KeyMouseActionButton);
    c.updateInt("mouse pick button",                    m_KeyMousePickButton);
    c.updateInt("add to selection modifier key",        m_KeyAddToSelection);
    // drag tresholds
    c.updateInt("mouse pick moving treshold",           m_MousePickDragTreshold);
    c.updateInt("mouse action moving treshold",         m_MouseActionDragTreshold);
    // keyboard modifiers
    c.updateInt("mouse pick modifier key",              m_KeyPickModifier);
    c.updateInt("mouse action modifier key",            m_KeyActionModifier);
    // weather to allow action when dragging
    c.updateBool("mouse action enabled when dragging",  m_KeyActionWhileDragging);
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
}


void UIAssetController::clearSelection()
{
    m_pUnitCommandDispatcher->clearUnits();
    if(!m_SelectedUIUnits.empty())
    {
        ListNode<UIUnit*>* node = m_SelectedUIUnits.headNode();
        while(node) {
            node->item->setSelected(false);
            node = m_SelectedUIUnits.removeGetNext(node);
        }
    }
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
            if(!selection->units.empty())
            {
                // add the selection to units under control
                ListNode<UIUnit*>* pNode = selection->units.headNode();
                while(pNode)
                {
                    UIUnit* pUnit = pNode->item;
                    if(!pUnit->isSelected())
                    {
                        m_pUnitCommandDispatcher->addUnit(pUnit->getUnit());
                        pUnit->setSelected(true);
                        m_SelectedUIUnits.pushHead(pUnit);
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
            UIUnit* pUIUnit = UI3DObjectManager::pickUnit(rayOrigin, rayDir);
            if(pUIUnit && !pUIUnit->isSelected())
            {
                pUIUnit->setSelected(true);
                m_pUnitCommandDispatcher->addUnit(pUIUnit->getUnit());
                m_SelectedUIUnits.pushHead(pUIUnit);
            }
            m_SelectionState = CLICK;
            break;
        }

    case CLICK:
        {
            if( abs(m_TempMouseX - MouseState::mouseX) > m_MousePickDragTreshold ||
                abs(m_TempMouseY - MouseState::mouseY) > m_MousePickDragTreshold)
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
                if( abs(m_TempMouseX - MouseState::mouseX) > m_MouseActionDragTreshold ||
                    abs(m_TempMouseY - MouseState::mouseY) > m_MouseActionDragTreshold)
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
        UIUnit* pUIUnit = UI3DObjectManager::pickUnit(rayOrigin, rayDir);
        if(pUIUnit)
        {
            // asset as target
            m_pUnitCommandDispatcher->getTarget()->setTarget(pUIUnit->getUnit());
            m_pUnitCommandDispatcher->dispatch();
            SoundManager::playSound(SoundManager::OK, 0.1f, *((D3DXVECTOR3*)m_pUnitCommandDispatcher->getUnits()->headNode()->item->getPosition()), m_pCamera);
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
                m_pUnitCommandDispatcher->dispatch();
                SoundManager::playSound(SoundManager::OK, 0.1f, *((D3DXVECTOR3*)m_pUnitCommandDispatcher->getUnits()->headNode()->item->getPosition()), m_pCamera);
                delete hitSquare;
            }
        }
    }
    m_ActionState = IDLE;
}
