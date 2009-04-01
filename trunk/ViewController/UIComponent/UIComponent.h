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
#include "ComponentCommon.h"

#include "../App/Vertices.h" // for TRANSLITVERTEX
#include "../Input/MouseState.h"

class IUIController;

class UIComponent
{

    friend class UIContainer;

public:

    UIComponent();

    virtual ~UIComponent()
    {
    }

    enum EventFlags
    {
        C_EVENT_NONE            = 0, // do not steal the input
        C_EVENT_MOUSE_DOWN      = 1 << 0, // mouse button is down
        C_EVENT_MOUSE_PRESSED   = 1 << 1, // mouse button is just being pressed
        C_EVENT_MOUSE_RELEASED  = 1 << 2, // mouse button is just being released
        C_EVENT_OUTSIDE         = 1 << 3  // event happened outside the component
        // keyboard-events?
    };

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
    virtual void update(const float frameTime)
    {
    }

    /**
     * Update call for focused component to act on input.
     *
     * Receive input-time and handle input if focused by upper level component.
     *
     * The return value is used for determing if the component steals the
     * control. If true is returned, it means that the component has exclusive
     * control and others should not handle the input (if return is false,
     * the component is "transparent" in sense of controls and the input handling
     * chain should continue.
     *
     * This default implementation steals the control (returns true) for
     * mouse buttons defined for this component (default or overridden).
     * with actions: pressed, down or released. It's not checked wether the
     * action happens in- or outside this component, do it in your own
     * implementation with:
     *
     * if(intersects(MouseState::mouseX, MouseState::mouseY))
     *     ; // action is inside us
     *
     * Mouse movement with buttons up (or buttons down that we are not set to
     * handle) will fall through this (returns false).
     *
     * You can use this from deriving class to quick check for mouse down/released
     * inside the component, and then possibly determine the actual action to be
     * performed.
     *
     * example:
     *
     * class SomeButton : public UIComponent {
     *     SomeButton() : UIComponent() {
     *         registerMouseButton(1); // listen to mouse button 1
     *     }
     *     virtual bool updateControls(const float frameTime) {
     *         if(!UIComponent::updateControls(frameTime)) {
     *             return false; // no mouse down event that was setinside us
     *         }
     *         if(!mouseIntersects()) {
     *             return true; // no action but we still steal the input
     *         }
     *         // mouse down event with button 0
     *         // determine which action (pressed, released..)
     *         // and what you intented to happen
     *     }
     * }
     *
     * @param frameTime Seconds passed since last frame (possibly NOT the last
     *                  call to this method). Use the update()-method for that.
     * @return          True, if the component handled the input and the
     *                  handling should not be passed to others -> input stealing
     *                  False means the controls will be passed on on upper levels.
     */
    virtual bool updateControls(const float frameTime);

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
        m_BackgroundARGB.x = (argb & 0xFFFFFFFF);
        m_BackgroundARGB.y = (argb & 0xFFFFFFFF);
        m_BackgroundStyle = FILLSTYLE_SOLID;
        m_Changed = true;
    }

    /**
     * Set default background colors for component.
     * The actual component may or may not use these values.
     * @param argb1 argb colorvalue 1
     * @param argb2 argb colorvalue 2
     */
    inline void setBackground(const int argb1, const int argb2)
    {
        m_BackgroundARGB.x = (argb1 & 0xFFFFFFFF);
        m_BackgroundARGB.y = (argb2 & 0xFFFFFFFF);
        m_Changed = true;
    }

    /**
     * Set the background texture for the component.
     * The style of the background is set to texture.
     * The concrete component may or may not use this value.
     * The ownership of the texture will not transfer to this component,
     * hence this will not take care of the releasing of the texture.
     */
    inline void setBackground(LPDIRECT3DTEXTURE9 pTexture)
    {
        m_pBackgroundTexture = pTexture;
        if(pTexture)
            m_BackgroundStyle = FILLSTYLE_TEXTURE;
        else if(m_BackgroundStyle = FILLSTYLE_TEXTURE)
            m_BackgroundStyle = FILLSTYLE_GRADIENT_V;
    }

    /**
     * Set the style for the component's background filling.
     * The actual component may or may not use these values.
     * @param style coloring style of the component background
     */
    inline void setBackgroundStyle(const ComponentFillStyle style)
    {
        if(style == FILLSTYLE_TEXTURE && !m_pBackgroundTexture)
            return; // no texture style if there's no texture!
        m_BackgroundStyle = style;
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
        return false;
    }


// ===== Controller support


    /**
     * Set the controller which receives updateControls -calls when the
     * component is active.
     *
     * NOTE: the ownership of the controller is not transferred, so you
     * need to delete the controller elsewhere, this won't do it.
     */
    virtual void setController(IUIController* controller)
    {
        // TODO?
        m_pController = controller;
    }

    /**
     * Get the attached controller, or NULL if none
     */
    inline IUIController* getController()
    {
        return m_pController;
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
     * @return the relative position X of the component (in parent space)
     */
    inline const int getRelativePosX() const
    {
        return m_Pos.x;
    }

    /**
     * @return the relative position Y of the component (in parent space)
     */
    inline const int getRelativePosY() const
    {
        return m_Pos.y;
    }

    /**
     * One-line-method version of the position getter.
     * @return  struct containing the relative position of the component (in
     *          parent space)
     */
    virtual Point2 getRelativePosition() const
    {
        return Point2(m_Pos.x, m_Pos.y);
    }

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
     * @return the width of the component
     */
    inline const int getWidth() const
    {
        return m_Size.x;
    }

    /**
     * @return the height of the component
     */
    inline const int getHeight() const
    {
        return m_Size.y;
    }

    /**
     * One-line-method version of the size getter.
     * @param width     set the width of the component to this
     * @param height    set the height of the component to this
     */
    inline const Point2 getSize() const
    {
        return Point2(m_Size.x, m_Size.y);
    }

    /**
     * Set the size of the component (in pixels)
     * @param width
     * @param height
     */
    virtual const bool setSize(const int width, const int height)
    {
        bool changed = false;
        if(width >= m_MinSize.x) {
            m_Size.x = width;
            changed = true;
        }
        if(height >= m_MinSize.y) {
            m_Size.y = height;
            changed = true;
        }
        if(changed)
            m_Changed = true;
        return changed;
    }


// ===== Input control


    /**
     * Set this container to steal given mouse button.
     * @param buttonId  id for mouse button to steal
     */
    inline void stealMouseButton(int buttonId)
    {
        m_StealedMouseButtons |= (1 << buttonId);
    }

    /**
     * Unset this container to not to steal previously registered mouse button.
     * @param buttonId  id for mouse button to take stealing away from
     */
    inline void unstealMouseButton(int buttonId)
    {
        m_StealedMouseButtons &= ~(1 << buttonId);
    }

    /**
     * Return which mouse buttons are currently stealed.
     * The return value is int containing bits for buttons that
     * are catched/stealed. For example bit 1 in index 2 means
     * that mouse button 2 is stealed by this component when
     * it's focused.
     */
    inline const int getStealedMouseButtons() const
    {
        return m_StealedMouseButtons;
    }

    /**
     * Return registered status for mouse button
     */
    inline const bool isMouseButtonStealed(int buttonId) const
    {
        return (m_StealedMouseButtons & (1 << buttonId)) ? true : false;
    }

    /**
     * Return the input events that are on that this component catches.
     * The return contains event-flags from Events enum.
     * @return Events-flags that are on and this component catches
     */
    virtual const int stealableEvents() const
    {
        // check if there is stealable mouse buttons pressed or released
        int flags = C_EVENT_NONE;
        if(MouseState::mouseButtonBits & m_StealedMouseButtons)
            flags |= C_EVENT_MOUSE_DOWN;
        if(MouseState::mouseButtonPressedBits & m_StealedMouseButtons)
            flags |= C_EVENT_MOUSE_PRESSED;
        if(MouseState::mouseButtonReleasedBits & m_StealedMouseButtons)
            flags |= C_EVENT_MOUSE_RELEASED;
        return flags;
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


// ===== Flags


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

protected:

    /**
     * Notification of gained and lost focuses, mainly only for use
     * of the basic-components (IUIComponent and IUIContainer) to
     * dispatch the lost focus by many levels in cases where
     * some nesting component is focused, and it's parent loses it's
     * focus. When the parent get's it's focus back, it's child would
     * be automaticly focused again and there could be problems - click
     * on another child would not be passed.
     *
     * These methods are in Component rather than Container since the
     * Container handles components (which can actually be containers).
     */
    virtual void focusLost() { m_Focused = false; }
    virtual void focusGain() { m_Focused = true; }

// ===== Members

    // the parent of this component, or NULL
    UIComponent*        m_pParent;

    // the input controller associated with this component, or NULL
    IUIController*      m_pController;

    // the minimum bounds of the component
    Point2              m_MinSize;

    // the component's size properties
    Point2              m_Size;

    // the component's actual positions are relative to it's parent, if any.
    Point2              m_Pos;

    // flags
    bool                m_Changed;
    bool                m_Visible;
    bool                m_Clipped;

    // flag for own focus control
    bool                m_Focused;

    // bits for stealed mouse buttons
    int                 m_StealedMouseButtons;


// Members for default background rendering

    // default colors in 0xAARRGGBB format
    Point2                  m_BackgroundARGB; // two color values for gradien (x for solid)
    int                     m_ForegroundARGB;
    // style of the background filling
    ComponentFillStyle      m_BackgroundStyle;
    // texture for the background, or NULL
    LPDIRECT3DTEXTURE9      m_pBackgroundTexture;
    // vertex buffer for default background
    LPDIRECT3DVERTEXBUFFER9 m_pBackgroundVB;

};

#endif // __UICOMPONENT_H__