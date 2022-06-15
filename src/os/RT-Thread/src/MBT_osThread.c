

#include <rtthread.h>

#include "MBT_osThread.h"

MBT_threadHandle MBT_threadCreate(const char *name, void (*fn)(void *), void *arg, size_t stack_size, int priority)
{
	rt_thread_t tid = rt_thread_create(name, fn, arg, stack_size, priority, 20);
	
	if(tid != RT_NULL){
		
		rt_thread_startup(tid);
	}
	
	
	return tid;
}