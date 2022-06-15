#include <rtthread.h>

#include "MBT_osTimer.h"


int m_sleep(int t)
{
	return rt_thread_mdelay(t * 1000);
}


int m_usleep(int usec)
{
	uint32_t ms = 0;
	if(usec != 0) {
			ms = usec / 1000;
			if (ms == 0) {
					ms = 1;
			}
	}
		
  return rt_thread_mdelay(ms);
}



time_t m_time(time_t *tloc)
{
	
	return time(tloc);
}