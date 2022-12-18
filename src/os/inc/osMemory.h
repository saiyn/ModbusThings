#ifndef _OS_MEMORY_H_
#define _OS_MEMORY_H_




#ifdef __cplusplus
extern "C"{
#endif


#ifdef _MBT_RTT

    #define MBT_malloc(size) rt_malloc(size)
    #define MBT_calloc(num, size) rt_calloc(num, size)
    #define MBT_realloc(ptr, size) rt_realloc(ptr, size)
    #define MBT_free(x)         \
    do {                   \
        if (x) {             \
        rt_free((void *)(x)); \
        x = 0;             \
        }                    \
    } while (0)


#else

    #define MBT_malloc(size) malloc(size)
    #define MBT_calloc(num, size) calloc(num, size)
    #define MBT_realloc(ptr, size) realloc(ptr, size)
    #define MBT_free(x)         \
    do {                   \
        if (x) {             \
        free((void *)(x)); \
        x = 0;             \
        }                    \
    } while (0)


#endif




#ifdef __cplusplus
}
#endif





#endif