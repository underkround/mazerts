/**
 * Test-component
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __DUMMYCOMPONENT_H__
#define __DUMMYCOMPONENT_H__

#include "UIComponent.h"
#include "../App/ID3DApplication.h"

class DummyComponent : public UIComponent
{
public:

    DummyComponent(int x=0, int y=0, int w=20, int h=20);

    virtual ~DummyComponent()
    {
    }

    enum TestMode
    {
        TESTMODE_NONE = 1,
        TESTMODE_BOUNCER,
        TESTMODE_TRANSPARENT_TOGGLER
    };

    virtual int processEvent(int eventFlag, TCHAR arg);

    virtual void onRender(LPDIRECT3DDEVICE9 pDevice);

    virtual void update(const float frameTime);

    void setupTransparencyToggler()
    {
        m_Mode = TESTMODE_TRANSPARENT_TOGGLER;
    }

    void setupBouncer()
    {
        m_Mode = TESTMODE_BOUNCER;
    }

private:

    bool m_Bouncing;
    ID3DApplication* m_App; // for testing text rendering.. this is not a good way though

    TestMode m_Mode; // flags for testing functionality

    // temporary moving values
    int m_ModX;
    int m_ModY;

    // counter for moving speed
    float downcounter;
    float downcounterStep;

};

#endif // __DUMMYCOMPONENT_H__
