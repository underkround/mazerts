/**
 *
 * TextComponent extends the functionality of the UIComponent
 * to include text rendering capabilities.
 *
 * This is done in separate class mostly to avoid bloating the
 * component for optional features, and secondly to help to
 * concentrate the implementation to one place since the
 * implementation likely to be changed if we'll decide to go
 * with textured fonts..
 *
 * The current implementation uses the I3DApplication to render
 * it's texts.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __TEXTCOMPONENT_H__
#define __TEXTCOMPONENT_H__

#include "UIComponent.h"

#include "../App/ID3DApplication.h" // for text rendering

class TextComponent : public UIComponent
{
public:

    TextComponent() : UIComponent()
    {
    }

    virtual ~TextComponent()
    {
    }

    /**
     * Public method to set label for the component
     * The position and handling of the label might depend from
     * the implementation.
     * This overrides the previously set label.
     */
    void setLabel(LPCTSTR strLabel);
    void setLabel(int intLabel);

    inline const LPCTSTR getLabel() const
    {
        return m_Label;
    }

    /**
     * Set a tooltip for the button.
     * The tooltip *should* popup when mouse idle's over the component,
     * but by the time writing this, it's really not implemented ;)
     * TODO TODO
     */
    void setTooltip(LPCTSTR strTooltip);

    inline const LPCTSTR getTooltip() const
    {
        return m_Tooltip;
    }

    /**
     * Should be called between beginText and EndText by the
     * I3DApplication in the rendering.
     * Renders all the texts that textcomponents have queued.
     */
    static void renderText(ID3DApplication* app);

protected:

    inline void setTextOffset(int x, int y)
    {
        m_TextOffset.x = x;
        m_TextOffset.y = y;
    }

    /**
     * Text drawing.
     * By default the line changes after each call
     */
//    static void drawText(char* str);
//    static void drawText(int num);

// ===== Members

    // text offset inside our position
    Point2          m_TextOffset;

    // the label of the component
    LPCTSTR         m_Label;
    LPCTSTR         m_Tooltip;

};

#endif // __TEXTCOMPONENT_H__
