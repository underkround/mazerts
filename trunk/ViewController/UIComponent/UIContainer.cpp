/**
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UIContainer.h"
#include "../Input/MouseState.h"
#include "ILayoutManager.h"

UIContainer::UIContainer(const int posX, const int posY, const unsigned int width, const unsigned int height)
    : UIComponent(posX, posY, width, height)
{
    m_Transparent = false;
    m_StealFlags = STEAL_MOUSE;
    m_ProcessFlags |= CPROCESS_MOUSE_BUTTONS;
    m_pLayoutManager = NULL;
    m_VertexZ = 0.001f;

    unsetLayoutFlag(LAYOUT_HINT_NORESIZE);
    unsetLayoutFlag(LAYOUT_HINT_NOREPOS);
}

void UIContainer::onParentChange()
{
    UIComponent::onParentChange();
    if(m_pLayoutManager) {
        m_pLayoutManager->reLayout();
    }
}

int UIContainer::processEvent(int eventFlag, TCHAR arg)
{
    return (m_Transparent) ? STEAL_NONE : UIComponent::processEvent(eventFlag, arg);
}

bool UIContainer::addComponent(UIComponent* child)
{
    if(!child || child == this)
        return false;
    // remove from old parent if any
    if(child->m_pParent && child->m_pParent != this)
    {
        child->m_pParent->removeComponent(child);
    }
    // add as my child
    child->m_pParent = this;
    m_Children.pushTail(child);
    child->onParentChange();
    // redo the layout
    if(m_pLayoutManager)
        m_pLayoutManager->reLayout();
    return true;
}


bool UIContainer::removeComponent(UIComponent* child)
{
    if(!child || child->m_pParent != this)
        return false; // not my child!
    m_Children.remove(child); // remove from my childs
    child->m_pParent = NULL; // clear the parent from the child
    // redo the layout
    if(m_pLayoutManager)
        m_pLayoutManager->reLayout();
    return true; // removed
}


bool UIContainer::releaseComponent(UIComponent* child)
{
    if(!child)
        return false; // sanity check
    if(removeComponent(child))
    {
        // found, now release and delete
        child->release();
        delete child;
        return true; // removed and released
    }
    return false; // not found
}


void UIContainer::releaseLayout()
{
    if(m_pLayoutManager)
        delete m_pLayoutManager;
    m_pLayoutManager = NULL;
}


const void UIContainer::setLayoutManager(ILayoutManager* pLManager)
{
    releaseLayout();
    m_pLayoutManager = pLManager;
    if(pLManager)
    {
        m_pLayoutManager->m_pParent = this;
        m_pLayoutManager->reLayout();
    }
}


// ===== Component methods


void UIContainer::release()
{
    // release the children
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        node->item->m_pParent = NULL;
        node->item->release();
        delete node->item;
        node = m_Children.removeGetNext(node);
    }
    releaseLayout();
    UIComponent::release();
}


void UIContainer::render(LPDIRECT3DDEVICE9 pDevice)
{
    if(!m_Visible)
        return; // we are hidden, no rendering

    // render children
    if(!m_Children.empty())
    {
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node) {
            node->item->render(pDevice);
            node = node->next;
        }
    }

    // render self (calls the onRender -method)
    if(!m_Transparent)
        UIComponent::render(pDevice);
}


void UIContainer::update(const float frameTime)
{
    if(!m_Visible)
        return;
    // update children
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        node->item->update(frameTime);
        node = node->next;
    }
}


const bool UIContainer::setSize(const int width, const int height)
{
    // @TODO! take children account
    bool res = UIComponent::setSize(width, height);
    // notify children of parent size change
    if(res) {
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node) {
            node->item->onParentChange();
            node = node->next;
        }
    }
    return res;
}


UIComponent* UIContainer::getFocus(ProcessFlags processEvent)
{
    if(!m_Children.empty())
    {
        int relMouseX = MouseState::mouseX - getPosX();
        int relMouseY = MouseState::mouseY - getPosY();
        UIComponent* comp;
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node)
        {
            comp = node->item;
            node = node->next;
            // check intersection
            // we can use mouseX and mouseY straight for child since we are the root at 0:0
            if(
                relMouseX >= comp->m_Pos.x &&
                relMouseY >= comp->m_Pos.y &&
                relMouseX <= comp->m_Pos.x + comp->m_Size.x &&
                relMouseY <= comp->m_Pos.y + comp->m_Size.y
            )
            {
                // found child for focus
                comp = comp->getFocus(processEvent);
                if(comp)
                    return comp;
            }
        }
    }
//    return this;
    return (!m_Transparent && (m_ProcessFlags & processEvent)) ? this : NULL;
}
