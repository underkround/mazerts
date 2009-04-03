/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UIComponent.h"
#include "../Controller/IUIController.h"

#include "../Input/MouseState.h"
//#include "../../Model/Common/Config.h" // for default mouse button

UIComponent::UIComponent()
{
    m_Pos(0, 0);
    m_MinSize(10, 10);
    m_Size(20, 20);

    m_pParent = NULL;

    m_Changed = true;
    m_Visible = true; // render the component?
    m_Clipped = false; // use viewport clipping with component?

    // Background drawing stuff - from BasicComponent
    m_BackgroundARGB.x = 0xAAAAAAAA;
    m_BackgroundARGB.y = 0xFFFFFFFF;
    m_ForegroundARGB = 0xFF222222;
    m_BackgroundStyle = FILLSTYLE_GRADIENT_V;
    m_pBackgroundTexture = NULL;
    m_pBackgroundVB = NULL;

    m_StealFlags = STEAL_MOUSE;
    m_ProcessFlags = CPROCESS_MOUSE_BUTTONS;
}


int UIComponent::processEvent(int eventFlag, TCHAR arg)
{
    return m_StealFlags;
}


// ===== Getters

const int UIComponent::getPosX() const
{
    if(m_pParent)
        return m_pParent->getPosX() + m_Pos.x;
    return m_Pos.x;
}


const int UIComponent::getPosY() const
{
    if(m_pParent)
        return m_pParent->getPosY() + m_Pos.y;
    return m_Pos.y;
}


void UIComponent::release()
{
    if(m_pParent)
    {
        m_pParent->removeComponent(this);
    }

    // Background drawing stuff - from BasicComponent
    if(m_pBackgroundVB)
    {
        m_pBackgroundVB->Release();
        m_pBackgroundVB = NULL;
    }
    if(m_pBackgroundTexture)
    {
        m_pBackgroundTexture = NULL;
    }
}


void UIComponent::render(LPDIRECT3DDEVICE9 pDevice)
{
    if(m_Visible)
    {
        // recreate the vertexbuffer
        if(m_Changed)
        {
            onChange(pDevice);
        }

        // clip the viewport if such flag set
        D3DVIEWPORT9 original;
        if(m_Clipped)
        {
            pDevice->GetViewport(&original);
            D3DVIEWPORT9 viewData = {
                (DWORD)getPosX(),
                (DWORD)getPosY(),
                (DWORD)getWidth(),
                (DWORD)getHeight(),
                0.0f,
                1.0f
            };
            pDevice->SetViewport(&viewData);
        }

        // do the actual rendering
        onRender(pDevice);

        // restore the viewport
        if(m_Clipped)
        {
            pDevice->SetViewport(&original);
        }
    }
}

/*
 * Default input handling,
 * eats defined mouse clicks and updates controller, if any set.
 * other inputs fall through
 */
/*
bool UIComponent::updateControls(const float frameTime)
{
    // hidden components do not handle controls
    if(!m_Visible)
        return false;

//    return (stealableEvents()) ? true : false;

    if(m_pParent)
        return true;

    int eventFlags = stealableEvents();

    if(m_Focused)
    {
        //if(!mouseIntersects())
        //    eventFlags |= C_EVENT_OUTSIDE;
        // if release-event happened outside us, we still steal it but lose our focus
        if((eventFlags & C_EVENT_MOUSE_RELEASED))
        //if(MouseState::mouseButtonReleasedBits)
        {
            // if the release happened outside us, we still steal it but do not act
            if(!mouseIntersects())
            {
                //return (eventFlags | C_EVENT_OUTSIDE);
                focusLost();
            }
            // if the release happened inside us, update the controller
            else if(m_pController)
                m_pController->updateControls(frameTime);
            return true;
        }
        else if((eventFlags & C_EVENT_MOUSE_PRESSED) || (eventFlags & C_EVENT_MOUSE_DOWN))
            return true;
        return false;
        //return eventFlags;
    }
    else if(eventFlags & C_EVENT_MOUSE_PRESSED)
    {
        // if(m_pParent) return true; // TODO: would this be optimization?
        if(mouseIntersects())
        {
            focusGain();
            return true;
        }
    }
    // nothing to pass, no constrol stealing
    //return C_EVENT_NONE;
    return false;
}
*/




/**
 *
 *
 * NOTE: This section contains default implementations for rendering
 * simple background for the component.
 * Normaly I would go with some kind of external Painter for
 * abstraction, but..
 * ..some say that in game project such as this, it would be
 * "overkill" to do that :<
 *
 * I could have a derived BasicComponent to implement these basic
 * renderings (and I did), but there was a problem since I wanted
 * the Container to be able to have the same basic background-functionality,
 * and it would result either:
 *
 * - base container not to derive from base component but some concrete component:
 *   - base container --> rendering component --> base component
 *   would be stupid because the middle part (rendering component) is just one
 *   implementing class amongst the others
 *
 * - concrete container to derive both:
 *   - rendering component deriving from base component
 *   - base container deriving from base component
 *   and that would be stupid :)
 *
 *
 * So I see (after 4 hours of rewriting and undoing) 2 options here:
 *
 * - implement the basic rendering in the top-part for all (in base component - here)
 *   and it will be available for every component. components can then override
 *   this base rendering and possibly call it to start with
 * or
 * - separate the rendering to own compositing class (aggregation), which in this case
 *   would make it slightly more complex to creating concrete components
 *   (would have to set right renderer or create your own).
 *   this would also introduce overhead which is not needed at all in this case 
 *
 *
 * btw!
 * no one will ever read this \o/      -z
 *
 */


HRESULT UIComponent::onDeviceLost()
{
    return S_OK;
}

HRESULT UIComponent::onRestore(LPDIRECT3DDEVICE9 pDevice)
{
    onChange(pDevice);
    return S_OK;
}


HRESULT UIComponent::onCreate(LPDIRECT3DDEVICE9 pDevice)
{
    // create default background vertexbuffer
    HRESULT hres = pDevice->CreateVertexBuffer(4 * sizeof(TRANSLITVERTEX), D3DUSAGE_WRITEONLY, TRANSLITVERTEX::GetFVF(), D3DPOOL_MANAGED, &m_pBackgroundVB, NULL);

    if(FAILED(hres))
        return hres;

    return S_OK;
}


void UIComponent::onRender(LPDIRECT3DDEVICE9 pDevice)
{
    // set texture if set
    if(m_BackgroundStyle == FILLSTYLE_TEXTURE && m_pBackgroundTexture)
        pDevice->SetTexture(0, m_pBackgroundTexture);

    // render the default background
    if(m_pBackgroundVB)
    {
        pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        pDevice->SetFVF(TRANSLITVERTEX::GetFVF());        
        pDevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(TRANSLITVERTEX));
        pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
}


void UIComponent::onChange(LPDIRECT3DDEVICE9 pDevice)
{
    if(!m_pBackgroundVB)
        if(!create(pDevice)) // vertexbuffer not created
            return; // could not create vertexbuffer

    // create default background for component
    TRANSLITVERTEX* pBackVertices = NULL;

    float posX = (float)getPosX();
    float posY = (float)getPosY();

    //m_pBackgroundVB->Lock(0, 0, (void**)&pBackVertices, D3DLOCK_DISCARD);
    m_pBackgroundVB->Lock(0, sizeof(TRANSLITVERTEX)*4, (void**)&pBackVertices, D3DLOCK_DISCARD);
    {
        pBackVertices[0].x = posX;
        pBackVertices[0].y = posY;
        pBackVertices[0].z = 0.000001f;
        pBackVertices[0].tu = 0.0f;
        pBackVertices[0].tv = 1.0f;
        pBackVertices[0].rhw = 0.99f;

        pBackVertices[1].x = posX + m_Size.x;
        pBackVertices[1].y = posY;
        pBackVertices[1].z = 0.000001f;
        pBackVertices[1].tu = 1.0f;
        pBackVertices[1].tv = 1.0f;
        pBackVertices[1].rhw = 0.99f;

        pBackVertices[2].x = posX + m_Size.x;
        pBackVertices[2].y = posY + m_Size.y;
        pBackVertices[2].z = 0.000001f;
        pBackVertices[2].tu = 1.0f;
        pBackVertices[2].tv = 0.0f;
        pBackVertices[2].rhw = 0.99f;

        pBackVertices[3].x = posX;
        pBackVertices[3].y = posY + m_Size.y;
        pBackVertices[3].z = 0.000001f;
        pBackVertices[3].tu = 0.0f;
        pBackVertices[3].tv = 0.0f;
        pBackVertices[3].rhw = 0.99f;

        switch(m_BackgroundStyle)
        {

        default:
        case FILLSTYLE_SOLID:
        case FILLSTYLE_TEXTURE:
            pBackVertices[0].dwColor = m_BackgroundARGB.x;
            pBackVertices[1].dwColor = m_BackgroundARGB.x;
            pBackVertices[2].dwColor = m_BackgroundARGB.x;
            pBackVertices[3].dwColor = m_BackgroundARGB.x;
            break;

        case FILLSTYLE_GRADIENT_H:
            pBackVertices[0].dwColor = m_BackgroundARGB.x;
            pBackVertices[1].dwColor = m_BackgroundARGB.y;
            pBackVertices[2].dwColor = m_BackgroundARGB.y;
            pBackVertices[3].dwColor = m_BackgroundARGB.x;
            break;

        case FILLSTYLE_GRADIENT_V:
            pBackVertices[0].dwColor = m_BackgroundARGB.x;
            pBackVertices[1].dwColor = m_BackgroundARGB.x;
            pBackVertices[2].dwColor = m_BackgroundARGB.y;
            pBackVertices[3].dwColor = m_BackgroundARGB.y;
            break;

        } // switch
    }
    m_pBackgroundVB->Unlock();

    m_Changed = false; // handled the change
}
