/**
 * Timer.h header file
 * Interface for the CTimer class
 * Copyright (c) 2009 Jani Immonen
 * www.jani-immonen.net
 * Date: 29.1.2009
 * 
 * Generic timer that is capable of computing
 * elapsed seconds in between two function calls
 */


#ifndef __TIMER_H__
#define __TIMER_H__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CTimer
{
public:
    CTimer(void);
    virtual ~CTimer(void);

    /**
     * Create
     * initialise the timer
     */
    void Create(void);

    /**
     * Release
     * release the timer
     */
    void Release(void);

    /**
     * BeginTimer
     * starts calculating the time in between
     * BeginTimer and EndTimer
     */
    void BeginTimer(void);

    /**
     * EndTimer
     * Calculates seconds passed since last call to
     * BeginTimer
     */
    void EndTimer(void);
    
    /** 
     * GetElapsedSeconds
     * @return time in seconds between the last call to
     * BeginTimer and EndTimer
     */
    inline float GetElapsedSeconds(void) const { return m_fElapsedSeconds; }

private:
    // timer values
    INT64            m_ddwStartTicks;
    INT64            m_ddwTickFrequency;

    float            m_fElapsedSeconds;
};


#endif    // __TIMER_H__

