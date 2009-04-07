#include "RootContainer.h"

#include "../../Model/Common/Config.h"

RootContainer::RootContainer() : UIContainer(0,0,0,0)
{
    m_pApp = NULL;
    m_Created = false;
    m_pDevice = NULL;
    m_pFocused = NULL;
    m_pIdleFocus = NULL;
    m_MouseIdleTreshold = 1.0f;
    m_CharBufferCount = 0;
}


void RootContainer::loadConfigurations()
{
    // TODO: read the mouse buttons from confs etc
    Config* c = Config::getInstance();
}


HRESULT RootContainer::create(LPDIRECT3DDEVICE9 pDevice, IApplication* pApp)
{
    if(!pApp)
    {
        return S_FALSE; // we need the IApplication pointer
    }

    if(!m_Created)
    {
        m_pDevice = pDevice;

        // store the application pointer
        m_pApp = pApp;

        // create our resource container
        m_ResourceContainer.Create(pDevice);

        // set our size to match the window
        RECT win = pApp->GetWindowRect();
        m_Size.x = win.right;
        m_Size.y = win.bottom;

        // supposedly we don't have any children, but why not..
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node)
        {
            node->item->create(pDevice);
            node = node->next;
        }

        HRESULT hres = UIContainer::create(pDevice);

        if(!FAILED(hres))
        {
            m_Created = true;
        }

        return hres;
    }
    return S_OK;
}


/**
 *
 * Controls and input
 *
 */


UIComponent* RootContainer::getFocus(ProcessFlags processEvent)
{
    UIComponent* result = NULL;
    // first check currently focused, if any
    if(m_pFocused)
    {
        // check intersection
        // get component's absolute positions
        int cAbsPosX = m_pFocused->getPosX();
        int cAbsPosY = m_pFocused->getPosY();
        if(
            MouseState::mouseX >= cAbsPosX &&
            MouseState::mouseY >= cAbsPosY &&
            MouseState::mouseX <= cAbsPosX + m_pFocused->getWidth() &&  //m_Size.x &&
            MouseState::mouseY <= cAbsPosY + m_pFocused->getHeight()    //m_Size.y
        )
        {
            //m_pFocused = m_pFocused->getFocus(processEvent);
            result = m_pFocused->getFocus(processEvent);
            // check that focused processes the event
            //if(!(m_pFocused->getProcessFlags() & processEvent))
            //    m_pFocused = NULL;
        }
        else
        {
            m_pFocused = NULL;
        }
    }
    // do the intersection for children if focused lost in check
    // or it never was
    if(!m_pFocused)
    {
        UIComponent* comp;
        ListNode<UIComponent*>* node = m_Children.headNode();
        while(node)
        {
            comp = node->item;
            node = node->next;
            // check intersection
            // we can use mouseX and mouseY straight for child since we are the root at 0:0
            if(
                MouseState::mouseX >= comp->getRelativePosX() &&
                MouseState::mouseY >= comp->getRelativePosY() &&
                MouseState::mouseX <= comp->getRelativePosX() + comp->getWidth() &&   //comp->m_Size.x &&
                MouseState::mouseY <= comp->getRelativePosY() + comp->getHeight()     //comp->m_Size.y
            )
            {
                result = comp->getFocus(processEvent);
                // found children for focus, check if it processes the event
                if(result)
                {
                    //m_pFocused = comp->getFocus(processEvent);
                    node = NULL; // break from the loop
                }
            }
        }
    }
//    return m_pFocused;
    return result;
}


int RootContainer::updateControls(const float frameTime)
{
    int result = STEAL_NONE;

    // mouse press
    if(MouseState::mouseButtonPressedBits)
    {
        m_pFocused = getFocus(CPROCESS_MOUSE_BUTTONS); //(PROCESS_MOUSE_BUTTONS);
        if(m_pFocused)
        {
            result = m_pFocused->processEvent(CEVENT_MOUSE_PRESSED, MouseState::mouseButtonPressedBits);
        }
    }

    // other mouse events, if focused and if it handles mouse
    else if(m_pFocused && (m_pFocused->getProcessFlags() & CPROCESS_MOUSE_BUTTONS))
    {
        if(MouseState::mouseButtonBits)
        {
            if(MouseState::mouseMoved)
            {
                // mouse drag
                result = m_pFocused->processEvent(CEVENT_MOUSE_DRAGGED, MouseState::mouseButtonBits);
            }
            else
            {
                // Do not report idle mouse down, no need for it
                // But still capture the mouse so controls don't fall through
                result |= m_pFocused->getStealFlags();
            }
        }

        // mouse release
        if(MouseState::mouseButtonReleasedBits)
        {
            result = m_pFocused->processEvent(CEVENT_MOUSE_RELEASED, MouseState::mouseButtonReleasedBits);
        }

        // mouse idle
        if(!MouseState::mouseMoved && MouseState::mouseIdle >= m_MouseIdleTreshold)
        {
            result = m_pFocused->processEvent(CEVENT_MOUSE_IDLE, 0);
        }
    }


    // MOUSE IDLE
    // if mouse idling, check for idle event over component
    if(!MouseState::mouseMoved && MouseState::mouseIdle >= m_MouseIdleTreshold)
    {
        if(!m_pIdleFocus)
        {
            m_pIdleFocus = getFocus(CPROCESS_MOUSE_IDLE);
            if(m_pIdleFocus)
                m_pIdleFocus->processEvent(CEVENT_MOUSE_IDLE, 0);
        }
    }
    // not idling, loose idle focus if any
    else if(m_pIdleFocus)
    {
        m_pIdleFocus = NULL;
    }


    // MOUSE WHEEL
    /*
    if(m_pFocused && (m_pFocused->getProcessFlags() & CPROCESS_MOUSE_WHEEL))
    {
    }
    */

    // chars in charbuffer
    if(m_pFocused && m_CharBufferCount && (m_pFocused->getProcessFlags() & CPROCESS_TEXT))
    {
        // send to focused (if any) if it processes text
        if(m_pFocused->getProcessFlags() & CPROCESS_TEXT)
        {
            for(int i=m_CharBufferCount; i>=0; --i)
                result = m_pFocused->processEvent(CEVENT_CHAR_TYPED, m_CharBuffer[i]);
            m_CharBufferCount = 0;
        }
    }

    // other keypresses
    /*
    else if(KeyboardState::keyDown)
    {
        // send to focused (if any) if it processes general keypresses
        if(m_pFocused->
    }*/

    return result;
}



/**
 *
 * Rendering related methods
 *
 */


void RootContainer::render(LPDIRECT3DDEVICE9 pDevice)
{
    // set common rendering flags settings for components here
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    // we just render the children
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        node->item->render(pDevice);
        node = node->next;
    }
}


HRESULT RootContainer::onDeviceLost()
{
    m_pDevice = NULL;
    HRESULT res = S_OK;
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        res = node->item->onDeviceLost();
        if(FAILED(res))
        {
            return res;
        }
        node = node->next;
    }
    return res;
}


HRESULT RootContainer::onRestore(LPDIRECT3DDEVICE9 pDevice)
{
    HRESULT res = S_OK;
    m_pDevice = pDevice;
    if(m_pApp)
    {
        RECT win = m_pApp->GetWindowRect();
        m_Size.x = win.right;
        m_Size.y = win.bottom;
    }
    ListNode<UIComponent*>* node = m_Children.headNode();
    while(node)
    {
        res = node->item->onRestore(pDevice);
        if(FAILED(res))
        {
            return res;
        }
        node = node->next;
    }
    return res;
}


/**
 * Resource management
 */


LPDIRECT3DTEXTURE9 RootContainer::getIconTexture(int id)
{
    TCHAR tmp[10];
    _stprintf_s(tmp, _T("%d"), id); // yeah, i suck with these T_¤!"#!% convertions
    return getIconTexture(tmp);
}


LPDIRECT3DTEXTURE9 RootContainer::getIconTexture(TCHAR id[])
{
    LPDIRECT3DTEXTURE9 pTexture;
    TCHAR filename[100];
    _stprintf_s(filename, _T("../data/icons/icon_%s.png"), id);
    pTexture = m_ResourceContainer.FindTexture(filename);
    if(!pTexture)
    {
        D3DXCreateTextureFromFile(m_pDevice, filename, &pTexture);
        if(pTexture)
        {
            m_ResourceContainer.AddResource(filename, pTexture);
            pTexture = m_ResourceContainer.FindTexture(filename);
        }
    }
    return pTexture;
}
