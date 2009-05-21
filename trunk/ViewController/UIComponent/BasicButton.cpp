#include "BasicButton.h"

#include "../Input/MouseState.h"
#include "../Controller/Cursor.h"

BasicButton::BasicButton(const int buttonId, const int width, const int height)
    : UIComponent(0, 0, width, height)
{
    m_pListener = 0;
    m_pBackgroundTextureClicked = 0;
    m_buttonId = buttonId;

    m_BackgroundARGBClicked.x = 0xFFFFFFFF;
    m_BackgroundARGBClicked.y = 0xFFFFFFFF;

    m_StealFlags |= STEAL_MOUSE;

    m_ProcessFlags |= CPROCESS_MOUSE_BUTTONS;
    m_ProcessFlags |= CPROCESS_MOUSE_IDLE; // for tooltips

    m_LayoutFlags = LAYOUT_HINT_ASPECTRATIO;
}


// implement when deriving
// this default action only steals (mouse)
int BasicButton::processEvent(int eventFlag, TCHAR arg)
{
    // only default action currently is to show a possible tooltip on mouse idle
    if(eventFlag == CEVENT_MOUSE_IDLE && m_Tooltip.GetLength() > 0)
        Cursor::getInstance()->setTooltip(m_Tooltip.GetBuffer(), 5.0f);

    if(m_pListener && eventFlag == CEVENT_MOUSE_RELEASED && mouseIntersects()) {
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
