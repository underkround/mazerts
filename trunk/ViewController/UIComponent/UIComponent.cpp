/**
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#include "UIComponent.h"

#include "../Input/MouseState.h"
#include "../Controller/Cursor.h" // for mouse tooltip

UIComponent::UIComponent(const int posX, const int posY, const unsigned int width, const unsigned int height)
{
    m_Pos(posX, posY);
    m_PreferredSize(width, height);
    m_Size(width, height);

    m_pParent = NULL;
    m_VertexZ = 0.0001f;

    m_Changed = true;
    m_Visible = true; // render the component?
    m_Clipped = false; // use viewport clipping with component?

//    m_pTooltip = NULL;

    // Background drawing stuff - from BasicComponent
    m_BackgroundARGB.x = 0xFFFFFFFF;
    m_BackgroundARGB.y = 0xFFFFFFFF;
    m_ForegroundARGB = 0xFF555555;
    m_BackgroundStyle = FILLSTYLE_GRADIENT_V;
    m_pBackgroundTexture = NULL;
    m_pBackgroundVB = NULL;

    m_StealFlags = STEAL_MOUSE;

    m_ProcessFlags = CPROCESS_MOUSE_BUTTONS;
    m_ProcessFlags |= CPROCESS_MOUSE_IDLE; // for tooltips

    m_LayoutFlags = LAYOUT_HINT_ASPECTRATIO;
}

// implement when deriving
// this default action only steals (mouse)
int UIComponent::processEvent(int eventFlag, TCHAR arg)
{
    // only default action currently is to show a possible tooltip on mouse idle
    if(eventFlag == CEVENT_MOUSE_IDLE && m_Tooltip.GetLength() > 0)
        Cursor::getInstance()->setTooltip(m_Tooltip.GetBuffer(), 5.0f);
/*
    if(eventFlag == CEVENT_MOUSE_IDLE && m_pTooltip)
        Cursor::getInstance()->setTooltip(m_pTooltip, 5.0f);
*/
    return m_StealFlags;
}

// ===== Size & position

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

// =====

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
    else
        pDevice->SetTexture(0, NULL);

    // render the default background
    if(m_pBackgroundVB)
    {
        // do this only if no parent
        if(!m_pParent)
        {
            pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }
        pDevice->SetFVF(TRANSLITVERTEX::GetFVF());        
        pDevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(TRANSLITVERTEX));
        pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
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

    m_pBackgroundVB->Lock(0, 0, (void**)&pBackVertices, D3DLOCK_DISCARD);
    //m_pBackgroundVB->Lock(0, sizeof(TRANSLITVERTEX)*4, (void**)&pBackVertices, D3DLOCK_DISCARD);
    {
        pBackVertices[0].x = posX;
        pBackVertices[0].y = posY;
        pBackVertices[0].z = m_VertexZ;
        pBackVertices[0].tu = 0.0f;
        pBackVertices[0].tv = 0.0f;
        pBackVertices[0].rhw = 0.99f;

        pBackVertices[1].x = posX + m_Size.x;
        pBackVertices[1].y = posY;
        pBackVertices[1].z = m_VertexZ;
        pBackVertices[1].tu = 1.0f;
        pBackVertices[1].tv = 0.0f;
        pBackVertices[1].rhw = 0.99f;

        pBackVertices[2].x = posX + m_Size.x;
        pBackVertices[2].y = posY + m_Size.y;
        pBackVertices[2].z = m_VertexZ;
        pBackVertices[2].tu = 1.0f;
        pBackVertices[2].tv = 1.0f;
        pBackVertices[2].rhw = 0.99f;

        pBackVertices[3].x = posX;
        pBackVertices[3].y = posY + m_Size.y;
        pBackVertices[3].z = m_VertexZ;
        pBackVertices[3].tu = 0.0f;
        pBackVertices[3].tv = 1.0f;
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
