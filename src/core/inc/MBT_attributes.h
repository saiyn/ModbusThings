#ifndef _MODBUS_ATTRIBUTES_H_
#define _MODBUS_ATTRIBUTES_H_


#define ATTRI_TOKEN_MAX_SIZE  (64)

#define ATTRI_CONFIG_MAX_SIZE	 (2048)


struct mdb_network;


typedef enum attri_key{
	ATTRI_DEV_TOKEN = 0,
	ATTRI_DEV_CONFIG = 1,
	ATTRI_DEV_CONFIG_TS,
	
	ATTRI_DEV_UUID,
	
	
	
	ATTRI_BULK_INDEX,
	ATTRI_BULK_FILE_TAIL_INDEX,
	ATTRI_BILK_FILE_TAIL_DATE,
	ATTRI_BULK_FORMAT_VERSION,
	
	
	
	
	ATTRI_SERVER_URI,
	ATTRI_BULK_SERVICE_URI,
	ATTRI_REALTIME_URI,
	
	ATTRI_KEY_NUM
	
}attri_key_e;



typedef struct attri_service{
	
	
	char *cache[ATTRI_KEY_NUM];
	

	
}attri_service_t;







int modbus_attributes_init(struct mdb_network *);



//should have cache internally
int mba_load_attribute(attri_key_e key ,char **attri, char*);


int mba_load_attribute_int(attri_key_e key ,int *attri);


int mba_save_attribute_int(attri_key_e key , int);

int mba_save_attribute(attri_key_e key ,char *attri);


int mba_attribute_check_expire(attri_key_e key);





#endif

