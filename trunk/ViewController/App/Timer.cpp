/**
 * Timer.cpp source file
 * Implementation for the CTimer class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 29.1.2009
 * 
 * Generic timer that is capable of computing
 * elapsed seconds in between two function calls
 */

#include "Timer.h"


CTimer::CTimer(void)
{
    m_ddwStartTicks = 0;
    m_ddwTickFrequency = 0;
    m_fElapsedSeconds = 0.0f;
}


CTimer::~CTimer(void)
{
}


void CTimer::Create(void)
{
    // ask the performace timer frequency from the windows
    ::QueryPerformanceFrequency( (LARGE_INTEGER*)&m_ddwTickFrequency);
    m_fElapsedSeconds = 0.0f;
}


void CTimer::Release(void)
{
    // nothing to do really...
}


void CTimer::BeginTimer(void)
{
    // get the current ticks from the system
    ::QueryPerformanceCounter( (LARGE_INTEGER*)&m_ddwStartTicks);
}


void CTimer::EndTimer(void)
{
    if (m_ddwStartTicks == 0)
    {
        // maybe timer was not started???
        m_fElapsedSeconds = 0.0f;
    }
    else
    {
        // compute elapsed seconds
        INT64 ddwEndTicks;
        float ftemp;

        do
        {
            ::QueryPerformanceCounter( (LARGE_INTEGER*)&ddwEndTicks);
            ftemp = (float)(ddwEndTicks - m_ddwStartTicks);
        } while (ftemp < 0.0000001f);

        m_fElapsedSeconds = ftemp / (float)m_ddwTickFrequency;
    }
}


