
#include "MBT_config.h"
#include "MBT_osFs.h"
#include "cJSON.h"
#include "MBT_simpleDb.h"

typedef struct simple_db{
    int size;
    char **array;
}simple_db_t;



static int parse_load(char *buf, simple_db_t *db)
{
    cJSON *root = cJSON_Parse(buf);
    if(!root){
        return -1;
    }

    cJSON *item = NULL;

    int index = 0;
    char key[32] = {0};

    cJSON_ArrayForEach(item, root){

        memset(key, 0, sizeof(key));

        sprintf(key, "%s_%d", ATTRIBUTE_KEY_PREFIX, index++);

        cJSON *att = cJSON_GetObjectItem(item, key);

        if(!att){
            db->array[index] = NULL;
        }else{
            db->array[index] = m_strdup(att->valuestring);
        }

        if(db->size < index){
            break;
        }
    }

}

static int convert_save(simple_db_t *db)
{
    


}

static char* load_data(void)
{
    int fd = m_open(MBT_DB_FILE_PATH, O_RDWR | O_CREAT);
    if(fd < 0){
        return NULL;
    }

    struct stat buf;

    int rc = m_stat(MBT_DB_FILE_PATH, &buf);
    if(rc != 0){
        return -1;
    }

    char *buffer = m_malloc(buf.st_size+1);
    if(!buffer){
        m_close(fd);
        return NULL;
    }

    int size = m_read(fd, buffer, buf.st_size);

    if(size != buf.st_size){

    }

    m_close(fd);

    buffer[buf.st_size] = '\0';

    return buffer;

}

int MBT_simpleDbInit(void **handle, int key_num_max)
{
    simple_db_t *db = m_malloc(sizeof(simple_db_t));
    if(!db){
        return -1;
    }

    db->array = m_malloc(sizeof(char *) * key_num_max);
    if(!db->array){
        free(db);
        return -1;
    }

    db->size = key_num_max;

    *handle = db;

    char *buffer = load_data();
    if(!buffer){
        return -1;
    }

    parse_load(buffer, db);


    m_free(buffer);

    return 0;
}



int MBT_simpleDbPut(MBT_DB_HANDLE h, MBT_DB_KEY key, MBT_DB_VALUE value)
{
    simple_db_t *db = (simple_db_t *)h;

    if(key < db->size){
        db->array[key] = value;

        convert_save(db);
    }


    return -1;
}


MBT_DB_VALUE MBT_simpleDbGet(MBT_DB_HANDLE h, MBT_DB_KEY key)
{
    simple_db_t *db = (simple_db_t *)h;

    if(key < db->size){
        return db->array[key];
    }

    return NULL;
}