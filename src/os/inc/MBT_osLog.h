#ifndef __MBT_OS_LOG_H__
#define __MBT_OS_LOG_H__



#ifdef __cplusplus
extern "C"{
#endif


#include "MBT_log.h"


#define MBT_OS_LOG_DEBUG(...) do{                                      \
    if(coreDebugFlag & (DEBUG_DEBUG)){                                      \
        MBT_printLog("OS DEBUG: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                       \
}while(0)


#define MBT_OS_LOG_INFO(...) do{                                      \
    if(coreDebugFlag & (DEBUG_INFO)){                                      \
        MBT_printLog("OS INFO: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                      \
}while(0)


#define MBT_OS_LOG_ERROR(...) do{                                      \
    if(coreDebugFlag & (DEBUG_ERROR)){                                      \
        MBT_printLog("OS ERROR: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                       \
}while(0)





#ifdef __cplusplus
}
#endif






#endif
