/**
 * PathFinderMaster
 *
 * Master-thread handling the updating of PathFinders
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef __PATHFINDERMASTER_H__
#define __PATHFINDERMASTER_H__

#include "../Common/PThread.h"

class PathFinderMaster : public PThread
{
public:

    /**
     * Returns the singleton instance
     * @return Pointer to PathFinderMaster-instance
     */
    inline static PathFinderMaster* getInstance()
    {
        return pInstance;
    }

    /**
     * Destructor
     */
    virtual ~PathFinderMaster()
    {
    }

    /**
     * Thread entry-point
     */
    void run();

    /**
     * Starts the thread
     */
    void start()
    { 
        if(!m_Running)
        {
            m_Running = true;
            PThread::start(pInstance);            
        }        
    }

private:

    /**
     * Static reference to the singleton instance
     */
    static PathFinderMaster* pInstance;

    /**
     * Default constructor
     */
    PathFinderMaster() 
    {
        m_Running = false;
    };

    /**
     * Tells if the thread is running or not
     */
    bool m_Running;
};


#endif // __PATHFINDERMASTER_H__