#ifndef __OS_ATOMIC_H__
#define __OS_ATOMIC_H__




#ifdef __cplusplus
extern "C"{
#endif


#ifdef _MBT_RTT

    

#else

    bool atomic_val_compare_exchange_8(int8_t volatile* ptr, int8_t oldval, int8_t newval);

    void atomic_add_fetch_32(int32_t volatile* ptr, int32_t value);


#endif




#ifdef __cplusplus
}
#endif





#endif