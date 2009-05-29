/**
 * UIComponent
 *
 * UIComponent is the base class for 2D screen components.
 * This implements the basic things for component and all
 * the components derive from this.
 *
 *  - size, positionion, input-handling
 *
 * Components can be inside IUIContainer and in such case the component's
 * position is relative to parent's position.
 *
 * TODOs:
 *  - preferred dimension
 *  - minimum dimension
 *  - flag for wether the component can be resized
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __UICOMPONENT_H__
#define __UICOMPONENT_H__

#include <d3dx9.h>
#include <TCHAR.h>

#include "ComponentCommon.h"

#include "../App/Vertices.h" // for TRANSLITVERTEX
#include "../Input/MouseState.h"
#include "../../Model/Common/JString.h"

class IUIController;

class UIComponent
{

    friend class UIContainer;

public:

    UIComponent(const int posX=10, const int posY=10, const unsigned int width=20, const unsigned int height=20);

    virtual ~UIComponent()
    {
    }


    /**
     * Process event that the RootContainer (or other central dispatcher)
     * has casted on us.
     * We need to return STEAL-flags to inform what input we steal from
     * others in the control-update chain.
     */
    virtual int processEvent(int eventFlag, TCHAR arg);

// =====
// ===== BEGIN OF METHODS YOU SHOULD IMPLEMENT
// =====

    /**
     * Initialize
     * @param pDevice   Direct3D-device to use
     * @return          S_OK or error code
     */
    virtual HRESULT onCreate(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Called on device lost, releases D3DPOOL_DEFAULT -resources
     */
    virtual HRESULT onDeviceLost();

    /**
     * Called after restoring from device lost, recreates D3DPOOL_DEFAULT-resources
     * @param pDevice Direct3D-device to recreate the resources with
     */
    virtual HRESULT onRestore(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Update call with deltaTime, override if you need one
     * @param frameTime seconds passed since last update call
     */
    virtual void update(const float frameTime) { }

    /**
     * Release resources.
     * Remember to call parent's (this) release when you override!
     */
    virtual void release();

protected:

    /**
     * The implementing concrete component overrides this and does it's rendering
     * here.
     * The default implementation of the component fills the background
     * by style set with background setters.
     * Override this to make own rendering for the component, and call
     * this if you need default background implementation.
     */
    virtual void onRender(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Called when the position or dimension has changed.
     * Rebuild vertex buffer etc. here, according to getPosX/Y() and m_Width/m_Height.
     * This default implementation a solid rectangle background for the component
     * using the background options set by background setters.
     */
    virtual void onChange(LPDIRECT3DDEVICE9 pDevice);

// =====
// ===== END OF METHODS YOU SHOULD IMPLEMENT
// =====

public:

    /**
     * Gets called when the parent container changes (resize atm).
     */
    virtual void onParentChange();

    /**
     * Set loading mode on, or change it's status.
     * If the button is in loadin phase, it's disabled.
     * The phase ends when 100 is given, or clearLoading gets called.
     * @param percentage loading state in 0..100 where 100 = complete, back to normal
     */
    void setLoadingStatus(const int percentage);

    /**
     * Return current loading value
     */
    int getLoadingValue() const;

    /**
     * Set loading mode off
     */
    void clearLoading();

// =====
// ===== BEGIN of default rendering - Colors and filling
// =====

/**
 * Note: read rant @ cpp if interested ;D
 */

    /**
     * Set default background color, which the actual component may or may
     * not use.
     * This sets the background style to solid.
     * @param argb  argb colorvalue
     */
    inline void setBackground(const int argb)
    {
        m_BackgroundTop.x = m_BackgroundTop.y = m_BackgroundBottom.x = m_BackgroundBottom.y = (argb & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Set default background color, which the actual component may or may
     * not use.
     * This sets the background style to solid.
     * @param argbTL    topleft argb
     * @param argbTR    topright argb
     * @param argbBR    bottomright argb
     * @param argbBL    bottomleft argb
     */
    inline void setBackground(const int argbTL, const int argbTR, const int argbBR, const int argbBL)
    {
        m_BackgroundTop.x = (argbTL & 0xFFFFFFFF);
        m_BackgroundTop.y = (argbTR & 0xFFFFFFFF);
        m_BackgroundBottom.x = (argbBL & 0xFFFFFFFF);
        m_BackgroundBottom.y = (argbBR & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Set default background colors for component as
     * horizontal gradient.
     * The actual component may or may not use these values.
     * @param argbT argb value for top
     * @param argbB argb value for bottom
     */
    inline void setBackgroundGradientH(const int argbT, const int argbB)
    {
        m_BackgroundTop.x = m_BackgroundTop.y = (argbT & 0xFFFFFFFF);
        m_BackgroundBottom.x = m_BackgroundBottom.y = (argbB & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Set default background colors for component as
     * vertical gradient.
     * The actual component may or may not use these values.
     * @param argbL argb value for left
     * @param argbR argb value for right
     */
    inline void setBackgroundGradientV(const int argb1, const int argb2)
    {
        m_BackgroundTop.x = m_BackgroundBottom.x = (argb1 & 0xFFFFFFFF);
        m_BackgroundTop.x = m_BackgroundBottom.y = (argb2 & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Set the background texture for the component.
     * The style of the background is set to texture.
     * The concrete component may or may not use this value.
     * The ownership of the texture will not transfer to this component,
     * hence this will not take care of the releasing of the texture.
     *
     * NOTE! use RootContainer's resource container for textures!
     * NOTE2! The background color will affect the texture, set to white
     * for clean
     *
     * @param pTexture texture to use as background, or NULL to clear
     */
    inline void setBackgroundTexture(LPDIRECT3DTEXTURE9 pTexture)
    {
        m_pBackgroundTexture = pTexture;
        m_Changed = true;
    }

    /**
     * Set default foreground color, which the actual component may or may
     * not use.
     * @param argb  argb colorvalue
     */
    inline void setForeground(const int argb)
    {
        m_ForegroundARGB = (argb & 0xFFFFFFFF);
        m_Changed = true;
    }

// =====
// ===== END of default rendering
// =====


// ===== Component methdos, mainly only fo containers to override


    /**
     * Render the component.
     * This is meant to call the concrete component's onRender method, do your
     * rendering there.
     *
     * NOTE: only container should override this, since this has essential
     * functionality, like clipping, that need to be done before and after
     * the concrete component's rendering. This calls the onRender method
     * that normal components should use.
     *
     * @param pDevice Device to render with
     */
    virtual void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Initialize, calls the onCreate.
     * The main reason for the two methods is when you don't use the default
     * rendering, you want to override all of the onFoo methods,
     * and if you derive for example from container, create can contain
     * important stuff..
     * @param pDevice   Direct3D-device to use
     * @return          S_OK or error code
     */
    virtual HRESULT create(LPDIRECT3DDEVICE9 pDevice)
    {
        // basic component does nothing here
        return onCreate(pDevice);
    }

    /**
     * This method does nothing.
     *
     * A bit of hax, or at least not clean implementation, but..
     * ..this is UIContainer's method that needed to be defined here:
     * For avoiding quite pointless cross-references (nothing that could
     * not be solved, but could have been better), I changed the type
     * of the component's parent to be UIComponent rather than UIContainer,
     * and this method was the only used from the UIContainer's interface,
     * and rather than change the parent back to UIContainer, I brought
     * this method level down to here..
     * Feel free to do better =)
     *
     * UPDATE: uncommented.. seems that there is yet another loop-catch
     * that I did: container.release calls children's release -> calls
     * parent container's (same) remove-method x| doh too sleepy to think.
     */
    virtual bool removeComponent(UIComponent* child)
    {
        // basic component does not have children, so no action
        return false;
    }

// ===== Parent

    /**
     * @return the parent component, or NULL if there is no parent
     */
    virtual UIComponent* getParent()
    {
        return m_pParent;
    }


// ===== Positioning


    /**
     * Get snapmargins of this component.
     * SnapMargins (should) work like:
     *     top
     * left    right
     *    bottom
     *
     * Where each value is disabled (<0) by default. If the value is
     * 0 or more, the component tries to maintain given margin to it's
     * parent. Then intent is to create component that can resize when
     * the parent resizes.
     *
     * NOTE: if you set this, you need to call onParentChange() on this
     * component to apply the snap settings!
     *
     * @return  pointer to snapmargin-object, into which you can set the
     *          values
     */
    inline MarginSet* getSnapMargins() {
        return &m_SnapMargins;
    }

    /**
     * @return the absolute position X of the component (in screen space).
     */
    inline const int getPosX() const;

    /**
     * @return the absolute position Y of the component (in screen space).
     */
    inline const int getPosY() const;

    /**
     * One-line-method version of the position getter.
     * Gets the absolute position of the component as screen coordinates
     * (relative to possible parent).
     * @return  struct containing the absolute posX and posY (the screen
     *          position) of the component
     */
    inline const Point2 getPosition() const
    {
        return Point2(getPosX(), getPosY());
    }

    /**
     * One-line-method version of the position getter.
     * @return  struct containing the relative position of the component (in
     *          parent space)
     */
    virtual Point2 getRelativePosition() const  { return Point2(m_Pos.x, m_Pos.y); }
    inline const int getRelativePosX() const    { return m_Pos.x; }
    inline const int getRelativePosY() const    { return m_Pos.y; }

    /**
     * Set the position of the component (relative to parent) in pixels (from
     * upperleft corner).
     * @param x
     * @param y
     */
    virtual const bool setPosition(const int x, const int y)
    {
        m_Pos(x, y);
        m_Changed = true;
        return true;
    }

// ===== Size

    /**
     * @return the preferred size of the component (by default this is the
     *          minimum size that the component allows to be set).
     */
    inline const int getPreferredWidth() const  { return m_PreferredSize.x; }
    inline const int getPreferredHeight() const { return m_PreferredSize.y; }
    inline const Point2 getPreferredSize() const { return Point2(m_PreferredSize.x, m_PreferredSize.y);  }

    /**
     * One-line-method version of the size getter.
     * @param width     set the width of the component to this
     * @param height    set the height of the component to this
     */
    inline const Point2 getSize() const { return Point2(m_Size.x, m_Size.y); }
    inline const int getWidth() const   { return m_Size.x; }
    inline const int getHeight() const  { return m_Size.y; }

    /**
     * Set the size of the component (in pixels)
     * @param width
     * @param height
     */
    virtual const bool setSize(const unsigned int width, const unsigned int height)
    {
        if(m_Size.x != width || m_Size.y != height) {
            m_Size.x = width;
            m_Size.y = height;
            m_Changed = true;
        }
        return m_Changed;
    }

    /**
     * This should be used by the creator to set the size of the component.
     * This is the real size that is maintained whenever possible.
     * @param width     the horizontal size to try to maintain
     * @param height    the vertical size to try to maintain
     */
    inline const void setPreferredSize(const unsigned int width, const unsigned int height)
    {
        m_PreferredSize(width, height);
    }

    /**
     * Helper method to determine if given screen coordinates are inside us.
     * @return      true, if given (absolute) screen coordinates are inside us
     */
    inline const bool intersects(const int x, const int y) const
    {
        int absPX = getPosX();
        int absPY = getPosY();
        return (x > absPX &&
                y > absPY &&
                x < absPX + m_Size.x &&
                y < absPY + m_Size.y) ? true : false;
    }

    /**
     * Helper method to determine if mouse is inside us.
     * @return      true, if mouse is currently inside us
     */
    inline const bool mouseIntersects() const
    {
        return intersects(MouseState::mouseX, MouseState::mouseY);
    }


// ===== Flags & Misc

    /**
     * Return the visibility-state of the component
     * @return true, if the component is visible and rendered
     */
    inline const bool isVisible() const
    {
        return m_Visible;
    }

    /**
     * Set the visibility of the component. If the visible-flag is set to
     * false, the component will not be rendered nor will it take controls
     * (behaviour could be changed by implementing class).
     * @param visible   if set to false, the component will not be rendered
     */
    inline void setVisible(const bool visible)
    {
        m_Visible = visible;
    }

    /**
     * Get focused component by mouse coordinates.
     * In this defaul implementation we trust to be inside container
     * which has already testet that we are under the coordinates, so
     * we don't do intersection test to save resources.
     * @return pointer to this component (no intersection test done)
     */
    virtual UIComponent* getFocus(ProcessFlags processEvent)
    {
        // if we process the requested event, we can have the focus
        return (m_ProcessFlags & processEvent) ? this : NULL;
    }

    inline const int getProcessFlags() { return m_ProcessFlags; }
    inline const int getStealFlags() { return m_StealFlags; }
    inline const int getLayoutFlags() { return m_LayoutFlags; }

    /**
     * Set flags for how the layout should handle this component
     */
    inline void setLayoutFlag(unsigned int flag) { m_LayoutFlags |= flag; }
    inline void unsetLayoutFlag(unsigned int flag) { m_LayoutFlags &= ~flag; }

    /**
     * Set a tooltip for this component, that is displayed when the
     * mouse idles over the component.
     * Set NULL / 0 to clear the tooltip.
     * @param tooltipStr    char-array to use as tooltip
     */
    void setTooltip(const char* tooltipStr)
    {
        m_Tooltip.Set(tooltipStr);
    }

protected:

// ===== Members

    float               m_VertexZ;

    // the parent of this component, or NULL
    UIComponent*        m_pParent;

    // the size that the component prefers to have
    Point2              m_PreferredSize;

    // the component's size properties
    Point2              m_Size;

    // the component's actual positions are relative to it's parent, if any.
    Point2              m_Pos;

    // snap margins override the size settings, and tries to maintain given distance
    // to parent's borders on each side. Snapping per side can be disabled by setting
    // the value for side to <0. Snapping is disabled by default, and may not work
    // with layouts etc... ;P
    MarginSet           m_SnapMargins;

    // flags
    bool                m_Changed;
    bool                m_Visible;
    bool                m_Clipped;

    // tooltip - sorry for using non-unicode, but I HATE THOSE FUCKING T"#¤)("#/¤ CHARS
//    char*               m_pTooltip;
    JString             m_Tooltip;

    // bits for stealed mouse buttons
    int                 m_StealFlags;
    // bits for events to process
    int                 m_ProcessFlags;
    // bits for how layout should handle us
    int                 m_LayoutFlags;

// Members for default background rendering

    int                 m_Loading;  // status for loading phase. if the value is between 0..99,
                                    // loading is on and the button is disabled

    // default colors in 0xAARRGGBB format
    Point2                  m_BackgroundTop; // two color values, x = topleft, y = topright vertex
    Point2                  m_BackgroundBottom; // two color values, x = bottomleft, y = bottomright vertex
    int                     m_ForegroundARGB;
    // texture for the background, or NULL
    LPDIRECT3DTEXTURE9      m_pBackgroundTexture;
    // vertex buffer for default background
    LPDIRECT3DVERTEXBUFFER9 m_pBackgroundVB;

};

#endif // __UICOMPONENT_H__
