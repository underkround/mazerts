/**
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UIContainer.h"
#include "../Input/MouseState.h"
#include "../Controller/IUIController.h"


UIContainer::UIContainer() : UIComponent()
{
    m_Transparent = false;
    m_StealFlags = STEAL_MOUSE;
    m_ProcessFlags = CPROCESS_MOUSE_BUTTONS;
}


int UIContainer::processEvent(int eventFlag, TCHAR arg)
{
    return (m_Transparent) ? STEAL_NONE : m_StealFlags;
}

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
    return true;
}


bool UIContainer::removeComponent(UIComponent* child)
{
    //if(child == m_pFocused)
    //    m_pFocused = NULL;
    if(child->m_pParent != this)
        return false; // not my child!
    m_Children.remove(child); // remove from my childs
    child->m_pParent = NULL; // clear the parent from the child
    return true; // removed
}


bool UIContainer::releaseComponent(UIComponent* child)
{
    //if(child == m_pFocused)
    //    m_pFocused = NULL;
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
        delete node->item;
        node = m_Children.removeGetNext(node);
    }
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
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        node->item->update(frameTime);
        node = node->next;
    }
}


const bool UIContainer::setSize(const int width, const int height)
{
    // TODO! take children account
    return UIComponent::setSize(width, height);
}


UIComponent* UIContainer::getFocus()
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
                return comp->getFocus();
            }
        }
    }
    return this;
}

/*
bool UIContainer::updateControls(const float frameTime)
{
    // if we are hidden, no control functionality
    if(!m_Visible)
        return false;

    int result = STEAL_NONE;

    // pressed-event
    // focus transfers with pressed, or with idle
    if(MouseState::mouseButtonPressedBits)
    {
        // always get new focus
        m_pFocused = getFocus();
        if(m_pFocused)
        {
            result = m_pFocused->updateControls(frameTime);
        }
        if(!result)
        {
            // our actions, if we are not transparent and mouse intersects us, we have the focus
            if(!m_Transparent && mouseIntersects())
            {
                return STEAL_MOUSE;
            }
        }
        return result;
    }

    // button down
    if(MouseState::mouseButtonBits)
    {
        // focus stays, update if inside focused
        if(m_pFocused)
        {
            // check if the event happens inside focused
            if(m_pFocused->mouseIntersects())
                return m_pFocused->updateControls(frameTime);
            // check if the focused handles outside events
            if(m_pFocused->getStealFlags() & STEAL_MOUSE_OUTSIDE)
                return m_pFocused->updateControls(frameTime);
            // we still steal the down-action if there is focused
            return STEAL_MOUSE;
        }
        // we don't have focus so pass the event
        return STEAL_NONE;
    }

    // button released - loose focused if any
    if(MouseState::mouseButtonReleasedBits)
    {
        if(m_pFocused)
        {
            // check if the event happens inside focused
            if(m_pFocused->mouseIntersects())
                return m_pFocused->updateControls(frameTime);
            // if outside, we unfocus in any case. We steal the mouse anyway if we have focus
            int result = STEAL_MOUSE;
            // check if the focused handles outside events
            if(m_pFocused->m_StealFlags & STEAL_MOUSE_OUTSIDE)
                result = m_pFocused->updateControls(frameTime);
            // unfocus
            m_pFocused = NULL;
            return result;
        }
        // we don't have focus so pass the event
        return STEAL_NONE;
    }

    // idle-event?
    if(!MouseState::mouseMoved && MouseState::mouseIdle > m_MouseIdleTreshold)
    {
        // always get the focus for idle event
        UIComponent* comp = getFocus();
        if(comp)
        {
            return m_pFocused->updateControls(frameTime);
        }
        return STEAL_NONE;
    }

    // TODO: keyboard events?
    // implement charbuffer to KeyboardEvents ?

    // the root container passes every event no dispatched to child
    return STEAL_NONE;





/*
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
*/

//}
