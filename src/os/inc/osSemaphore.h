#ifndef _OS_SEMAPHORE_H_
#define _OS_SEMAPHORE_H_




#ifdef __cplusplus
extern "C"{
#endif


#ifdef _MBT_RTT




#else

    #define msem_t sem_t
    #define msem_init sem_init
    int tsem_wait(msem_t* sem);
    #define msem_post sem_post
    #define msem_destroy sem_destroy


#endif




#ifdef __cplusplus
}
#endif





#endif