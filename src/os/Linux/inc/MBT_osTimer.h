#ifndef _MBT_OS_TIMER_H_
#define _MBT_OS_TIMER_H_

#include <sys/time.h>

int m_sleep(int);

int m_usleep(int);


time_t m_time(time_t *tloc);





#endif
