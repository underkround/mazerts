#ifndef __TIMER_H__
#define __TIMER_H__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
 
class CTimer
{
public:
    CTimer(void);
    virtual ~CTimer(void);
 
    void Create(void);
    void Release(void);
    void BeginTimer(void);
    void EndTimer(void);
 
    /**
     * GetElapsedSeconds
     * @return time in seconds between the last call to
     * Begintimer and EndTimer
     */
    inline float GetElapsedSeconds(void) const {return  m_fElapsedSeconds; }
 
private:
    // timer values
    INT64            m_ddwStartTicks; //ddw = double double word
    INT64            m_ddwTickFrequency;
 
    float            m_fElapsedSeconds;
};


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
    //ask the performance timer frequency from the windows
    ::QueryPerformanceFrequency( (LARGE_INTEGER*)&m_ddwTickFrequency);
    m_fElapsedSeconds = 0.0f;
}
 
void CTimer::Release(void)
{
    //nothing to do really...
}
 
void CTimer::BeginTimer(void)
{
    // get the current ticks from the system
    ::QueryPerformanceCounter( (LARGE_INTEGER*)&m_ddwStartTicks);
}
 
void CTimer::EndTimer(void)
{
    if(m_ddwStartTicks == 0)
    {
        //maybe timer was not started!?
        m_fElapsedSeconds = 0.0f;
    }
    else
    {
        //compute elapsed seconds
        INT64 ddwEndTicks;
        float ftemp;
 
        do //sanity check loop if no ticks have passed we wait until they do
        {
            ::QueryPerformanceCounter( (LARGE_INTEGER*)&ddwEndTicks);
            ftemp = (float)(ddwEndTicks - m_ddwStartTicks);
        } while (ftemp < 0.0000001f);
 
        m_fElapsedSeconds = ftemp / (float)m_ddwTickFrequency;
    }
}

#endif __TIMER_H__