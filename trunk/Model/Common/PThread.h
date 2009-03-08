/**
 * PThread
 *
 * Helper class for POSIX-threads. Derive this class and implement the thread
 * mainloop by overriding run(). To start the running the thread, call
 *        PThread::start(this);
 * in your derived class, wherever it is appropriate
 *
 * Original source: http://en.wikipedia.org/wiki/POSIX_Threads
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#pragma comment(lib,"pthreadVC2.lib")
#include <pthread.h>

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/* IF YOU ARE HERE BECAUSE OF MISSING pthread.h -LIBRARY (Windows):
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Visual studio must be able to find the .h files in the include-directories 
and the .lib via library-directories:

Change the library and include-settings in your Visual studio to point in the directories
where .h -files and .lib can be found (trunk\Model\ExternalLibs): 

    -Select Tools -menu -> Options -> Projects and Solutions -> VC++ Directories
    -Select "Include files" from the "Show directories for" -dropdown-list
    -Click the "shiny folder" -button and add path to projects' ExternalLibs -directory
    -Select "Library files" from the "Show directories for" -dropdown-list
    -Click the "shiny folder" -button and add path to projects' ExternalLibs -directory    

To add the dependency (although the #pragma comment above _should_ take care of it),
select in Visual Studio:
    -Project -menu -> <ProjectName> Properties
    -Change Configuration to "All Configurations"
    -From the tree, select Configuration Properties->Linker->Input
    -In the "Additional Dependencies"-field, type "pthreadVC2.lib" (without quotes)

The .dll is located in trunk\Model
*/

class PThread
{
public:
    /**
     * Default constructor
     */
    PThread()
    {
    }

    /**
     *Destructor
     */
    virtual ~PThread()
    {
        pthread_exit(NULL);
    }

    /**
     * This method will be executed in its own thread when the
     * start-method is called
     */
    virtual void run() = 0;

    /**
     * Start the thread
     * @param pThreadInstance The instance of a class deriving from this class
     *        whose run()-method will act as the thread main
     * @return 0 if thread creation was successful, otherwise error code 
     *         (from pthread_create)     
     */
    int start(PThread* pThreadInstance)
    {        
        return pthread_create(&thread, NULL, PThread::thread_func, (void*)pThreadInstance);
    }

    /**
     * Waits for the thread to complete
     * @return 0 if successful, otherwise error code (from pthread_join)
     */
    int wait()
    {
        return pthread_join(thread, NULL);
    }

protected:
    /**
     * The thread
     */
    pthread_t thread;

    /**
     * Static thread-function called by pthread_create, directs the call
     * to correct objects' run-method
     * @param pThreadInstance PThread-derivative pointed through void-type pointer
     * @return A void* NULL, ignore this, the return is here just because the method 
     *          signature must match the pthread_create -calls parameter
     */
    static void *thread_func(void* pThreadInstance)
    {
        ((PThread *)pThreadInstance)->run();
        return NULL;
    }

};