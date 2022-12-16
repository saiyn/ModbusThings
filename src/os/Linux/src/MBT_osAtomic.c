#include "MBT_os.h"



bool atomic_val_compare_exchange_8(int8_t volatile* ptr, int8_t oldval, int8_t newval)
{
    return  __atomic_compare_exchange_n(ptr, &oldval, newval, false,
									   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

}