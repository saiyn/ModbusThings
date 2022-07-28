#ifndef _MBT_SIMPLE_DB_H_
#define _MBT_SIMPLE_DB_H_


typedef void*   MBT_DB_HANDLE;
typedef int     MBT_DB_KEY;
typedef char*   MBT_DB_VALUE;




int MBT_simpleDbInit(MBT_DB_HANDLE *handle, int key_num_max);


int MBT_simpleDbPut(MBT_DB_HANDLE h, MBT_DB_KEY key, MBT_DB_VALUE value);


MBT_DB_VALUE MBT_simpleDbGet(MBT_DB_HANDLE h, MBT_DB_KEY key);




#endif
