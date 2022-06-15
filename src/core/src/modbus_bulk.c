#include <stdlib.h>
#include <stdio.h>

#include "modbus_bulk.h"

#include "modbus_attributes.h"

#include "modbus_system.h"


#define DBG_TAG              "mdbb"
#define DBG_LVL              DBG_LOG
#include <rtdbg.h>

static char *gen_cache_filename(int index, int version)
{
	//cache file name format: <date>_<version>_<index>
	time_t now;
	
	now = time(NULL);
	
	struct tm *p_tm = localtime(&now);
	
	char tmp[64] = {0};
	
	int rc = snprintf(tmp, sizeof(tmp), "%d-%d-%d_%d_%d", p_tm->tm_year, p_tm->tm_mon, p_tm->tm_mday, version, index);
	if(rc >= sizeof(tmp)){
		return NULL;
	}
	
	return m_strdup(tmp);
	
}

//very first file
static char *fetch_head_cache_filename(int index, int version)
{
	return gen_cache_filename(index, version);
	
}

//very last file
static char *fetch_tail_cache_filename(int index, int version)
{
	return gen_cache_filename(index, version);
}



static int save_bulk_data(struct mdb_bluk *mdbb)
{
	int rc = 0;
	
	
	char *fname = gen_cache_filename(mdbb->next_index, mdbb->version_number);
	
	
	int fd = m_open(fname, O_CREAT | O_RDWR | O_TRUNC);
	if(fd < 0)
	{
		LOG_E("can't save file:%s", fname);
		return -1;
	}
	
	int ret = m_write(fd, mdbb->cache_buf, mdbb->cur_cached_off);
	if(ret !=  mdbb->cur_cached_off)
	{
		LOG_E("write file:%s fail:%d-%d", fname, ret,  mdbb->cur_cached_off);
		rc = -1;
	}
	
	m_close(fd);
	
	
	return rc;
}


static char *get_head_cache_file_path(struct mdb_bluk *mdbb)
{
	
	char *fname = fetch_head_cache_filename(mdbb->next_index - 1, mdbb->version_number);
	
	
	
	return fname;
}

static char *get_tail_cache_file_path(struct mdb_bluk *mdbb)
{
	
	char *fname = fetch_tail_cache_filename(0, mdbb->version_number);
	
	
	
	return fname;
	
	
}


static int post_bluk_data_by_file(struct mdb_bluk *mdbb, char *uri, char *file_name)
{
	


	
	return 0;
}


mdb_bluk_t * mdb_bulk_service_init(void)
{
	mdb_bluk_t * mdbb = m_malloc(sizeof(mdb_bluk_t));
	if(!mdbb){
		return NULL;
	}
	
	mdbb->cur_cached_off = 0;
	
	mdbb->cache_buf = m_malloc(BULK_FILE_MAX_BYTES);
	if(!mdbb->cache_buf){
		return NULL;
	}
	
	
	int rc = mba_load_attribute_int(ATTRI_BULK_INDEX, &mdbb->next_index);
	if(rc != 0)
		mdbb->next_index = 0;
	
	
	rc = mba_load_attribute_int(ATTRI_BULK_FORMAT_VERSION, &mdbb->version_number);
	if(rc != 0)
		mdbb->version_number = 0;
	
	
	mdbb->save_bulk_data = save_bulk_data;
	
	mdbb->get_head_cache_file_path = get_head_cache_file_path;
	
	mdbb->get_tail_cache_file_path = get_tail_cache_file_path;
	
	mdbb->post_bluk_data_by_file = post_bluk_data_by_file;
	
	return mdbb;
}



