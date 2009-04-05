/**
 *
 * Gridlayout is layout manager that layouts the components
 * in grid form.
 * It supports margins and spacing between the component.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __GRIDLAYOUT_H__
#define __GRIDLAYOUT_H__

#include "ILayoutManager.h"
#include "ComponentCommon.h"

class GridLayout : public ILayoutManager
{
public:

    /**
     * Create layout by giving rows and columns.
     * Margins and gaps are set to default (5 pixels).
     */
    GridLayout(const int rows=1, const int cols=0) : ILayoutManager()
    {
        //m_Margins = MarginSet(5,5);
        //m_Gaps(5, 5);
        m_Margins = MarginSet(5);
        m_Gaps = Point2(5);
        setupRowsCols(rows, cols);
    }

    /**
     * Create layout by giving rows, columns, margin to borders and
     * space left between the components
     */
    GridLayout(const unsigned int rows, const unsigned int cols, const unsigned int margin, const unsigned int gap) : ILayoutManager()
    {
        m_Margins = MarginSet(margin);
        m_Gaps = Point2(gap);
        setupRowsCols(rows, cols);
    }

    /**
     * Create layout by giving precise margin and gap -values.
     */
    GridLayout(const unsigned int rows, const unsigned int cols, const MarginSet margins, const Point2 gaps) : ILayoutManager()
    {
        m_Margins = margins;
        m_Gaps = gaps;
        setupRowsCols(rows, cols);
    }

    /**
     * Redo the layout
     */
    virtual void reLayout();

protected:

    /**
     * Setup the form of the grid. Either by rows OR cols (not both).
     */
    void setupRowsCols(const int rows, const int cols)
    {
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

// ===== Members

//    unsigned int        m_Margins;
//    unsigned int        m_Gap;
//    unsigned int        m_Rows;
//    unsigned int        m_Cols;

    // margin-values
    MarginSet       m_Margins;

    // gaps between the components
    Point2          m_Gaps;

    // rows and columns of the grid
    Point2          m_GridSize;

};

#endif // __GRIDLAYOUT_H__
