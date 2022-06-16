#ifndef _MODBUS_APP_H_
#define _MODBUS_APP_H_



typedef enum mb_type{
	MB_TYPE_COILS_E = 0,
	MB_TYPE_DISCRET_INPUT_E,
	MB_TYPE_INPUT_REG_E,
	MB_TYPE_HOLD_REG_E
	
}mb_type_e;


typedef enum scan_stat{
	MB_SCAN_SUCCESS,
	MB_SCAN_FAIL
	
}scan_stat_e;


typedef char* (*convert)(char *json);


typedef struct reg_input{
	char					*module;
	int 					start;
	int						offset;
	unsigned int 	dev_addr;		
}reg_input_t;


typedef struct reg_hold{
	char 				  *module;
	int						mode;
	int 					start;
	int						offset;
	unsigned int 	dev_addr;	
	
	
}reg_hold_t;



typedef struct mds_item{
	union{
		reg_input_t input;
		reg_hold_t hold;
	}reg;
	
	mb_type_e type;
	unsigned short *value;
	int size;
	
	
	scan_stat_e stat;
	
	struct mds_item * next;
	struct mds_item * prev;
	
}mds_item_t;


typedef struct modbus_ops{
	
	int (*read_input_regs)(void *ctx ,int addr, int nb, unsigned short *dest);
	
	
	
	
}modbus_ops_t;


typedef struct mds_scan{
	
	mds_item_t * head;
	
	
	modbus_ops_t ops;
	
	void *userdata;
	
}mds_scan_t;



mds_scan_t * mds_scan_service_init(void);

int mds_parse_scan(char *json, convert c, mds_scan_t *result);

int mds_scan(mds_scan_t *scan);

int mds_merge_result(mds_scan_t *scan, char **result);



//port interface
void *modbus_master_init(void);


int modbus_master_read_input_regs(void *ctx ,int addr, int nb, unsigned short *dest);




#endif
