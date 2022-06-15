#ifndef _MBT_PORT_DB_H_
#define _MBT_PORT_DB_H_



int service_db_init(int);

char *service_db_load(int key);

int service_db_save(int key, char *value);

int service_db_load_int(int key, int *);

int service_db_save_int(int key, int);







#endif
