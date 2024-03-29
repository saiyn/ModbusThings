
#include "MBT_os.h"


#include "MBT_osThread.h"

MBT_threadHandle MBT_threadCreate(const char *name, void (*fn)(void *), void *arg, size_t stack_size, int priority)
{
    (void)stack_size;
    (void)priority;
    (void)name;

    void *(*thread_entry)(void *);

    thread_entry = (void *(*)(void *))fn;

    pthread_t *pt = MBT_malloc(sizeof(pthread_t));
    if(!pt){
        return NULL;
    }

    int rc = pthread_create(pt, NULL, thread_entry, arg);
    if(rc){
        MBT_free(pt);
        return NULL;
    }

    return  (MBT_threadHandle)pt;
}



int64_t MBT_getSelfPthreadId() {
  static __thread int id = 0;
  if (id != 0) return id;
  id = syscall(SYS_gettid);
  return id;
}