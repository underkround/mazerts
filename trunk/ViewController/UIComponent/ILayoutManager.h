/**
 *
 * Interface for LayoutManager that is a property of UIContainer and friend
 * of UIContainer. It's purpose is to set the sizes and positions of th
 * children components of the container.
 *
 * UIComponent properties that the LayoutManager should be concerned:
 *
 *  UIComponent::getPreferredSize
 *  UIComponent::setSize
 *  UIComponent::setPosition
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __ILAYOUTMANAGER_H__
#define __ILAYOUTMANAGER_H__

class UIContainer;

class ILayoutManager
{

friend class UIContainer;

public:

    ILayoutManager() : m_pParent(0) { }

    virtual ~ILayoutManager();

    /**
     * Redo the layout
     */
    virtual void reLayout() = 0;

protected:

    // the container that we lay out (and that deletes us)
    UIContainer*            m_pParent;

};

#endif // __ILAYOUTMANAGER_H__
