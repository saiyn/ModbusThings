#include <stdio.h>
#include "cJSON.h"
#include "MBT_scan.h"
#include <stdlib.h>
#include <string.h>
#include "MBT_portMaster.h"


#define MB_TYPE_COILS 				"coils"
#define MB_TYPE_DISCRET_INPUT "discret_input"
#define MB_TYPE_INPUT_REG			"input_reg"
#define MB_TYPE_HOLD_REG			"hold_reg"


typedef int (*config_parse)(cJSON *conf, mds_scan_t* re);


static int parse_input_reg_config(cJSON *conf, mds_scan_t* re);

static int parse_hold_reg_config(cJSON *conf, mds_scan_t* re);


struct parse_handler{
	char *type;
	
	config_parse parser;
};



static struct parse_handler mdcps[] = {
	{MB_TYPE_COILS, NULL},
	{MB_TYPE_DISCRET_INPUT, NULL},
	{MB_TYPE_INPUT_REG, parse_input_reg_config},
	{MB_TYPE_HOLD_REG, parse_hold_reg_config}
	
};


//{
//	"hold_reg": [],
//	"input_reg": [
//		{
//			"devAddres": 1,
//			"module": "Engine",
//			"start": 0,
//			"offset": 180
//		}
//	],
//	"proto_id": 5,
//	"proto_ver": 4
//}

static int parse_input_reg_config(cJSON *conf, mds_scan_t* re)
{	
	cJSON *item = NULL;

	
	cJSON_ArrayForEach(item, conf){
		cJSON *as = cJSON_GetObjectItem(item, "start");
		cJSON *os = cJSON_GetObjectItem(item, "offset");
		cJSON *ad = cJSON_GetObjectItem(item, "devAddres");
		cJSON *la = cJSON_GetObjectItem(item, "module");
		
		
		mds_item_t * node = calloc(sizeof(mds_item_t), 1);
		if(!node){
			return -1;
		}
		
		node->type = MB_TYPE_INPUT_REG_E;
		node->reg.input.module = m_strdup(la->valuestring);
		node->reg.input.start = as->valueint;
		node->reg.input.offset = os->valueint;
		node->reg.input.dev_addr = ad->valueint;
		
		
		//alloc mem for scan result
		
		node->size = (node->reg.input.offset - node->reg.input.start) * sizeof(unsigned short);
		node->value = (unsigned short *)malloc(node->size);
		
		snprintf(node->alias_name, sizeof(node->alias_name), "%d:%s", ad->valueint, la->valuestring);
		
		if(re->head == NULL){
			re->head = node;
		}else{
			re->head->next = node;
		}
		
	}
	
	
	return 0 ;
}


static int parse_hold_reg_config(cJSON *conf, mds_scan_t* re)
{
	
	return 0;
}

static void clear_all_scan_node(mds_scan_t *s)
{
	
	
	
}


int mds_parse_scan(char *json, convert c, mds_scan_t *result)
{
	
	if(c){
		json = c(json);
	}
	
	char *data_type[] = {MB_TYPE_COILS, MB_TYPE_DISCRET_INPUT, MB_TYPE_INPUT_REG, MB_TYPE_HOLD_REG};
	
	cJSON *mb_config = NULL;
	
	cJSON *root = cJSON_Parse(json);
	if(!root){
		return -1;
	}

	cJSON *shared = cJSON_GetObjectItem(root, "shared");
	if(!shared){
		mb_config = cJSON_GetObjectItem(root, "mb_config");
		if(!mb_config){
			cJSON_Delete(root);
			return -1;
		}
	}else{

		mb_config = cJSON_GetObjectItem(shared, "mb_config");
		if(!mb_config){
			cJSON_Delete(root);
			return -1;
		}
	}
	
	//here we should remove all exist scan node
	clear_all_scan_node(result);

	for(int i = 0; i < sizeof(data_type) / sizeof(data_type[0]); i++){
		
			cJSON *dt = cJSON_GetObjectItem(mb_config, data_type[i]);
			if(!dt)
				continue;
			
			for(int j = 0; j < 4; j++){
				if(strcmp(mdcps[j].type, data_type[i]) == 0){
					if(mdcps[j].parser){
						mdcps[j].parser(dt, result);
						break;
					}
				}
			}

	}

	cJSON_Delete(root);

	return 0;
}

int mds_scan(mds_scan_t *scan)
{
	int retval = 0;
	int anyOneOk = 0;
	
	for(mds_item_t *node = scan->head; node != NULL; node = node->next){
		
		switch(node->type){
			
			case MB_TYPE_INPUT_REG_E:
					retval = scan->ops.read_input_regs(scan->userdata, node->reg.input.dev_addr, node->size / 2, node->value);
			
					node->stat = retval < 0 ? MB_SCAN_FAIL : MB_SCAN_SUCCESS;

					LOG_I("[%s] scan %s", node->alias_name, node->stat == MB_SCAN_SUCCESS ? "success" : "fail");
				break;
			
			default:
				break;
			
		}

		if(node->stat == MB_SCAN_SUCCESS)
			anyOneOk = 1;
		
		m_usleep(500000);
	}
	
	return !anyOneOk;
}



//{
//  "type": "rawData",
//  "rawData": {
//    "input.engineL":  "00d800d8",
//    "hold.engineL": "00d800d8",
//		"hold.engineL.event": 88,
//    "input.engineR": "00dd00dd",
//    "hold.engineR": "00dd00dd",
//		"hold.engineR.sn": "xxxxx",
//    "hold.UltraFlow": "dd00dd00"
//  },
//  "ts": 6883838383
//}


//{
//	"values": {
//		"type": "rawData",
//		"rawData": {
//			"input.engineL": "00d800d8",
//			"hold.engineL": "00d800d8",
//			"hold.engineL.event": 88,
//			"input.engineR": "00dd00dd",
//			"hold.engineR": "00dd00dd",
//			"hold.engineR.sn": "xxxxx",
//			"hold.UltraFlow": "dd00dd00"
//		}
//	},
//	"ts": 6883838383
//}


static int bin2hexString(char *outS, unsigned short *inD, int size)
{
	int i;
	char tmp[16] = {0};
	char *p = outS;
	
	for(i = 0; i < size; i++)
	{
		sprintf(tmp, "%02x%02x", *(inD+i) >> 8, (*(inD+i)) & 0xff);
		p = strcat(p, tmp);
	}
		

	return 0;
}

int mds_merge_result(mds_scan_t *scan, char **result)
{
	
	cJSON *res_merge = cJSON_CreateObject();
	if(res_merge == NULL)
	{
		return -1;
	}

	cJSON *values = cJSON_CreateObject();
	if(res_merge == NULL)
	{
		return -1;
	}
	
	cJSON *ty_str = cJSON_CreateString("rawData");
	if(ty_str == NULL)
	{
		
		cJSON_Delete(res_merge);
		
		return -1;
	}
	
	cJSON_AddItemToObject(values, "type", ty_str);
	
	cJSON *item = cJSON_CreateObject();
	if(item == NULL)
	{
		cJSON_Delete(res_merge);
		
		return -1;
	}
	
	//meger all data info
	for(mds_item_t *node = scan->head; node != NULL; node = node->next){
		
		char tmp[node->size * 2 + 1];
		
		memset(tmp, 0, sizeof(tmp));
		
		switch(node->type){
			
			case MB_TYPE_INPUT_REG_E:
						
				bin2hexString(tmp, node->value, node->size / 2);
			
				char table[32] = {0};
				
				sprintf(table, "input.%s", node->reg.input.module);
			
				cJSON *tmp_str = cJSON_CreateString(tmp);
				if(tmp_str != NULL)
				{
					cJSON_AddItemToObject(item, table, tmp_str);
				}
			
				break;
			
			default:
				break;
			
		}
	
	}
	
	cJSON_AddItemToObject(values, "rawData", item);

	cJSON_AddItemToObject(res_merge, "values", values);
	
	//add timestamp info
	time_t now = m_time(NULL);
	
	cJSON* ts = cJSON_CreateNumber(now); 
	
	
	cJSON_AddItemToObject(res_merge, "ts", ts);
	
	
	if(*result)
		*result = cJSON_PrintUnformatted(res_merge);
	
	
	cJSON_Delete(res_merge);
	
	
	return 0;
}




mds_scan_t * mds_scan_service_init(void)
{
	mds_scan_t *mdss = m_malloc(sizeof(mds_scan_t));
	if(!mdss){
		return NULL;
	}

	memset(mdss, 0, sizeof(mds_scan_t));


	mdss->userdata = MBT_masterInit();
	
	mdss->ops.read_input_regs = MBT_masterReadInputRegs;
	
	

	return mdss;
}


