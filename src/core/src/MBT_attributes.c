
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MBT_config.h"
#include "MBT_portHttpClient.h"


#include "MBT_attributes.h"

#include "modbus_network.h"






struct attri_service _as;


//although we can recive attributes update event by realtime service, but we still need to 
//pasitivly to check the attirbute during system initing
static int check_config_update(char *token ,char *config, char *server_uri)
{
	char *new_config = NULL;
	
	int rc = service_attri_update(DEV_ATTRI_CONFIG_KEY, token , server_uri,&new_config);
	if(rc < 0){
		return rc;
	}
	
	if(strcmp(new_config, config)){
		
		mba_save_attribute(ATTRI_DEV_CONFIG, new_config);
		
		time_t now = m_time(NULL);
		
		mba_save_attribute_int(ATTRI_DEV_CONFIG_TS, now);
	}
	
	
	return rc;
	
}


static int do_dev_provisioning(struct mdb_network *mdbn, char **token, char *server_uri)
{
	//we have to wait util the network get ready 
	while(mdbn->get_net_state() != MDB_NETWORK_ONLINE){
		
		m_sleep(1);
		
	}
	
	char *uuid = DEFAULT_DEV_UUID;
	
	//need to log out error to warn caller set uuid first
	int rc = mba_load_attribute(ATTRI_DEV_UUID, &uuid, DEFAULT_DEV_UUID);
	//if(rc < 0)
		//return rc;
	
	
	return service_dev_provision(token, server_uri, uuid);
}


int modbus_attributes_init(struct mdb_network *mdbn)
{
	char *token = NULL;
	char *config = NULL;
	int timeout = -1;
	
	
	service_db_init(ATTRI_KEY_NUM);
	
	
	int rc =  mba_load_attribute(ATTRI_DEV_TOKEN, &token, NULL);
	if(rc == 0){			
		
			rc = mba_load_attribute(ATTRI_DEV_CONFIG, &config, NULL);
			if(rc == 0){
				
				rc = mba_attribute_check_expire(ATTRI_DEV_CONFIG_TS);
				if(rc){
					//config too long no update, we can wait for sometime to try update it
					//but we won't wait for too long
					timeout = 5000;
				}else{
					//token and config both are in good state, we can continue without need to wait for network get ready
					timeout = -1;
				}
				
			}else{
				//we only has token, so we have to wait forever to fetch config
				timeout = 0;
			}		
	}else{
		//we will wait forever in this if the network is not ready,
		//because, without token means we have no config, without config, we can't do anything
		char *server_uri = DEFAULT_SERVER_FQDN;
		
		rc = mba_load_attribute(ATTRI_SERVER_URI, &server_uri, DEFAULT_SERVER_FQDN);

		
		rc = do_dev_provisioning(mdbn, &token, server_uri);
		if(rc < 0){
			return -1;
		}
		
		mba_save_attribute(ATTRI_DEV_TOKEN, token);
	}
		
	if(mdbn->get_net_state() == MDB_NETWORK_OFFLINE && timeout >= 0){
		
		//timeout=0 means forever
		mdbn->wait_for_ready(timeout);
		
	}
	
	
	//althouge timeout maybe -1 means token and config both are in good state,
	//but since the netowrk is ready, it's better for us got do a update check
	//so 1) we just has token 2) the config expire 3)both good
	if(mdbn->get_net_state() == MDB_NETWORK_ONLINE){
		
		char *server_uri = DEFAULT_SERVER_FQDN;
		
		rc = mba_load_attribute(ATTRI_SERVER_URI, &server_uri, DEFAULT_SERVER_FQDN);
		
		
		check_config_update(token ,config, server_uri);
		
	}
	
	
	return 0;
	
}




int mba_load_attribute(attri_key_e key ,char **attri, char *dft)
{
	int rc = 0;
	
	if(_as.cache[key]){
		
		*attri = _as.cache[key];
	}else{
		
		char *value;
		
		if((value = service_db_load(key)) != NULL){
			
			 _as.cache[key] = value;
			
			*attri = value;
		}else{

			if(dft){
				_as.cache[key] = dft;
				*attri = dft;
			}else{	
				rc = -1;
			}
		}	
		
	}
	
	return rc;
}


int mba_load_attribute_int(attri_key_e key ,int *attri)
{
	
	return service_db_load_int(key, attri);
	
}


int mba_save_attribute_int(attri_key_e key , int value)
{
	
	return service_db_save_int(key, value);
}


int mba_save_attribute(attri_key_e key ,char *attri)
{
	if(_as.cache[key] && !strcmp(_as.cache[key], attri)){
		
		return 0;
	}
	
	if(_as.cache[key]){
		
		free(_as.cache[key]);
	}
	
	_as.cache[key] = m_strdup(attri);
	
	return service_db_save(key, attri);
	
}


int mba_attribute_check_expire(attri_key_e key)
{
	int last_update_ts = 0;
	
	
	int rc = mba_load_attribute_int(key, &last_update_ts);
	if(rc == 0){
		
		time_t now = m_time(NULL);
		
		if(now > last_update_ts){
			return (now - last_update_ts) > DEV_ATTRI_CONFIG_EXPIRE_SECOND;
		}
		
	}
	
	//any exception, we hope to see it expired
	return 1;
}







