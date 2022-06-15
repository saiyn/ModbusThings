
#include <stdlib.h>
#include <time.h>


#include "modbus_config.h"

#include "modbus_attributes.h"

#include "modbus_bulk.h"

#include "modbus_realtime.h"

#include "modbus_scan.h"

#include "modbus_network.h"

#include "modbus_system.h"

#include "cJSON.h"


#define DBG_TAG              "mdc"
#define DBG_LVL              DBG_LOG
#include <rtdbg.h>



enum{
	CHECK_REASON_SCAN_FAIL = 0,
	CHECK_REASON_CACHE_FULL,
	
};


typedef struct mda_core{
	char *token;
	char *config;
	int		ts;
	
	int config_update;
	
	mdr_service_t *mdrs;
	
	mds_scan_t * md_scan_service;
	
	mdb_bluk_t * md_bulk_service;
	
	mdb_network_t * md_net_service;
	
}mda_core_t;

static mda_core_t _mc;



static int mdc_network_setup(mda_core_t *mc)
{
	
	mc->md_net_service = mdb_network_service_init();
	
	return 0;
}

static int mdc_attribute_update(mda_core_t *mdc)
{
	//the attribute service init will do very complex work
	//in some situation we may stuck here
	modbus_attributes_init(mdc->md_net_service);
	
	//this load should get result from cache
	mba_load_attribute(ATTRI_DEV_TOKEN, &mdc->token);
	
	mba_load_attribute(ATTRI_DEV_CONFIG, &mdc->config);

	mdc->config_update = 1;
	
	return 0;
}


static int mdc_realtime_setup(mda_core_t *mdc)
{
	char *server_uri = NULL;
	char *uuid = NULL;
	
	mba_load_attribute(ATTRI_SERVER_URI, &server_uri);
	mba_load_attribute(ATTRI_DEV_UUID, &uuid);
	
	mdr_service_t * mdrs = modbus_realtime_init(mdc->token, uuid, server_uri);
	
	mdc->mdrs = mdrs;
	
	return 0;
}

static int mdc_bulk_setup(mda_core_t *mdc)
{
	mdb_bluk_t *mdbb =  mdb_bulk_service_init();
	
	mdc->md_bulk_service = mdbb;
	
	return 0;
}


static int mdc_scan_setup(mda_core_t *mdc)
{
	
	mds_scan_t * mdss = mds_scan_service_init();
	
	mdc->md_scan_service = mdss;
	
	return 0;
}


static void check_do_bulk_service(mda_core_t *mdc, int reason)
{
	//if network is not aviable, we can't do anything
	if(mdc->md_net_service->get_net_state() != MDB_NETWORK_ONLINE){
		
		return;
	}
	
	char *file_path = NULL;
	char *bulk_server_uri = DEFAULT_SERVER_FQDN"/"DEFAULT_BULK_SERVICE_API;
	
	
	mba_load_attribute(ATTRI_BULK_SERVICE_URI, &bulk_server_uri);
	
	switch(reason){
		//maybe the 485 bus is not connected, so we can check for upload bluk data cached in last working time
		//maybe the 485 bus error for a time, so we do a bluk check and upload to give the 485 bus time to resume
		case CHECK_REASON_SCAN_FAIL:
		{
			//try to upload the very first cache data
			file_path = mdc->md_bulk_service->get_head_cache_file_path(mdc->md_bulk_service);
			if(file_path)
				mdc->md_bulk_service->post_bluk_data_by_file(mdc->md_bulk_service, bulk_server_uri, file_path);
		
			break;
		}
		
		//maybe the 485 bus data read speed beyond the mqtt upload speed, so we do a bluk upload to speed up the data upload
		//maybe the mqtt service is down for a time, so we fallback to http bluk data only mode
		//if the network fully lost cause the full, then we can't do bluk update neither
		case CHECK_REASON_CACHE_FULL:
		{
			//try to upload the very last cache data
			file_path = mdc->md_bulk_service->get_tail_cache_file_path(mdc->md_bulk_service);
			if(file_path)
				mdc->md_bulk_service->post_bluk_data_by_file(mdc->md_bulk_service, bulk_server_uri, file_path);	
		
			break;
		}
		
	}
		
		
	
}



static void mdc_loop(void *arg)
{
	
	mda_core_t *mdc = (mda_core_t *)arg;
	
	int rc = 0;
	
	for(;;){
		
	
		if(mdc->config_update){
			
				mds_parse_scan(mdc->config, (void *)0, mdc->md_scan_service);
			
				mdc->config_update = 0;
		}
		
		
		char *result;
		
		rc = mds_scan(mdc->md_scan_service);
		 
		if(rc < 0){
			
			check_do_bulk_service(mdc, CHECK_REASON_SCAN_FAIL);
			
			continue;
		}
		
		
		rc = mds_merge_result(mdc->md_scan_service, &result);
		if(rc < 0){
			continue;
		}
		
		
		telemetry_t *tt = m_malloc(sizeof(telemetry_t));
		
		//result is the json string contains ts info
		tt->msg = result;
		tt->size = strlen(tt->msg);
		tt->ts = m_time(NULL);
		tt->topic = TELEMETRY_TOPIC;
		
		rc = mdc->mdrs->push_data(mdc->mdrs->userdata ,tt, mdc->mdrs->telemetry_topic);
		
		if(rc == REALTIME_ERR_QUEUE_FULL){
			
			mdc->mdrs->dump_cached_data(mdc->mdrs->userdata, mdc->md_bulk_service);
			
			
			check_do_bulk_service(mdc, CHECK_REASON_CACHE_FULL);
		}
		
	
	}
	
}


static void realtime_service_connect(void)
{
    
}

static void realtime_service_connected(void)
{

}

static void realtime_service_disconnected(void)
{

}


//all attributes update throuth this
static void realtime_msg_received(char *msg)
{
		cJSON *root = cJSON_Parse(msg);
		if(root == NULL)
		{
			const char *error_ptr = cJSON_GetErrorPtr();
			if(error_ptr != NULL)
			{
				LOG_E("prase json error: %s", error_ptr);
			}
			return;
		}
		
		//mb_config
		cJSON *mb = cJSON_GetObjectItem(root, "mb_config");
		if(mb){
			
			_mc->config_update = 1;
			
			
			mba_save_attribute(ATTRI_DEV_CONFIG, mb->valuestring);
			
			
			LOG_I("recv attribute mb_config update");
			
			return;
		}
	
}


static void mdc_start(mda_core_t *mdc)
{
	stat_ops_t stat_ops = {
		realtime_service_connect,
		realtime_service_connected,
		realtime_service_disconnected
	};
	
	msg_ops_t msg_ops = {
		REALTIME_SUBTOPIC,
		realtime_msg_received
	};
	
	
	mdc->mdrs->start(mdc->mdrs->userdata, &stat_ops, &msg_ops);
	
	
	m_create_thread("mb-core", mdc_loop, mdc, MODBUS_CORE_THREAD_STACK_SIZE, MODBUS_CORE_THREAD_PRIORITY);
	
}

int modbus_core_start(void){
	
	mdc_network_setup(&_mc);

	mdc_attribute_update(&_mc);
	
	mdc_realtime_setup(&_mc);
	
	mdc_bulk_setup(&_mc);
	
	mdc_scan_setup(&_mc);
	
	
	mdc_start(&_mc);
	
	return 0;
}

