#include <stdlib.h>
#include <stdio.h>

#include "MBT_bulk.h"

#include "modbus_attributes.h"


#define DBG_TAG              "mdbb"
#define DBG_LVL              DBG_LOG
#include <rtdbg.h>

static char*get_date_str(void)
{
	static char date[32] = {0};
	
	time_t now;
	
	now = time(NULL);
	
	struct tm *p_tm = localtime(&now);

	
	snprintf(date, sizeof(date), "%d-%d-%d", p_tm->tm_year, p_tm->tm_mon, p_tm->tm_mday);
	
	
	return date;
}



static char *gen_cache_filename(int index, int version)
{
	//cache file name format: mbt_<index>.dat
	static char tmp[64] = {0};
	
	
	memset(tmp, 0, sizeof(tmp));
	
	int rc = snprintf(tmp, sizeof(tmp), "%s_%d.%s", BULK_SAVE_FILE_PREFIX, index, BULK_SAVE_FILE_SUFFIX);
	if(rc >= sizeof(tmp)){
		return NULL;
	}
	
	return tmp;
	
}



static void update_index(struct mdb_bluk *mdbb)
{
	//1.check whether we have set the ATTRI_BULK_FILE_TAIL_INDEX and ATTRI_BILK_FILE_TAIL_DATE
	int tail_index = 0;
	int rc = mba_load_attribute_int(ATTRI_BULK_FILE_TAIL_INDEX, &tail_index);
	if(rc < 0){
		//no tail index yet, set here
		mba_save_attribute_int(ATTRI_BULK_FILE_TAIL_INDEX, mdbb->next_index);
		//mba_save_attribute(ATTRI_BILK_FILE_TAIL_DATE, get_date_str());
	}
	
	//2.update ATTRI_BULK_INDEX
	mdbb->next_index++;
	
	mba_save_attribute_int(ATTRI_BULK_INDEX, mdbb->next_index);
	
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
	
	//update indexs
	update_index(mdbb);
	

	
	return rc;
}

//get the very old file
static char *get_tail_cache_file_path(struct mdb_bluk *mdbb)
{
	int index = 0;
	
	int rc = mba_load_attribute_int(ATTRI_BULK_FILE_TAIL_INDEX, &index);
	if(rc < 0){
		return NULL;
	}
	
	return gen_cache_filename(index, mdbb->version_number);
}

//update the very old index
static int update_tail_index(struct mdb_bluk *mdbb)
{
	int index = 0;
	
	int rc = mba_load_attribute_int(ATTRI_BULK_FILE_TAIL_INDEX, &index);
	//we should never fail here, since we should get result from cache
	if(rc < 0){
		return -1;
	}
	
	char new_name[64] = {0};
	
	//since all things done in the same thread, we don't warry the index change outside
	char *name = gen_cache_filename(index, mdbb->version_number);
	
	snprintf(new_name, sizeof(new_name), "%s.%s", name, BULK_UPLOADED_FILE_SUFFIX);
	
	rc = rename(name, new_name);
	
	index++;
	
	//if the tail index get very large, we should delete these files
	
	
	
	
	return mba_save_attribute_int(ATTRI_BULK_FILE_TAIL_INDEX, index);
}


//get the very latest file
static char *get_head_cache_file_path(struct mdb_bluk *mdbb)
{
	if(mdbb->next_index > 0)
		return gen_cache_filename(mdbb->next_index - 1, mdbb->version_number);
	
	
	return NULL;
}

//update the head index
static int update_head_index(struct mdb_bluk *mdbb)
{
	//we just rename the file
	//we should rename the file to prevent repeate upload
	
	char new_name[64] = {0};
	
	//since all things done in the same thread, we don't warry the index change outside
	char *name = gen_cache_filename(mdbb->next_index - 1, mdbb->version_number);
	
	snprintf(new_name, sizeof(new_name), "%s.%s", name, BULK_UPLOADED_FILE_SUFFIX);
	
	return m_rename(name, new_name);
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

	mdbb->update_tail_index = update_tail_index;
	
	mdbb->update_head_index = update_head_index;
	
	return mdbb;
}



