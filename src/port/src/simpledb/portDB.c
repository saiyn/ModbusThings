#include "MBT_portDB.h"

#include "MBT_simpleDb.h"

MBT_DB_HANDLE *_db;

int service_db_init(int key_num_max)
{
    return MBT_simpleDbInit(&_db, key_num_max);

}

char *service_db_load(int key)
{
    return MBT_simpleDbGet(_db, key);
}

int service_db_save(int key, char *value)
{
    return MBT_simpleDbPut(_db, key, value);
}

int service_db_load_int(int key, int *value)
{
    char *value = NULL;

    if((value = MBT_simpleDbGet(_db, key))){
        *value = atoi(value);
        return 0;
    }

    return -1;
}

int service_db_save_int(int key, int value)
{
    char tmp[32] = {0};

    sprintf(tmp, "%d", value);

    return MBT_simpleDbPut(_db, key, tmp);
}