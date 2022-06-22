#include <unistd.h>
#include <sys/time.h>

#include "MBT_Timer.h"



int m_sleep(int t)
{
	return sleep(t);
}


int m_usleep(int usec)
{
	return usleep(usec);
}



time_t m_time(time_t *tloc)
{
	
	return time(tloc);
}