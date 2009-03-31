/**
 * Common structs and defines for components
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __COMPONENTCOMMON_H__
#define __COMPONENTCOMMON_H__


// button used to transfer the focus
#define MOUSE_FOCUS_BUTTON 0


/**
 * Filling styles for the components
 */
enum ComponentFillStyle
{
    FILLSTYLE_SOLID = 1,    // single color
    FILLSTYLE_GRADIENT_H,   // horizontal gradient
    FILLSTYLE_GRADIENT_V,   // vertical gradient
    FILLSTYLE_TEXTURE       // texture, if set
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

    MarginSet() : top(0), right(0), bottom(0), left(0) { }

    MarginSet(int t, int r, int b, int l) : top(t), right(r), bottom(b), left(l) { }

    MarginSet(int horizontal, int vertical) : top(vertical), bottom(vertical), left(horizontal), right(horizontal) { }

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

    Point2() : x(0), y(0) { }
    Point2(int px, int py) : x(px), y(py) { }

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
