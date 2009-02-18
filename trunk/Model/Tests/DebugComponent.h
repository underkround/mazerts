/**
 * DebugComponent
 *
 * Component for testing.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __DEBUGCOMPONENT_H__
#define __DEBUGCOMPONENT_H__

#include "../Unit/IComponent.h"

#include <iostream>
using namespace std;

class DebugComponent : public IComponent
{
public:

    inline DebugComponent(const int mode)
    {
        m_Id = counter;
        ++counter;
        cout << "(" << m_Id << ") DebugComponent: CONSTRUCT\n";
        m_Unit = NULL;
        m_T = Terrain::getInstance();
        setMode(mode);
    }

    virtual ~DebugComponent(void)
    {
        cout << "(" << m_Id << ") DebugComponent: DESTRUCT\n";
    }

    inline virtual char update(const float deltaT)
    {
        switch(m_Mode)
        {
            // sweep x -mode
        case 1:
            if(m_T->isPassable(m_Unit->getCellX() + m_DirX, m_Unit->getCellY(), m_Unit->getWidth())) {
                m_Unit->forcePosition(m_Unit->getCellX() + m_DirX, m_Unit->getCellY());
                //cout << "moved unit\n";
            }
            else {
                m_DirX = -m_DirX;
                cout << "(" << m_Id << ") changing direction X to " << m_DirX << "\n";
            }
            break;

            // sweep y -mode
        case 2:
            if(m_T->isPassable(m_Unit->getCellX(), m_Unit->getCellY() + m_DirY, m_Unit->getWidth())) {
                m_Unit->forcePosition(m_Unit->getCellX(), m_Unit->getCellY() + m_DirY);
                //cout << "moved unit\n";
            }
            else {
                m_DirY = -m_DirY;
                cout << "(" << m_Id << ") changing direction Y to " << m_DirY << "\n";
            }
            break;

        default:
            cout << "(" << m_Id << ") in not-doing-anything-mode\n";
            break;
        }
        return 0;
    }

    inline virtual void attach(Unit* host)
    {
        m_Unit = host;
    }

    inline virtual bool detach(Unit* host)
    {
        m_Unit = NULL;
        return true;
    }

    inline void setMode(int mode)
    {
        switch(mode)
        {
        case 1:
            m_DirX = 1; m_DirY = 0;
            break;
        case 2:
            m_DirX = 0; m_DirY = 1;
            break;
        default:
            m_DirX = 0; m_DirY = 0;
            break;
        }
        cout << "(" << m_Id << ") Changing to mode " << mode << "\n";
        m_Mode = mode;
    }

    static int counter;

private:

    int m_Id;
    int m_DirX;
    int m_DirY;
    Unit* m_Unit;
    Terrain* m_T;
    int m_Mode;

};

int DebugComponent::counter = 0;

#endif // __DEBUGCOMPONENT_H__
