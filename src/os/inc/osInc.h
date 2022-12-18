#ifndef _MBT_OS_INC_H_
#define _MBT_OS_INC_H_



#ifdef __cplusplus
extern "C"{
#endif


#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <math.h>

#include <stdarg.h>
#include <stdbool.h>




//#if defined(_TD_LINUX_64) || defined(_TD_LINUX_32) || defined(_TD_MIPS_64)  || defined(_TD_ARM_32) || defined(_TD_ARM_64)  || defined(_TD_DARWIN_64)

#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include <sys/syscall.h>
#include <sys/types.h>


#if defined(_TD_DARWIN_64)
    #include <dispatch/dispatch.h>
    #include "osEok.h"
#else
    // #include <argp.h>
    // #include <dlfcn.h>
    // #include <endian.h>
    // #include <linux/sysctl.h>
    // #include <poll.h>
    // #include <sys/epoll.h>
    // #include <sys/eventfd.h>
    // #include <sys/resource.h>
    // #include <sys/sendfile.h>
    #include <sys/prctl.h>

    #if !(defined(_ALPINE))
      #include <error.h>
    #endif
#endif








#ifdef __cplusplus
}
#endif






#endif
