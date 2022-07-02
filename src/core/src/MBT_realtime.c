#include <stdlib.h>

#include "MBT_config.h"

#include "MBT_portMqtt.h"

#include "MBT_osMemory.h"

#include "MBT_realtime.h"

#include "MBT_bulk.h"


static void do_start(void *userdata, stat_ops_t *stat_ops, msg_ops_t *msg_ops)
{
	mqtt_client_start(userdata, stat_ops, msg_ops);
}


static int push_data(void *userdata, telemetry_t* tt, char *topic)
{
	return mqtt_push_data(userdata, tt, topic);
	
}

static int dump_cached_data(void *userdata, mdb_bluk_t * mdbb)
{
	
	int rc = mqtt_dump_cached_data(userdata, mdbb->cache_buf, mdbb->cur_cached_off, BULK_FILE_MAX_BYTES - mdbb->cur_cached_off);
	
	if(rc > 0){
		
		mdbb->cur_cached_off += rc;
		
		//if the size exceed half of the max cache size after dump
		if(mdbb->cur_cached_off  > BULK_FILE_MAX_BYTES / 2){
			
			if(mdbb->save_bulk_data(mdbb) == 0){
				
				mdbb->cur_cached_off = 0;
				
				mdbb->next_index++;
				
			}
		
		}
		
		
	}
	

	return rc;
}


mdr_service_t * modbus_realtime_init(char *token, char *cid, char *uri)
{
		mdr_service_t * service = m_malloc(sizeof(mdr_service_t));
		if(!service){
			return NULL;
		}
	
		service->telemetry_topic = TELEMETRY_TOPIC;
		
		service->userdata = mqtt_client_init(token, cid, uri);
	
		service->start = do_start;
		service->push_data = push_data;
		service->dump_cached_data = dump_cached_data;
		
	
		return service;
}




