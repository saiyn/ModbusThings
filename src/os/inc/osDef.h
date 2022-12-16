#ifndef _OS_DEF_H_
#define _OS_DEF_H_


#ifdef __cplusplus
extern "C" {
#endif


#if defined(_TD_LINUX_64) || defined(_TD_LINUX_32) || defined(_TD_MIPS_64)  || defined(_TD_ARM_32) || defined(_TD_ARM_64)  || defined(_TD_DARWIN_64)
  #if defined(_TD_DARWIN_64)
    // MacOS
    #if !defined(_GNU_SOURCE)
      #define setThreadName(name) do { pthread_setname_np((name)); } while (0)
    #else
      // pthread_setname_np not defined
      #define setThreadName(name)
    #endif
  #else
    // Linux, length of name must <= 16 (the last '\0' included)
    #define setThreadName(name) do { prctl(PR_SET_NAME, (name)); } while (0)
  #endif
#else
  // Windows
  #define setThreadName(name)
#endif










#ifdef __cplusplus
}
#endif



#endif