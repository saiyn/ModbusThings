
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "MBT_config.h"

#include "MBT_attributes.h"

#include "MBT_bulk.h"

#include "MBT_realtime.h"

#include "MBT_scan.h"

#include "MBT_osNetwork.h"
#include "MBT_osThread.h"
#include "MBT_portHttpClient.h"
#include "MBT_osTimer.h"
#include "MBT_osMemory.h"
#include "MBT_portOTA.h"

#include "cJSON.h"



enum{
	CHECK_REASON_SCAN_FAIL = 0,
	CHECK_REASON_CACHE_FULL,
	CHECK_REASON_CONFIG_ERROR,
	
};


typedef struct mda_core{
	char *token;
	char *config;
	int		ts;
	
	int config_update;

	int ota_update;
	
	char *ota_title;
	char *ota_version;
	unsigned int ota_file_size;
	
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
	mba_load_attribute(ATTRI_DEV_TOKEN, &mdc->token, NULL);
	
	mba_load_attribute(ATTRI_DEV_CONFIG, &mdc->config, NULL);

	mdc->config_update = 1;
	
	return 0;
}


static int mdc_realtime_setup(mda_core_t *mdc)
{
	char *server_uri = NULL;
	char *uuid = NULL;
	
	mba_load_attribute(ATTRI_SERVER_URI, &server_uri, REALTIME_SERVER_DEFAULT);
	mba_load_attribute(ATTRI_DEV_UUID, &uuid, DEFAULT_DEV_UUID);
	
	mdr_service_t * mdrs = modbus_realtime_init(mdc->token, uuid, server_uri);
	
	mdc->mdrs = mdrs;
	
	return 0;
}

static int mdc_bulk_setup(mda_core_t *mdc)
{
	mdb_bluk_t *mdbb =  mdb_bulk_service_init(mdc->token);
	
	mdc->md_bulk_service = mdbb;
	
	return 0;
}


static int mdc_scan_setup(mda_core_t *mdc)
{
	
	mds_scan_t * mdss = mds_scan_service_init();
	
	mdc->md_scan_service = mdss;
	
	return 0;
}


static void check_post_status(mda_core_t *mdc, int reason, int rc)
{
	//old file upload
	if(reason == CHECK_REASON_SCAN_FAIL){
		//-2 means post fail due to no exist dat file but has dat.bk file, which means the tail index
		//update in trouble
		if(rc == 0 || rc == -2){
			mdc->md_bulk_service->update_tail_index(mdc->md_bulk_service);
		}else{
			//will one file always post fail????
		}
		
	}else{
		if(rc == 0){
			mdc->md_bulk_service->update_head_index(mdc->md_bulk_service);
		}
	}
	
}


static int check_do_bulk_service(mda_core_t *mdc, int reason)
{
	//if network is not aviable, we can't do anything
	if(mdc->md_net_service->get_net_state() != MDB_NETWORK_ONLINE){
		
		return -1;
	}
	
	int rc = 0;
	char *file_path = NULL;
	char *bulk_server_uri = DEFAULT_SERVER_FQDN;
	
	
	mba_load_attribute(ATTRI_BULK_SERVICE_URI, &bulk_server_uri, DEFAULT_SERVER_FQDN);
	
	switch(reason){
		//maybe the 485 bus is not connected, so we can check for upload bluk data cached in last working time
		//maybe the 485 bus error for a time, so we do a bluk check and upload to give the 485 bus time to resume
		case CHECK_REASON_SCAN_FAIL:
		case CHECK_REASON_CONFIG_ERROR:
		{
			//try to upload the very old cache data
			file_path = mdc->md_bulk_service->get_tail_cache_file_path(mdc->md_bulk_service);
			if(file_path){
				rc = mdc->md_bulk_service->post_bluk_data_by_file(mdc->md_bulk_service, bulk_server_uri, file_path);
				
				//judge whether we need to retry according to the return code
				check_post_status(mdc, CHECK_REASON_SCAN_FAIL, rc);
			}else{
				rc = -1;
			}
		
			break;
		}
		
		//maybe the 485 bus data read speed beyond the mqtt upload speed, so we do a bluk upload to speed up the data upload
		//maybe the mqtt service is down for a time, so we fallback to http bluk data only mode
		//if the network fully lost cause the full, then we can't do bluk update neither
		case CHECK_REASON_CACHE_FULL:
		{
			//try to upload the very latest cache data
			file_path = mdc->md_bulk_service->get_head_cache_file_path(mdc->md_bulk_service);
			if(file_path){
				rc = mdc->md_bulk_service->post_bluk_data_by_file(mdc->md_bulk_service, bulk_server_uri, file_path);	
				
				//update index info according return status
				check_post_status(mdc, CHECK_REASON_CACHE_FULL, rc);
				
			}else{
				rc = -1;
			}
		
			break;
		}
		
	}
		
	return -1;		
	
}

static void _do_ota(mda_core_t * mdc)
{
	
	char uri[128] = {0};
	
	snprintf(uri,sizeof(uri),"%s/%s/%s/%s?title=%s&version=%s", DEFAULT_SERVER_FQDN,DEV_OTA_API_PREFIX,mdc->token,
		DEV_OTA_API_SUFFIX, mdc->ota_title, mdc->ota_version);
	
	
	//LOG_I("try ota uri:%s", uri);
	
	int ret = httpclient_get_file(uri, DEV_OTA_FILE_PATH);
		
	if(ret == 0)
	{
		//sync to flash
		//LOG_I("download upgrade file:%s successfully", DEV_OTA_FILE_PATH);
		
		//ret = OTA_action();
	}
	
	
}



static void mdc_loop(void *arg)
{
	
	mda_core_t *mdc = (mda_core_t *)arg;
	
	int rc = 0;
	
	for(;;){
		
	
		if(mdc->config_update){
				mdc->config_update = 0;
				
				mds_parse_scan(mdc->config, (void *)0, mdc->md_scan_service);
				if(rc < 0){
					//LOG_E("parse scan config fail, try to do bulk service");
					
					if(check_do_bulk_service(mdc, CHECK_REASON_CONFIG_ERROR) < 0){
						m_sleep(3);
					}
					
				
				}
		}

		if(mdc->ota_update){
			
			mdc->ota_update = 0;
			
			_do_ota(mdc);
			
		}
		
		
		char *result;
		
		rc = mds_scan(mdc->md_scan_service);
		 
		if(rc < 0){
			
			if(check_do_bulk_service(mdc, CHECK_REASON_SCAN_FAIL)  < 0){
				m_sleep(1);
			}
			
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
		
		if(rc == -REALTIME_ERR_QUEUE_FULL){
			
			mdc->mdrs->dump_cached_data(mdc->mdrs->userdata, mdc->md_bulk_service);
			
			//try again
			rc = mdc->mdrs->push_data(mdc->mdrs->userdata ,tt, mdc->mdrs->telemetry_topic);
			if(rc < 0){
				m_free(result);
			}

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
				//LOG_E("prase json error: %s", error_ptr);
			}
			return;
		}
		
		//mb_config
		cJSON *mb = cJSON_GetObjectItem(root, "mb_config");
		if(mb){
			
			_mc.config_update = 1;
			
			
			mba_save_attribute(ATTRI_DEV_CONFIG, mb->valuestring);
			
			
			//LOG_I("recv attribute mb_config update");
			
		}else{
			
			//LOG_W("unknown msg:%s", cJSON_PrintUnformatted(root));
		}
	

		cJSON_Delete(root);
	
}


static void realtime_ota_msg_received(char *msg)
{
	
	//LOG_I("OTA:%s", msg);


	cJSON *root = cJSON_Parse(msg);
	if(root == NULL)
	{
		const char *error_ptr = cJSON_GetErrorPtr();
		if(error_ptr != NULL)
		{
			//LOG_E("prase json error: %s", error_ptr);
		}
		return;
	}


	cJSON *fw_title = cJSON_GetObjectItem(root, "fw_title");
	if(fw_title){
		
		//LOG_I("fw title update to:%s", fw_title->valuestring);
		_mc.ota_title = m_strdup(fw_title->valuestring);
	}

	cJSON *fw_size = cJSON_GetObjectItem(root, "fw_size");
	if(fw_size){
		
		//LOG_I("fw firmware size :%d", fw_size->valueint);
		_mc.ota_file_size = fw_size->valueint;
	}
	
	cJSON *fw_ver = cJSON_GetObjectItem(root, "fw_version");
	if(fw_ver){
		//LOG_I("fw version update to:%s", fw_ver->valuestring);
		_mc.ota_version = m_strdup(fw_ver->valuestring);
		
		_mc.ota_update = 1;
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
		.nodes = {
			{
				REALTIME_ATTR_SUBTOPIC,
				realtime_msg_received
			},
			{
				REALTIME_OTA_SUBTOPIC,
				realtime_ota_msg_received
			}
		},
		.n = 2
	};
	
	
	mdc->mdrs->start(mdc->mdrs->userdata, &stat_ops, &msg_ops);
	
	
	MBT_threadCreate("mb-core", mdc_loop, mdc, MODBUS_CORE_THREAD_STACK_SIZE, MODBUS_CORE_THREAD_PRIORITY);
	
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

