#ifndef _MBT_PORT_SERVER_H_
#define _MBT_PORT_SERVER_H_



int service_dev_provision(char **, char *, char*);

int service_attri_update(char *, char*, char*,char**);

int service_bulk_post(const char* server_uri, const char* filename);





#endif
