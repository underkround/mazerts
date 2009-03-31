/**
 * GridPanel is a concrete UIContainer which stores
 * it's children in grid form.
 *
 * Actually the layout should be implemented in separate LayoutManager,
 * but somehow I'm afraid I would get some hints from the outside that
 * this approach would be "overkill" or somethin ;(
 *
 * So for now I'll hardcode the basic grid-style layout into this
 * container :\ (Maybe I'll do the manager later in secret ^^)
 *
 * The grid's rows and columns, as well as margin values,
 * can be defined from outside.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __GRIDPANEL_H__
#define __GRIDPANEL_H__

#include "UIContainer.h"

class GridPanel : public UIContainer
{
public:

    /**
     * Create by setting _either_ rows or columns for this gridpanel.
     *
     * The number of rows sets how many components there can be side by
     * side in horizontal direction, and the number of columns limits the
     * count of the components in vertical direction.
     *
     * One of the value should be set to positive number, and the other
     * to zero (it will be ignored and real value depends from the count
     * of the items).
     * If both of the numbers are non-zero, cols will be ignored.
     */
    GridPanel(int rows=1, int cols=0) : UIContainer()
    {
        m_Margins = MarginSet(5,5);
        m_Gaps(5, 5);
        // by rows
        if(rows > 0)
            m_GridSize(rows, 0);
        // by columns
        else if(cols > 0)
            m_GridSize(0, cols);
        // the default
        else
            m_GridSize(1, 0);
    }

    virtual ~GridPanel()
    {
    }

    /**
     * Set the margins for container.
     * Give negative value to preserve the old value.
     * @param margins   struct containing new margins for this container
     */
    void setMargins(const MarginSet margins);

    /**
     * Get margin values of this container.
     * If you modify the margins, you need to pass the modified set to
     * this container via setMargins -method.
     * @return  struct containing current margins of this container
     */
    MarginSet getMargins() const
    {
        return m_Margins;
    }

protected:

// ===== Grid methods

    /**
     * Layout (change position + size) the components to match the size
     * of this panel.
     */
    void layoutByComponents();

    /**
     * Same as layoutComponents, but respects (does not change) the size
     * of the components, and rather resizes the panel to fit the content.
     */
    void layoutByPanel();

// ===== Members

    // margin-values
    MarginSet       m_Margins;

    // gaps between the components
    Point2          m_Gaps;

    // rows and columns of the grid
    Point2          m_GridSize;

};

#endif // __GRIDPANEL_H__
