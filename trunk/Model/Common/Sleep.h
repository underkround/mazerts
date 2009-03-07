/**
 * Cross-platform sleep
 *
 * $Revision$
 * $Date$
 * $Id$
 */

#ifndef _SLEEP_H
#define _SLEEP_H

#ifdef _WIN32
    #include <windows.h>    
    #define msleep(t) Sleep(t)
#else
    #include <unistd.h>
    #define msleep(t) usleep((t) * 1000)
#endif

#endif /* _SLEEP_H */