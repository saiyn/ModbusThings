#ifndef _MODBUS_REALTIME_H_
#define _MODBUS_REALTIME_H_

#include <stddef.h>


#define REALTIME_ERR_QUEUE_FULL  (2)

#define REALTIME_OUT_CACHE_MAX   (50)

struct mdb_bluk;

typedef struct telemetry{
	char 			*msg;
	size_t 			size;
	int 			ts;
	unsigned short	ms;
	
	char *topic;
}telemetry_t;


typedef struct stat_ops{
	
	void (*onConnect)(void);
	void (*onConnected)(void);
	void (*onDisconnected)(void);
	
}stat_ops_t;


typedef struct msg_ops_node{
	char *topic;
	
	void (*onMessage)(char *msg);
}msg_ops_node_t;

typedef struct msg_ops{
	msg_ops_node_t nodes[REALTIME_SUBTOPIC_MAX_NUM];
	
	int n;
	
}msg_ops_t;


typedef struct mdr_service{
	
	char *telemetry_topic;
	
	
	void (*start)(void *userdata, stat_ops_t *, msg_ops_t *);
	
	
	int (*push_data)(void *userdata, telemetry_t *, char *topic);
	
	
	int (*dump_cached_data)(void *userdata, struct mdb_bluk *mdbb);
	
	
	void *userdata;
	

}mdr_service_t;



mdr_service_t * modbus_realtime_init(char *token, char *cid, char *uri);




#endif

