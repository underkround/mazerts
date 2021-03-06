#include "BasicButton.h"

#include "../Input/MouseState.h"
#include "../Controller/Cursor.h"

BasicButton::BasicButton(const int width, const int height, const int buttonId, IButtonListener* listener)
    : UIComponent(0, 0, width, height)
{
    m_pListener = listener;
    m_buttonId = buttonId;
    m_ButtonDown = false;

    m_AlphaBlend = false;

    m_BackgroundARGBClicked = 0xFFFF2222;
    m_pBackgroundTextureClicked = 0;

    m_StealFlags |= STEAL_MOUSE;

    m_ProcessFlags |= CPROCESS_MOUSE_BUTTONS;
    m_ProcessFlags |= CPROCESS_MOUSE_IDLE; // for tooltips

    m_LayoutFlags = LAYOUT_HINT_ASPECTRATIO;

    m_pRandomObject = 0;

    //kiitos jussi kun muistit alusta muuttujasi, my�h�styi palautus tunnilla <3 <3 <3
    m_Enabled = true;
}


BasicButton::~BasicButton()
{
    if(m_pRandomObject)
        delete m_pRandomObject;
}


// implement when deriving
// this default action only steals (mouse)
int BasicButton::processEvent(int eventFlag, TCHAR arg)
{
    // only default action currently is to show a possible tooltip on mouse idle
    if(eventFlag == CEVENT_MOUSE_IDLE && m_Tooltip.GetLength() > 0)
        Cursor::getInstance()->setTooltip(m_Tooltip.GetBuffer(), 5.0f);

    if(eventFlag == CEVENT_MOUSE_RELEASED)
        m_ButtonDown = false;
    else if(eventFlag == CEVENT_MOUSE_PRESSED && m_Enabled)
        m_ButtonDown = true;

    if(m_Enabled && m_pListener && eventFlag == CEVENT_MOUSE_RELEASED && mouseIntersects()) {
        // arg holds mouse buttons as bits
        if(arg & (1 << MouseState::mouseFirstButton)) {
            m_pListener->onButtonClick(this);
        }
        if(arg & (1 << MouseState::mouseSecondButton)) {
            m_pListener->onButtonAltClick(this);
        }
    }

    return m_StealFlags;
}


void BasicButton::onRender(LPDIRECT3DDEVICE9 pDevice)
{
    // set texture if set
    if(m_ButtonDown && m_pBackgroundTextureClicked)
        pDevice->SetTexture(0, m_pBackgroundTextureClicked);
    else if(m_pBackgroundTexture)
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
        if(m_AlphaBlend)
        {
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }
        pDevice->SetFVF(TRANSLITVERTEX::GetFVF());
        pDevice->SetStreamSource(0, m_pBackgroundVB, 0, sizeof(TRANSLITVERTEX));
        pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
    }
}
