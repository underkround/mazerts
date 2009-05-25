/**
 * BasicButton
 *
 * Simple button that can be clicked, and has click listener.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BASICBUTTON_H__
#define __BASICBUTTON_H__

#include "UIComponent.h"
#include "IButtonListener.h"

class BasicButton : public UIComponent
{
public:

    BasicButton(const int width, const int height, const int buttonId=0, IButtonListener* listener=0);

    inline const int getId() const
    {
        return m_buttonId;
    }

    /**
     * Set a listener for this button.
     * The listener receives calls when the button is clicked.
     * Possible previous listener will be overridden.
     * Give 0 to clear the listener.
     */
    inline void setButtonListener(IButtonListener* listener)
    {
        m_pListener = listener;
    }

    /**
     * Set the background texture for button when it's clicked
     * (when the mouse button is down).
     * The ownership of the texture will not transfer to this component,
     * hence this will not take care of the releasing of the texture.
     *
     * Use resource container in RootComponent plz
     */
    inline void setBackgroundTextureClicked(LPDIRECT3DTEXTURE9 pTexture)
    {
        m_pBackgroundTextureClicked = pTexture;
        //m_Changed = true;
    }

    inline void setBackgroundClicked(const int argb)
    {
        m_BackgroundARGBClicked = (argb & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Process event that the RootContainer (or other central dispatcher)
     * has casted on us.
     * We need to return STEAL-flags to inform what input we steal from
     * others in the control-update chain.
     */
    virtual int processEvent(int eventFlag, TCHAR arg);


    /**
     * Override from UIComponent to support clicked texture
     */
    virtual void BasicButton::onRender(LPDIRECT3DDEVICE9 pDevice);

protected:

    int                 m_buttonId;

    bool                m_ButtonDown; // internal state indicator for alt texture

    IButtonListener*    m_pListener;
    LPDIRECT3DTEXTURE9  m_pBackgroundTextureClicked;

    int                 m_BackgroundARGBClicked; // two color values for gradien (x for solid)

};

#endif // __BASICBUTTON_H__
