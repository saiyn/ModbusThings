
#include <stdio.h>
#include <string.h>

#include "cJSON.h"

#include "MBT_portServer.h"

#include "MBT_config.h"
#include "MBT_osMemory.h"
#include "MBT_portHttpClient.h"



#define GENE_PROVISION_REQUEST(data, off, name, key, sec) do{				\
	off += sprintf(data, "{\"deviceName\": \"%s\",", name);						\
	off += sprintf(data + off, "\"provisionDeviceKey\": \"%s\",", key);			\
	off += sprintf(data + off, "\"provisionDeviceSecret\": \"%s\"}", sec);	\
}while(0)
	

#define TB_PROVISION_DEVICE_KEY  "nvjmil2xbub4w3jui3mj"
#define TB_PROVISION_DEVICE_SEC	 "zs78vd565a6jjzzgt2y4"


static int parse_token(char *rsp, char **token)
{
	cJSON *root = cJSON_Parse(rsp);
	if(root){
		cJSON *status = cJSON_GetObjectItem(root ,"status");
		if(status){
			if(strcmp(status->valuestring, "SUCCESS") ==0){
				
				cJSON *toke = cJSON_GetObjectItem(root ,"credentialsValue");
				if(toke){
					//strncpy(*token, toke->valuestring, size);
					*token = m_strdup(toke->valuestring);
					
					return 0;
				}
				
			}
			
		}
		
	}
	
	
	return -1;
}



int service_dev_provision(char **token, char *server_uri, char *devid)
{
	int index = 0;
	char *request = NULL, *header = NULL;
	char req_uri[128] = {0};
	char devName[64] = {0};
	char post_data[256] = {0};
	
	

	int rc = snprintf(req_uri, sizeof(req_uri), "%s%s", server_uri, DEV_PROVISION_API);
	if(rc >= sizeof(req_uri)){
		//we just throw error
		return -1;
	}
	
	rc = snprintf(devName, sizeof(devName), "%s_%s", DEV_PROVISION_NAME_PREFIX, devid);
	if(rc >= sizeof(devName)){
		return -1;
	}
	
	
	GENE_PROVISION_REQUEST(post_data, index, devName,TB_PROVISION_DEVICE_KEY, TB_PROVISION_DEVICE_SEC);
	
	
	httpclient_request_header_add((void **)&header, "Content-Length: %d\r\n", strlen(post_data));
    httpclient_request_header_add((void **)&header, "Content-Type: application/json\r\n");
	
	if (httpclient_request(req_uri, (const char *)header, post_data, (unsigned char **)&request) < 0)
	{
			httpclient_free(header);
			return -1;
	}
	
	
	return parse_token(request, token);

}


int service_attri_update(char *key, char *token, char* server_uri, char**config)
{
	
	char uri[256] = {0};
	
	
	int rc = snprintf(uri, sizeof(uri), "%s/api/v1/%s/attributes?sharedKeys=%s", server_uri, token, key);
	if(rc >= sizeof(uri)){
		
		//LOG_E("need redefine uri size");
		return -1;
	}
	
	
	rc = httpclient_request(uri, NULL, NULL, (unsigned char **)config);
	
	return rc;
	
}


int service_bulk_post(const char* server_uri, const char* filename)
{
	
	return httpclient_post_file(server_uri, filename, BULK_SERVICE_FORM_STR);
	
}











