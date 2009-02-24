#ifndef _SLEEP_H
#define _SLEEP_H

#ifdef _WIN32
#  include <windows.h>
# ifndef _NEED_SLEEP_ONLY
#  define msleep(t) Sleep(t)
# endif
#else
# include <unistd.h>
# ifndef _NEED_SLEEP_ONLY
#  define msleep(t) usleep((t) * 1000)
# endif
#endif

#endif /* _SLEEP_H */