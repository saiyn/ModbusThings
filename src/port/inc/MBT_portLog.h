#ifndef __MBT_PORT_LOG_H__
#define __MBT_PORT_LOG_H__



#ifdef __cplusplus
extern "C"{
#endif


#include "MBT_log.h"


#define MBT_PORT_LOG_DEBUG(m, ...) do{                                      \
    if(portDebugFlag & (DEBUG_DEBUG)){                                      \
        MBT_printLog(m" DEBUG: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                       \
}while(0)


#define MBT_PORT_LOG_INFO(m, ...) do{                                      \
    if(portDebugFlag & (DEBUG_INFO)){                                      \
        MBT_printLog(m" INFO: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                      \
}while(0)


#define MBT_PORT_LOG_ERROR(m, ...) do{                                      \
    if(portDebugFlag & (DEBUG_ERROR)){                                      \
        MBT_printLog(m" ERROR: ", coreDebugFlag, __VA_ARGS__);              \
    }                                                                       \
}while(0)





#ifdef __cplusplus
}
#endif






#endif
