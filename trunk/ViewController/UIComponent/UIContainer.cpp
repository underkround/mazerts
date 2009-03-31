/**
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UIContainer.h"
#include "../Input/MouseState.h"
#include "../Controller/IUIController.h"

bool UIContainer::addComponent(UIComponent* child)
{
    if(child == this)
        return false;
    // remove from old parent if any
    if(child->m_pParent && child->m_pParent != this)
    {
        child->m_pParent->removeComponent(child);
    }
    // add as my child
    child->m_pParent = this;
    m_Children.pushHead(child);
    // get the buttons that the child steals so we can transfer the focus it wants
    m_StealedMouseButtonsForChildren |= child->getStealedMouseButtons();
    return true;
}


bool UIContainer::removeComponent(UIComponent* child)
{
    if(child->m_pParent != this)
        return false; // not my child!
    m_Children.remove(child); // remove from my childs
    child->m_pParent = NULL; // clear the parent from the child
    // reload the actions that the children catch
    m_StealedMouseButtonsForChildren = 0;
    if(!m_Children.empty()) {
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node) {
            m_StealedMouseButtonsForChildren |= node->item->getStealedMouseButtons();
            node = node->next;
        }
    }
    return true; // removed
}


bool UIContainer::releaseComponent(UIComponent* child)
{
    // first remove the child from my children
    if(removeComponent(child))
    {
        // found, now release and delete
        child->release();
        delete child;
        return true; // removed and released
    }
    return false; // not found
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
        delete node->item; // TODO! why is this failing?
        node = m_Children.removeGetNext(node);
    }
    m_StealedMouseButtonsForChildren = 0;
    UIComponent::release();
}


void UIContainer::render(LPDIRECT3DDEVICE9 pDevice)
{
    if(!m_Visible)
        return; // we are hidden, no rendering

    // render self (calls the onRender -method)
    if(!m_Transparent)
        UIComponent::render(pDevice);

    // render children
    if(!m_Children.empty())
    {
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node) {
            node->item->render(pDevice);
            node = node->next;
        }
    }
}


void UIContainer::update(const float frameTime)
{
    if(!m_Visible)
        return;
    // update children
    if(!m_Children.empty())
    {
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node)
        {
            node->item->update(frameTime);
            node = node->next;
        }
    }
}


const bool UIContainer::setSize(const int width, const int height)
{
    // TODO! take children account
    return UIComponent::setSize(width, height);
}


bool UIContainer::updateControls(const float frameTime)
{
    // if we are hidden, no control functionality
    if(!m_Visible)
        return false;

    // if we do not have parent (in which case the update call should have
    // come from it and we trust it), we should check if the click even hit us
    /*
    if(!m_pParent)
    {
        if(!insersects(MouseState::mouseX, MouseState::mouseY))
            return false; // out of our area
    }*/

    // if we have focused child
    if(m_pFocused)
    {
        // check for focus lost - button released action
        if( (MouseState::mouseButtonReleasedBits & m_StealedMouseButtonsForChildren) ) // TODO: should this be only the sealed buttons by focused?
        {
            // outside focused?
            if(!m_pFocused->intersects(MouseState::mouseX, MouseState::mouseY))
            {
                // button released outside the focused component -> focus lost
                m_pFocused->focusLost();
                m_pFocused = NULL;
                // steal the released button since it was pressed inside the component
                return true;
            }
        }
        // new button was pressed while there is focused child, check if the click was inside it
        if( (MouseState::mouseButtonPressedBits & m_StealedMouseButtonsForChildren) ) // TODO: should this be only the sealed buttons by focused?
        {
            if(!m_pFocused->mouseIntersects())
            {
                // the click was outside focused, take the focus away
                m_pFocused->focusLost();
                m_pFocused = NULL;
                // keep searching for new focus subject or dispatch the click
            }
        }
        // keep updating the focused component, if it's still focused after
        // previous checks
        if(m_pFocused)
        {
            if(m_pFocused->updateControls(frameTime))
            {
                // child took explicit control of the input
                return true;
            }
        }
    }
    // no focused child
    // check for focus-setting events (mouse action click)
    if(!m_pFocused)
    {
        // if the mouse action button was pressed and we have children, check for dispatch
//        if(MouseState::mouseButtonPressed[MOUSE_FOCUS_BUTTON] && !m_Children.empty())

        // if one of the mouse buttons handled by children was pressed, and
        // we have children, check if the click hit one of them and that the
        // child that was hit handles the event
        if((MouseState::mouseButtonPressedBits & m_StealedMouseButtonsForChildren) && !m_Children.empty())
        {
            // get the mouse position inside this container
            int relativeMouseX = MouseState::mouseX - getPosX();
            int relativeMouseY = MouseState::mouseY - getPosY();
            // check for component under the button hit
            UIComponent* comp;
            ListNode<UIComponent*>* node = m_Children.headNode();
            while(node)
            {
                comp = node->item;
                node = node->next;
                // use this rather than component's intersect, since it needs to
                // determine the absolute position for every call, and this doesn't
                if( relativeMouseX >= comp->m_Pos.x &&
                    relativeMouseY >= comp->m_Pos.y &&
                    relativeMouseX <= comp->m_Pos.x + comp->m_Size.x &&
                    relativeMouseY <= comp->m_Pos.y + comp->m_Size.y
                    )
                {
                    if(comp->stealableEvents())
                    {
                        m_pFocused = comp;
                        m_pFocused->focusGain();
                        node = NULL;
                    }
                }
            }
            // if we found focused child, transfer control call
            if(m_pFocused)
            {
                if(m_pFocused->updateControls(frameTime))
                {
                    // child took the control
                    return true;
                }
                // child did not handle the controls, continue to our controller
            }
        }
    }

    // if there was no focused child, focus transfer or the focused
    // didn't take take explicit control over the controls, check if
    // we can have controller to handle the controls
    //
    return (!m_Transparent &&
            UIComponent::updateControls(frameTime) &&
            mouseIntersects())
            ? true : false;
}


void UIContainer::focusLost()
{
    m_Focused = false;
    if(m_pFocused)
    {
        m_pFocused->focusLost();
        m_pFocused = NULL;
    }
}
