/**
 * UIContainer
 *
 * UIContainer is the base class for UIComponent that contain's child
 * components.
 *
 * Panel etc. derive from this.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#include "UIComponent.h"
#include "../../Model/Common/DoubleLinkedList.h"

class UIContainer : public UIComponent
{
public:

    /**
     * The ctor, Get Born Again \m/
     */
    UIContainer();

    virtual ~UIContainer()
    {
    }

// ===== Public container methods

    /**
     * Add child component inside this component.
     * This will set this as the parent of the child.
     * By default when this container is released, it
     * releases and deletes the child components.
     *
     * @param child component to be added as child of this
     * @return      true, if the component was added
     */
    virtual bool addComponent(UIComponent* child);

    /**
     * Removes the child component if this is it's parent.
     * DOES NOT release and delete the child.
     *
     * @see releaseComponent()
     * @param child component to be removed from this
     * @return      true, if the child was found and removed
     */
    virtual bool removeComponent(UIComponent* child);

    /**
     * Remove, release and delete the child component if this is
     * it's parent.
     *
     * @see removeComponent()
     * @param child component to be released and deleted
     * @return      true, if the child was found and released
     */
    virtual bool releaseComponent(UIComponent* child);

    /**
     * Set container transparent
     * Transparent container is not rendered, nor does it steal input.
     * In contrast to visibility-flag, transparency does not affect the
     * rendering of the children.
     *
     * @param transparent   true to set this container transparent
     */
    void setTransparent(const bool transparent)
    {
        m_Transparent = transparent;
    }

    /**
     * Determine if the container is transparent (not rendered).
     * The transparency will not affect the rendering of the children,
     * so transparent container can contain visible components.
     * Transparent container does not steal input.
     *
     * @return      true, if this container is set to transparent
     */
    const bool isTransparent() const
    {
        return m_Transparent;
    }

// ===== Public component methods

    /**
     * Render self..
     * UPDATE: Changed to use the default implementation of the IUIComponent
     */
    //virtual void onRender(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Render self (if visible and not transparent).
     * Render children (if visible).
     * This does the clipping by the clipping-flag, or skips the whole
     * rendering in cases when the component is not visible
     * @see IUIComponent::render
     * @param pDevice Device to render with
     */
    virtual void render(LPDIRECT3DDEVICE9 pDevice);

    /**
     * Dispatch the update call to children, if we are visible
     * @see IUIComponent::update
     * @param frameTime seconds passed since last update call
     */
    virtual void update(const float frameTime);

    /**
     * Set the size of the component (in pixels)
     * @see IUIComponent::setSize
     * @param width
     * @param height
     */
    virtual const bool setSize(const int width, const int height);

    /**
     * Release resources and children.
     * instead of this. Only containers should override this.
     * @see IUIComponent::release
     */
    virtual void release();

    /**
     * Return component from mouse coordinates, either this or child
     */
    virtual UIComponent* getFocus();

    virtual int processEvent(int eventFlag, TCHAR arg);

protected:

// ===== Protected component methods

    /**
     * This gets called when one of these happen:
     *  - container's position or dimension is changed
     *  - child's position or dimension has changed
     *  - new child is added
     *  - layout has changed
     * @see IUIComponent::onChange
     */
    virtual void onChange(LPDIRECT3DDEVICE9 pDevice)
    {
        UIComponent::onChange(pDevice);
        // TODO: layout-stuff ?
    }

// ===== Protected container methods

// ===== Members

    DoubleLinkedList<UIComponent*>  m_Children;

    // transparent component does not render itself nor handle the controls
    bool            m_Transparent;

};

#endif // __UICONTAINER_H__
