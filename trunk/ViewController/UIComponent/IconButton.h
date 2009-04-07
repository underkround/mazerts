/**
 *
 * BasicButton is class that can have dual-functionality with two mouse
 * buttons, and callbacks for clicks to them.
 *
 * This derives from TextComponent, and aligns the text-label for the
 * intented use with build-counter-number in mind.
 *
 * If you use this as asset-construction-buttons, set the class doing
 * the actual building as handler, and set the buttonTag so that you
 * can determine where the action came from.
 *
 * Then set appropriate texture for the button, and modify the label.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BASICBUTTON_H__
#define __BASICBUTTON_H__

#include "TextComponent.h"
#include "IButtonListener.h"

class BasicButton : public TextComponent
{
public:

    BasicButton(IButtonListener* listener, int buttonTag=0)
        : TextComponent(), m_ButtonTag(buttonTag), m_pListener(listener)
    {
    }

    ~BasicButton()
    {
    }

    /**
     * Get the associated tag (id)
     */
    inline const int getButtonTag() const
    {
        return m_ButtonTag;
    }

protected:

    int                 m_ButtonTag;
    IButtonListener*    m_pListener;

};

#endif // __BASICBUTTON_H__
