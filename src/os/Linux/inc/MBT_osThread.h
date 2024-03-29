#ifndef _MBT_OS_THREAD_H_
#define _MBT_OS_THREAD_H_

#include "MBT_os.h"

typedef void* MBT_threadHandle;



MBT_threadHandle MBT_threadCreate(const char *name, void (*fn)(void *), void *arg, size_t stack_size, int priority);


int64_t MBT_getSelfPthreadId();


#endif

