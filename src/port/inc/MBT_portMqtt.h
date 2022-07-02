#ifndef _MBT_PORT_MQTT_H_
#define _MBT_PORT_MQTT_H_


struct stat_ops;
struct msg_ops;
struct telemetry;

/**
 * @brief 
 * 
 * @param token pass through username
 * @param cid as client id
 * @param uri 
 * @return void* handle 
 */
void * mqtt_client_init(char *token, char *cid, char *uri);

/**
 * @brief 
 * 
 * @param userdata 
 * @param stat_ops 
 * @param msg_ops 
 */
void mqtt_client_start(void *userdata, struct stat_ops *stat_ops, struct msg_ops *msg_ops);


/**
 * @brief 
 * 
 * @param userdata 
 * @return int 
 */
int mqtt_push_data(void *userdata, struct telemetry *, char *);

/**
 * @brief 
 * 
 * @param userdata 
 * @return int 
 */
int mqtt_get_cached_data_size(void *userdata);

/**
 * @brief 
 * 
 * @param userdata 
 * @param buf 
 * @param off 
 * @param limit 
 * @return int 
 */
int mqtt_dump_cached_data(void *userdata, char *buf, int off, int limit);





#endif

