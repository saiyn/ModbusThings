#include "MBT_portDB.h"

#include "modbus_system.h"

#include <easyflash.h>


int service_db_init(void)
{
	return easyflash_init();
}


static char *convert_str_key(int key)
{
	static char *key_map[ATTRI_KEY_NUM] = {0};
	
	if(key_map[key]){
		
		return key_map[key];
	}
	
	char tmp[32] = {0};
	
	snprintf(tmp, sizeof(tmp), "%s_%d", ATTRIBUTE_KEY_PREFIX, key);
	
	key_map[key] = m_strdup(tmp);
	
	
	return key_map[key];
}

char *service_db_load(int key)
{
	
	char *rc = ef_get_env(convert_str_key(key));
	if(!rc)
		return NULL;
	
	
	return m_strdup(rc);
}




int service_db_save(int key, char *value)
{
	return ef_set_env(convert_str_key(key), value);	
}

int service_db_save_int(int key, int value)
{
	int ret;
	
	ret = ef_set_env_blob(convert_str_key(key), (unsigned char *)&value, sizeof(value));
		
	if(EF_NO_ERR != ret)
	{
		return -1;
	}
	
	return 0;
}


int service_db_load_int(int key, int *value)
{
	size_t rlen, alen;
	
	
	rlen = ef_get_env_blob(convert_str_key(key), (unsigned char *)value, sizeof(*value), &alen);
	if(rlen == 0)
	{	
		return -1;
	}
	
	if(rlen != alen)
	{
		//LOG_W("read len:%d is not equal to blob len:%d and actual len is:%d", rlen, alen, sizeof(cid));
	}
	
	return 0;
	
}



