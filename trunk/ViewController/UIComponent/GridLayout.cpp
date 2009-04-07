/**
 *
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#include "GridLayout.h"
#include "UIContainer.h"

#include "../../Model/Common/DoubleLinkedList.h"

void GridLayout::reLayout()
{
    if(!m_pParent)
        return; // nothing to layout

    const int count = m_pParent->getChildren()->count();

    // maximum child size, used later in many places
    int maxSizeX = 0;
    int maxSizeY = 0;
    int nrows = 1;
    int ncols = 1;

    // if we have any children..
    if(count)
    {

        UIComponent* comp = NULL;
        nrows = m_GridSize.x;
        ncols = m_GridSize.y;
        if(nrows > 0) {
            ncols = (int)((count + nrows - 1) / nrows);
            if(nrows > count)
                nrows = count;
        } else {
            nrows = (int)((count + ncols - 1) / ncols);
            if(ncols > count)
                ncols = count;
        }

        // find max sizes
        // ok, this is two iterations over the components, but this is done relatively
        // seldom - and there is propably less than 10 comps, so it's good for now
        ListNode<UIComponent*>* node = m_pParent->getChildren()->tailNode();
        while(node) {
            Point2 size = node->item->getPreferredSize();
            if(size.x > maxSizeX)
                maxSizeX = size.x;
            if(size.y > maxSizeY)
                maxSizeY = size.y;
            node = node->prev;
        }

        // check sizes - loop
        // check if we should pack the parent or reduce the size of the children
        bool resizeComp = false;
        if(m_pParent->getLayoutFlags() & LAYOUT_HINT_NORESIZE)
        {
            Point2 parSize = m_pParent->getPreferredSize();
            // calculate what the parent's size would be for current components
            int tobeSizeX = m_Margins.left  + maxSizeX * ncols  + (ncols - 1) * m_Gaps.x  + m_Margins.right;
            int tobeSizeY = m_Margins.top   + maxSizeY * nrows  + (nrows - 1) * m_Gaps.y  + m_Margins.bottom;
            // if the size would be bigger, reduce the maxSizes and set components to use it when topping
            if(parSize.x < tobeSizeX) {
                resizeComp = true;
                maxSizeX = (int)( (parSize.x - ( (ncols-1) * m_Gaps.x  +  m_Margins.left  +  m_Margins.right )) / ncols );
            }
            if(parSize.y < tobeSizeY) {
                resizeComp = true;
                maxSizeY = (int)( (parSize.y - ( (nrows-1) * m_Gaps.y  +  m_Margins.top   +  m_Margins.bottom )) / nrows );
            }
        }

        // resize - loop
        // one more loop.. what do we care if it looks decent ;FF
        node = m_pParent->getChildren()->tailNode();
        while(node)
        {
            comp = node->item;
            Point2 compPrefSize = comp->getPreferredSize();
            int newSizeX = compPrefSize.x;
            int newSizeY = compPrefSize.y;
            // resize the children if parent is too selfish to give space. what a lousy parent.
            if(compPrefSize.x > maxSizeX || compPrefSize.y > maxSizeY && !(comp->getLayoutFlags() & LAYOUT_HINT_NORESIZE))
            {
                newSizeX = (compPrefSize.x > maxSizeX) ? maxSizeX : compPrefSize.x;
                newSizeY = (compPrefSize.y > maxSizeY) ? maxSizeY : compPrefSize.y;
                // maintain the aspect ratio
                if(comp->getLayoutFlags() & LAYOUT_HINT_ASPECTRATIO)
                {
                    float aspRatio = (float)(compPrefSize.x / compPrefSize.y);
                    if((newSizeX / aspRatio) < newSizeY)
                        newSizeY = (int)(newSizeX / aspRatio);
                    else
                        newSizeX = (int)(newSizeY * aspRatio);
                }
            }
            comp->setSize(newSizeX, newSizeY);
            node = node->prev;
        }

        // the reposition loop! :D
        int offsetX = 0;
        int offsetY = 0;
        node = m_pParent->getChildren()->tailNode();
        for(int row=0; row<nrows; ++row) {
            for(int col=0; col<ncols; ++col) {
                if(node) {
                    comp = node->item;
                    Point2 compSize = comp->getSize();
                    offsetX = (int)((maxSizeX - compSize.x) / 2 + (col * m_Gaps.x));
                    offsetY = (int)((maxSizeY - compSize.y) / 2 + (row * m_Gaps.y));
                    if(!(comp->getLayoutFlags() & LAYOUT_HINT_NOREPOS))
                    {
                        comp->setPosition(  m_Margins.left + offsetX + maxSizeX * col,
                                            m_Margins.top + offsetY + maxSizeY * row);
                    }
                    node = node->prev;
                } else {
                    // out of components, break from loop
                    row = nrows;
                    col = ncols;
                }
            }
        }

    } // end of if(count)

    // check if we should pack the parent
    if(!(m_pParent->getLayoutFlags() & LAYOUT_HINT_NORESIZE))
    {
        const int totalSizeX = m_Margins.left  + maxSizeX * ncols  + (ncols - 1) * m_Gaps.x  + m_Margins.right;
        const int totalSizeY = m_Margins.top   + maxSizeY * nrows  + (nrows - 1) * m_Gaps.y  + m_Margins.bottom;
        const Point2 prefSize = m_pParent->getPreferredSize();
        const bool pack = (m_pParent->getLayoutFlags() & LAYOUT_HINT_PACK) ? true : false;

        // if (content size bigger than preferred size) -> [set to content size]
        //     else if (pack) [set to content size] else [set to preferred size]
        int newSizeX = (totalSizeX > prefSize.x) ? totalSizeX : (pack) ? totalSizeX : prefSize.x;
        int newSizeY = (totalSizeY > prefSize.y) ? totalSizeY : (pack) ? totalSizeY : prefSize.y;

        m_pParent->setSize(newSizeX, newSizeY);
    }
}
