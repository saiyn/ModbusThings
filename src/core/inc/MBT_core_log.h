#ifndef __MBT_CORE_LOG_H__
#define __MBT_CORE_LOG_H__



#ifdef __cplusplus
extern "C"{
#endif


#include "MBT_log.h"


#define MBT_CORE_LOG_INFO(...) do{                                         \
    if(coreDebugFlag & (DEBUG_INFO)){                                      \
        MBT_printLog("CORE INFO: ", coreDebugFlag, __VA_ARGS__);           \
    }                                                                      \
}while(0)


#define MBT_CORE_LOG_ERROR(...) do{                                         \
    if(coreDebugFlag & (DEBUG_ERROR)){                                      \
        MBT_printLog("CORE ERROR: ", coreDebugFlag, __VA_ARGS__);           \
    }                                                                       \
}while(0)





#ifdef __cplusplus
}
#endif






#endif
