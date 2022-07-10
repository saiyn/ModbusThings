#ifndef _MBT_OS_NETWORK_H_
#define _MBT_OS_NETWORK_H_

#define MDB_NETWORK_UNKNOWN -1
#define MDB_NETWORK_OFFLINE 0
#define MDB_NETWORK_ONLINE 1




typedef struct mdb_network{
	
	int (*wait_for_ready)(int timeout);
	
	int (*get_net_state)(void);
	
	
	
	
}mdb_network_t;


mdb_network_t* mdb_network_service_init(void);








#endif

