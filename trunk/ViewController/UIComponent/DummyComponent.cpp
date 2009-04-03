#include "DummyComponent.h"

#include "../Input/MouseState.h"

#include "../App/Vertices.h"
#include <tchar.h>

#include "../App/IApplication.h" // for random

DummyComponent::DummyComponent(int x, int y, int w, int h) : UIComponent()
{
    m_Pos(x, y);
    m_Size(w, h);
    downcounter = 0.0f;
    downcounterStep = 0.01f;

    m_ModX = IApplication::RandInt(1, 4);
    m_ModY = IApplication::RandInt(1, 4);

    m_Bouncing = false;
    m_Mode = TESTMODE_NONE;
    m_App = NULL;
}


void DummyComponent::onRender(LPDIRECT3DDEVICE9 pDevice)
{
    // parent rendering
    UIComponent::onRender(pDevice);
    // own additional rendering

    /*
    // test label rendering, will this be slow or something?
    if (m_pSprite)
    {
        m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
        m_pSprite->End();
    } */
}


void DummyComponent::update(const float frameTime)
{
    if(!m_Bouncing)
        return;

    Point2 min;
    Point2 max;

    if(m_pParent)
    {
        min(0, 0);
        max.x = m_pParent->getWidth() - m_Size.x;
        max.y = m_pParent->getHeight() - m_Size.y;
    }
    else
    {
        min.x = 0;
        min.y = 0;
        max.x = 1024 - m_Size.x;
        max.y = 768 - m_Size.y;
    }

    downcounter -= frameTime;
    if(downcounter <= 0)
    {
        downcounter = downcounterStep;

        m_Pos.x += m_ModX;
        if(m_Pos.x < min.x)
        {
            m_ModX = IApplication::RandInt(1, 4);
            m_Pos.x = min.x;
        }
        else if(m_Pos.x > max.x)
        {
            m_ModX = -IApplication::RandInt(1, 4);
            m_Pos.x = max.x;
        }

        m_Pos.y += m_ModY;
        if(m_Pos.y < min.y)
        {
            m_ModY = IApplication::RandInt(1, 4);
            m_Pos.y = min.y;
        }
        else if(m_Pos.y > max.y)
        {
            m_ModY = -IApplication::RandInt(1, 4);
            m_Pos.y = max.y;
        }
    }
    m_Changed = true;
}

int DummyComponent::processEvent(int eventFlag, TCHAR arg)
{
//        setBackground(IApplication::RandInt(0xAA000000, 0xFF000000) + IApplication::RandInt(0x000000, 0xFFFFFF));
    if(eventFlag & CEVENT_MOUSE_PRESSED)
    {
        setBackground(0xAAFF0000);
    }
    if(eventFlag & CEVENT_MOUSE_DRAGGED)
    {
        setBackground(0xAA00FF00);
    }
    if(eventFlag & CEVENT_MOUSE_RELEASED)
    {
        setBackground(0xAA0000FF);
    }
/*
    switch(m_Mode)
    {

    case TESTMODE_TRANSPARENT_TOGGLER:
        if(m_pParent)
        {
            UIContainer* parent = (UIContainer*)m_pParent;
            parent->setTransparent(!parent->isTransparent());
        }
        break;

    case TESTMODE_BOUNCER:
        m_Bouncing = !m_Bouncing;
        break;

    default: break;
    } // switch
*/
    return STEAL_MOUSE;
}
