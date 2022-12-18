#ifndef _MBT_LOG_H_
#define _MBT_LOG_H_



#ifdef __cplusplus
extern "C"{
#endif


#define DEBUG_FATAL 1U
#define DEBUG_ERROR DEBUG_FATAL
#define DEBUG_WARN  2U
#define DEBUG_INFO  DEBUG_WARN
#define DEBUG_DEBUG 4U
#define DEBUG_TRACE 8U
#define DEBUG_DUMP  16U

#define DEBUG_SCREEN 64U
#define DEBUG_FILE   128U


extern int32_t uDebugFlag;
extern int32_t coreDebugFlag;


#define MBT_LOG_INFO(...) do{                                              \
    if(uDebugFlag & (DEBUG_INFO)){                                         \
        MBT_printLog("UTL INFO: ", uDebugFlag, __VA_ARGS__);                 \
    }                                                                      \
}while(0)







int32_t MBT_initLog(char *logName, int32_t numOfLogLines, int32_t maxFiles);
void    MBT_closeLog();
void    MBT_resetLog();

void    MBT_printLog(const char *flags, int32_t dflag, const char *format, ...)
#ifdef __GNUC__
 __attribute__((format(printf, 3, 4)))
#endif
;

void    MBT_printLongString(const char * flags, int32_t dflag, const char *format, ...)
#ifdef __GNUC__
 __attribute__((format(printf, 3, 4)))
#endif
;



#ifdef __cplusplus
}
#endif

#endif



