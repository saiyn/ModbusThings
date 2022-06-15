#ifndef _MODBUS_NETWORK_H_
#define _MODBUS_NETWORK_H_

#define MDB_NETWORK_OFFLINE 0
#define MDB_NETWORK_ONLINE 1



typedef struct mdb_network{
	
	
	int (*get_net_state)(void);
	
	int (*wait_for_ready)(int timeout);
	
	
}mdb_network_t;


mdb_network_t* mdb_network_service_init(void);








#endif

