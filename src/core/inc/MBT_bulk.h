#ifndef _MODBUS_BULK_H_
#define _MODBUS_BULK_H_

#define BULK_FILE_MAX_BYTES  (10240)


//hold a defined max size cache in the memory, which will be used in get_tail_cache_file_path

//cache file name format: <date>_<version>_<index>



typedef struct mdb_bluk{
	
	int cur_cached_off;
	
	char* cache_buf;
	
	int next_index;
	
	char *dev_token;

	int version_number;
	

	//when a device offline for a long time, and cached many days files, then when the device online, it frist try
//to upload same date and version file and the next closet date file
	char * (*get_head_cache_file_path)(struct mdb_bluk *);
	
	//very latest file
	char * (*get_tail_cache_file_path)(struct mdb_bluk *);
	
	int (*save_bulk_data)(struct mdb_bluk *);
	
	
	int (*post_bluk_data_by_file)(struct mdb_bluk *, char *uri, char *file_name);

	int (*update_tail_index)(struct mdb_bluk *);
	
	
	int (*update_head_index)(struct mdb_bluk *);
	
	
}mdb_bluk_t;




mdb_bluk_t * mdb_bulk_service_init(char *);










#endif
