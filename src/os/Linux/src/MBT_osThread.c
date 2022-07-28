
#include <pthread.h>

#include "MBT_osMemory.h"
#include "MBT_osThread.h"

MBT_threadHandle MBT_threadCreate(const char *name, void (*fn)(void *), void *arg, size_t stack_size, int priority)
{
    (void)stack_size;
    (void)priority;
    (void)name;

    void *(*thread_entry)(void *);

    thread_entry = (void *(*)(void *))fn;

    pthread_t *pt = m_malloc(sizeof(pthread_t));
    if(!pt){
        return NULL;
    }

    int rc = pthread_create(pt, NULL, thread_entry, arg);
    if(rc){
        m_free(pt);
        return NULL;
    }

    return  (MBT_threadHandle)pt;
}