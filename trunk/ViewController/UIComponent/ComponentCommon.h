/**
 * Common structs and defines for components
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __COMPONENTCOMMON_H__
#define __COMPONENTCOMMON_H__

#include <TCHAR.h>


/**
 * Flags for the layout, what it should and should not resize/relocate.
 */
enum ComponentLayoutFlag
{
    LAYOUT_HINT_PACK        = 1 << 0,   // swing-inspired name, resize parent by children (even if smaller than preferred size)
                                        // if the resize is allowed and PACK is NOT set, the parent will be set to it's preferred size

    LAYOUT_HINT_NORESIZE    = 1 << 1,   // no resizing allowed for component
    LAYOUT_HINT_NOREPOS     = 1 << 2,   // no repositioning allowed for component
    LAYOUT_HINT_ASPECTRATIO = 1 << 7    // maintain the component's aspect ratio
};


/**
 * Result flags for component's event process method, flags indicate
 * whan the component stole, and what should not be dispatched to
 * others.
 */
enum StealFlags
{
    STEAL_NONE      = 0,        // empty value to return, or just 0
    STEAL_MOUSE     = 1 << 0,   // steal mouse
    STEAL_KEYBOARD  = 1 << 1,   // steal all keys
    STEAL_TEXT      = 1 << 2,   // steal just text keys
    STEAL_MOUSE_OUTSIDE = 1 << 5 // internal component specifig hint
};


/**
 * Flags for what events does the components process
 */
enum ProcessFlags
{
    CPROCESS_MOUSE_BUTTONS  = 1 << 0,
    CPROCESS_MOUSE_IDLE     = 1 << 1,
    CPROCESS_MOUSE_WHEEL    = 1 << 2,
    CPROCESS_TEXT           = 1 << 2,   // process text keys
    CPROCESS_KEYBOARD       = 1 << 3    // process all keys
};


/**
 * Id's for events that are sent to the focused component to process
 */
enum EventFlags
{
    CEVENT_NONE             = 0,
    CEVENT_OUTSIDE          = 1 << 0,   // action happened outside
    // mouse button events
    CEVENT_MOUSE_PRESSED    = 1 << 1,
    CEVENT_MOUSE_RELEASED   = 1 << 2,
    CEVENT_MOUSE_DRAGGED    = 1 << 3,
    CEVENT_MOUSE_IDLE       = 1 << 4,
    // mouse motion events
    CEVENT_MOUSE_ENTERED    = 1 << 5,
    CEVENT_MOUSE_EXITED     = 1 << 6,
    // mouse wheel events
    CEVENT_MOUSE_WHEEL      = 1 << 7,
    // text events
    CEVENT_CHAR_TYPED       = 1 << 8,   // typed char event
    // keypress events
    CEVENT_KEY_PRESSED      = 1 << 9,   // general keypress event
    CEVENT_KEY_RELEASED     = 1 << 10   // general keyrelease event
};


/**
 * Define spaces inside the container and between it's children.
 * Use negative values to preserve the old ones.
 */
class MarginSet
{
public:

    int top;
    int right;
    int bottom;
    int left;

    MarginSet()
        : top(0), right(0), bottom(0), left(0) { }

    MarginSet(const int top, const int right, const int bottom, const int left)
        : top(top), right(right), bottom(bottom), left(left) { }

    MarginSet(const int horizontal, const int vertical)
        : top(vertical), bottom(vertical), left(horizontal), right(horizontal) { }

    MarginSet(const int margin)
        : top(margin), bottom(margin), left(margin), right(margin) { }
};


/**
 * All the dimensional values in components are paired,
 * so let's make struct for it. Hope the name won't
 * be duplicate somewhere ;O
 */
class Point2
{
public:

    int x;
    int y;

    Point2()
        : x(0), y(0) { }

    Point2(const int xy)
        : x(xy), y(xy) { }

    Point2(const int px, const int py)
        : x(px), y(py) { }

    Point2 Point2::operator()(int px, int py) {
        x = px;
        y = py;
        return *this;
    }

    Point2 Point2::operator()(Point2& p2) {
        x = p2.x;
        y = p2.y;
        return *this;
    }

    Point2 Point2::operator+(Point2 p2) {
        Point2 temp(p2.x + x, p2.y + y);
        return temp;
    }

    bool Point2::operator==(const Point2 &p2) const {
        return (x == p2.x && y == p2.y) ? true : false;
    }
};


#endif // __COMPONENTCOMMON_H__
