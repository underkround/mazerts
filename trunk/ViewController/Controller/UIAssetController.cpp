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
#include "../../Model/Asset/IAsset.h"
#include "../../Model/Asset/Unit.h"
#include "../../Model/Asset/Building.h"
#include "../../Model/Asset/AssetFactory.h"

#include "../UIComponent/RootContainer.h"
#include "../UIComponent/GridLayout.h"

#include "../../Model/Defs/Defs.h"
#include "../../Model/Defs/DefManager.h"

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

    m_pUnitCarryingCamera = 0;

    // create the buttoncontainer
    RootContainer* rc = RootContainer::getInstance();
    const int bcX = 10;
    const int bcY = 320;
    const int bcW = 2*64 + 10;
    const int bcH = 4*64 + 20;
    m_pButtonPanel = new UIContainer(bcX, bcY, bcW, bcH);
    m_pButtonPanel->setBackground(0xFF000000);
    m_pButtonPanel->setLayoutFlag(LAYOUT_HINT_PACK);
    m_pButtonPanel->setLayoutManager(new GridLayout(0, 2));
    m_pButtonPanel->setTooltip("Command panel");
    MarginSet* panelSnaps = m_pButtonPanel->getSnapMargins();
    panelSnaps->left = 10;
//    panelSnaps->bottom = 10;
    rc->addComponent(m_pButtonPanel);

    // create the container for asset-instance spesific components
    const int ipX = bcX + bcW + 10;
    const int ipY = bcY;
    const int ipW = bcW;
    const int ipH = bcH;
    m_pInstanceControlPanel = new UIContainer(ipX, ipY, ipW, ipH);
    m_pInstanceControlPanel->setBackground(0xFF111111);
    m_pInstanceControlPanel->setLayoutFlag(LAYOUT_HINT_PACK);
    m_pInstanceControlPanel->setLayoutManager(new GridLayout(0, 2));
    m_pInstanceControlPanel->setTooltip("Asset command panel");
//    m_pInstanceControlPanel->setVisible(false);
    rc->addComponent(m_pInstanceControlPanel);

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

    // create buttons for assets that are not build inside another asset
    ListNode<AssetDef*>* node = DefManager::getInstance()->getAssetDefNode();
    while(node) {
        if(!node->item->anonymous && node->item->constructionIn == 0) {
            createBuildingButton(node->item);
        }
        node = node->next;
    }

    m_CurrentBuildOreTaken = 0;
    m_pCurrentBuildAssetDef = 0;
    m_pCurrentBuildButton = 0;
    m_pCurrentBuildAsset = 0;

    m_pCurrentBuildQueue = 0;
    m_pCurrentNukeSilo = NULL;
}


UIAssetController::~UIAssetController()
{
    release();
}


void UIAssetController::createBuildingButton(AssetDef* pAssetDef)
{
    BasicButton* button = new BasicButton(64, 64, pAssetDef->tag, this);
    RootContainer* rc = RootContainer::getInstance();
    // hax for speed
    //button->setRandomObject(pAssetDef);
    // primary texture
    button->setBackgroundTexture(rc->getIconTexture(pAssetDef->tag));
    // alt texture name
    char nameC[64];
    sprintf_s(nameC, "alt_%d", pAssetDef->tag);
    wchar_t nameWTF[10];
    MultiByteToWideChar(CP_ACP, 0, nameC, -1, nameWTF, 10);
    button->setBackgroundTextureClicked(rc->getIconTexture(nameWTF));
    // tooltip
    sprintf_s(nameC, "%s, cost: %d", pAssetDef->name.c_str(), pAssetDef->constructionCostOre);
    button->setTooltip(nameC);
    m_pButtonPanel->addComponent(button);

    m_pStopButton = new BasicButton(64, 64, 439857, this);
    m_pStopButton->setTooltip("Stop");
    m_pStopButton->setBackgroundTexture(rc->getIconTexture(_T("stop")));
    m_pStopButton->setBackgroundTextureClicked(rc->getIconTexture(_T("alt_stop")));

    m_pNukeButton = new BasicButton(64, 64, 666, this);
    m_pNukeButton->setTooltip("Armageddon");
    m_pNukeButton->setBackgroundTexture(rc->getIconTexture(_T("8")));
    m_pNukeButton->setEnabled(false);
    m_pNukeButton->setBackgroundTextureClicked(rc->getIconTexture(_T("alt_8")));
}


// ===== IButtonListener methods
#include "../../Model/Console.h"

void UIAssetController::onButtonClick(BasicButton* pSrc)
{
    // stop-command for units
    if(pSrc == m_pStopButton) {
        DoubleLinkedList<Unit*>* pUnitList = m_pUnitCommandDispatcher->getUnits();
        ListNode<Unit*>* node = pUnitList->headNode();
        while(node) {
            if(node->item->getMovingLogic())
                node->item->getMovingLogic()->clearTargets();
            node = node->next;
        }
        return;
    }

    if (pSrc->getId() == 666)
    {
        // launch nuke
        m_pSelector->setState(Selector::SELECTOR_NUKE);
        changeState(STATE_NUKE);
        return;
    }

    // build button
    char* msg = new char[128];
    sprintf_s(msg, 128, "button %d clicked", pSrc->getId());
    Console::debug(msg);
    // check for insufficient funds
    /*
    AssetDef* def = (AssetDef*)pSrc->getRandomObject();
    if(!def)
        def = DefManager::getInstance()->getAssetDef(pSrc->getId());
    */
    AssetDef* def = DefManager::getInstance()->getAssetDef(pSrc->getId());
    if(m_pCurrentPlayer->getOre() < def->constructionCostOre) {
        Console::error("Cannot build: not enough ore");
        return;
    }
    if(!m_pCurrentPlayer->hasAsset(def->constructionRequires)) {
        Console::error("Cannot build: requirements not met");
        return;
    }
    // take the ore first to prefent the situation where there is no funds when placed
    m_CurrentBuildOreTaken = def->constructionCostOre;
    m_pCurrentPlayer->modifyOre(-m_CurrentBuildOreTaken);
    // start building
    m_pCurrentBuildAssetDef = def;
    m_pCurrentBuildButton = pSrc;
    changeState(STATE_BUILDING_PLACEMENT);
}


void UIAssetController::onButtonAltClick(BasicButton* pSrc)
{
    /* not needed with building construction */
}


void UIAssetController::changeState(State newState)
{
    if(m_State == newState)
        return;

    // end current state
    switch(m_State) {

        case STATE_ASSET_CONTROL:
            m_SelectionState = IDLE;
            m_ActionState = IDLE;
            m_TempMouseX = 0;
            m_TempMouseY = 0;
            clearSelection();
            m_pInstanceControlPanel->removeAllComponents();
            break;

        case STATE_BUILDING_PLACEMENT:
            m_pSelector->setState(Selector::SELECTOR_NORMAL);
            m_CurrentBuildOreTaken = 0;
            m_pCurrentBuildAsset = 0;
            m_pCurrentBuildAssetDef = 0;
            m_pCurrentBuildButton = 0;
            break;

        case STATE_NUKE:
            m_pSelector->setState(Selector::SELECTOR_NORMAL);
//            m_pCurrentNukeSilo = NULL;
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
            {
                Selector::SELECTION* tmpSelection = m_pSelector->buttonUp();
                if(tmpSelection)
                    delete tmpSelection;
            }
            break;

        case STATE_BUILDING_PLACEMENT:
            // check for what to build
            if(!m_pCurrentBuildButton || !m_pCurrentBuildAssetDef) {
                // cannot continue placement state without these values
                changeState(STATE_ASSET_CONTROL);
                break;
            }
//            m_pCurrentBuildButton->setLoadingStatus(0);
            clearSelection();
            m_pSelector->setState(Selector::SELECTOR_BUILDINGPLACEMENT);
            m_pSelector->setSize(D3DXVECTOR2(m_pCurrentBuildAssetDef->width, m_pCurrentBuildAssetDef->height));
            break;

        case STATE_NUKE:
            //clearSelection();
            m_pSelector->setState(Selector::SELECTOR_NUKE);
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
    // remove & delete build queues
    /*for(map<IAsset*,BuildQueue*>::iterator it = m_BuildQueues.begin(); it != m_BuildQueues.end(); ++it) {
        BuildQueue* bq = (*it).second;
        delete bq;
    }*/
    // @TODO: why does this delete the key objects? or something else
    //        elements are deleted, so that's ok. just that the map is now unsafe =P
    //m_BuildQueues.clear();
}


void UIAssetController::updateControls(const float frameTime)
{
    // update build queues
    map<IAsset*,BuildQueue*>::iterator p;
    for(p = m_BuildQueues.begin(); p != m_BuildQueues.end(); p++) {
        if(p->second == m_pCurrentBuildQueue)
            p->second->update(true); // visible queue, update logic & visual
        else
            p->second->update(false); // non-visible queue, update only logic
    }

    // @TODO: update build status to buttons
    if(!m_ButtonsToUpdate.empty()) {
        IAsset* asset;
        BasicButton* button;
        ListNode<BasicButton*>* node = m_ButtonsToUpdate.headNode();
        while(node) {
            button = node->item;
            asset = (IAsset*)button->getRandomObject();
            if(asset && asset->getState() == IAsset::STATE_BEING_BUILT) {
                int percentage = (int)(asset->getHitpoints() / asset->getHitpointsMax() * 100);
                button->setLoadingStatus(percentage);
                node = node->next;
            } else {
                button->setEnabled(true);
                node = m_ButtonsToUpdate.removeGetNext(node);
            }
        }
    }

    // update nuke button
    if (m_pCurrentNukeSilo)
    {
        int percentage = (int)(m_pCurrentNukeSilo->getWeapon()->getLoadingPercentage() * 100);
        if (percentage < 1)
        {
            m_pNukeButton->setLoadingStatus(100);
            m_pNukeButton->setEnabled(true);
        }
        else
        {
            m_pNukeButton->setLoadingStatus(percentage);
            m_pNukeButton->setEnabled(false);
        }
    }

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
                toggleFirstPersonCamera();
            }
            break;

        // =====
        case STATE_NUKE:
        case STATE_BUILDING_PLACEMENT:
            // sanity check
            if(m_State == STATE_BUILDING_PLACEMENT && (!m_pCurrentBuildButton || !m_pCurrentBuildAssetDef)) {
                changeState(STATE_ASSET_CONTROL);
                break;
            }

            // update selector
            D3DXMATRIX matProj, matView;
            D3DXVECTOR3 rayOrigin, rayDir;
            m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
            m_pDevice->GetTransform(D3DTS_VIEW, &matView);
            MouseState::transformTo3D(matView, matProj, rayOrigin, rayDir);
            D3DXVECTOR3* hitSquare = TerrainIntersection::pickTerrain(rayOrigin, rayDir);
            if(hitSquare)
            {
                m_pSelector->setPoint(D3DXVECTOR2(hitSquare->x, hitSquare->y));
                delete hitSquare;
            }

            // mouse actions
            if(MouseState::mouseButtonReleased[MouseState::mouseSecondButton]) {
                // return the preserved ore
                if(m_CurrentBuildOreTaken) {
                    m_pCurrentPlayer->modifyOre(m_CurrentBuildOreTaken);
                }
                // cancel build state with secondary mouse button
                changeState(STATE_ASSET_CONTROL);
                setNukeSilo(NULL);
                break;
            } else if(MouseState::mouseButtonReleased[MouseState::mouseFirstButton] && m_State == STATE_BUILDING_PLACEMENT) {
                // placement made
                if(m_pSelector->isBuildable()) {
                    D3DXVECTOR2 bpos = m_pSelector->getBuildingPoint();
                    // @TODO: assetfactory->create
                    IAsset* newAsset = AssetFactory::createAsset(m_pCurrentPlayer, m_pCurrentBuildAssetDef->tag, (short)bpos.x, (short)bpos.y, true);
                    if(newAsset) {
                        m_pCurrentBuildButton->setEnabled(false);
                        m_pCurrentBuildButton->setRandomObject(newAsset);
                        m_ButtonsToUpdate.pushHead(m_pCurrentBuildButton);
                    }
                    // problem, new asset not created, return ore to player
                    else {
                        if(m_CurrentBuildOreTaken)
                            m_pCurrentPlayer->modifyOre(m_CurrentBuildOreTaken);
                    }
                    changeState(STATE_ASSET_CONTROL);
                    break;
                }
                // @TODO ??
            } else if(MouseState::mouseButtonReleased[MouseState::mouseFirstButton] && m_State == STATE_NUKE) {
                if (m_pCurrentNukeSilo != NULL)
                {
                    IWeapon* pWeapon = m_pCurrentNukeSilo->getWeapon();
                    pWeapon->setTarget(new Target((unsigned short)m_pSelector->getNukePoint().x, (unsigned short)m_pSelector->getNukePoint().y));
                }
                changeState(STATE_ASSET_CONTROL);
                setNukeSilo(NULL);
                break;
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
                        if (m_SelectedAssetType != UNIT) {
                            templist.release();
                            m_pInstanceControlPanel->removeAllComponents();
                            m_pCurrentBuildQueue = 0;
                        }
                        m_SelectedAssetType = UNIT;
                        templist.pushHead(pAsset);
                        // show stop-button for friendly asset selection
                    }
                    else if ((m_SelectedAssetType == NONE || m_SelectedAssetType == ENEMY) && m_pCurrentPlayer == pAsset->getAsset()->getOwner() && pAsset->getAsset()->getAssetType() == IAsset::BUILDING)
                    {
                        if (m_SelectedAssetType != BUILDING) {
                            templist.release();
                            m_pInstanceControlPanel->removeAllComponents();
                            m_pCurrentBuildQueue = 0;
                        }
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

                    // show stop-button for friendly asset selection
                    if(m_pInstanceControlPanel->empty() && m_SelectedAssetType == UNIT) {
                        m_pInstanceControlPanel->addComponent(m_pStopButton);
                    }
                    // show build menu for factories
                    else if(m_SelectedAssetType == BUILDING && pNode->item) {
                        if(pNode->item->getAsset()->getDef()->isfactory) {
                            setupBuildQueue(pNode->item->getAsset());
                        } else if (pNode->item->getAsset()->getTypeTag() == BUILDING_TYPE_SILO) {
                            // show nuke
                            m_pInstanceControlPanel->removeAllComponents();
                            m_pInstanceControlPanel->addComponent(m_pNukeButton);
                            setNukeSilo((Building*)pNode->item);
                        } else {
                            m_pInstanceControlPanel->removeAllComponents();
                            m_pCurrentBuildQueue = 0;
                            setNukeSilo(NULL);
                        }
                    }

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

    if(m_SelectedUIAssets.empty()) {
        if(!m_pInstanceControlPanel->getChildren()->empty()) {
            m_pInstanceControlPanel->removeAllComponents();
        }
    }

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
                    m_pInstanceControlPanel->removeAllComponents();
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
                if (m_SelectedAssetType != ENEMY) {
                    SoundManager::playSound(SOUND_YES, 0.1f, *(D3DXVECTOR3*)&pUIAsset->GetMatrix()._41, false);

                    // show stop button for units
                    if(m_pInstanceControlPanel->empty() && m_SelectedAssetType == UNIT) {
                        m_pInstanceControlPanel->addComponent(m_pStopButton);
                    }
                    // show build menu for factories
                    else if(m_SelectedAssetType == BUILDING) {
                        if(pUIAsset->getAsset()->getDef()->isfactory)
                            setupBuildQueue(pUIAsset->getAsset());
                        else if (pUIAsset->getAsset()->getTypeTag() == BUILDING_TYPE_SILO) {
                            // show nuke
                            m_pInstanceControlPanel->removeAllComponents();
                            m_pInstanceControlPanel->addComponent(m_pNukeButton);
                            setNukeSilo((Building*)pUIAsset->getAsset());
                        } else
                            m_pInstanceControlPanel->removeAllComponents();
                    }

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
    if (m_pCurrentNukeSilo && instance == m_pCurrentNukeSilo)
    {
        m_pCurrentNukeSilo = NULL;
    }

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

    // check for buildqueue
    /* @TODO: fix
    if(m_BuildQueues[instance]) {
        BuildQueue* bq = m_BuildQueues[instance];
        delete bq;
        m_BuildQueues.erase(instance);
    }
    */
    // @TODO: temporary fix
    /*
    BuildQueue* bq = m_BuildQueues[instance];
    if(bq) {
        bq->haxDisableBySettingAssetToNull();
    }
    */
    // JUST HIDE THE PANEL FFS
    if(m_pCurrentBuildQueue && m_pCurrentBuildQueue->getAsset() == instance) {
        m_pCurrentBuildQueue = 0;
        m_pInstanceControlPanel->removeAllComponents();
    }
}

void UIAssetController::toggleFirstPersonCamera()
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

void UIAssetController::toggleFirstPersonCamera(UIUnit* pUnit)
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
    else
    {
        m_pUnitCarryingCamera = pUnit;
        m_UnitCamera.attach(m_pUnitCarryingCamera);
        Camera::pushTop(&m_UnitCamera);
    }
}


void UIAssetController::setupBuildQueue(IAsset* pAsset) {
    m_pInstanceControlPanel->removeAllComponents();
    // check if queue is already made
    BuildQueue* queue = m_BuildQueues[pAsset];
    if(!queue) {
        queue = new BuildQueue(pAsset);
        m_BuildQueues[pAsset] = queue;
    }
    // setup buttons from queue
    queue->fillPanel(m_pInstanceControlPanel);
    // set as current (for ui-updates)
    m_pCurrentBuildQueue = queue;
}
