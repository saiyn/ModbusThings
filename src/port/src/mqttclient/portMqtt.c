#include <stddef.h>
#include "MBT_config.h"

#include "MBT_realtime.h"
#include "MBT_portMqtt.h"

#include "MBT_queue.h"
#include "MBT_osThread.h"

#include "MBT_osMemory.h"
#include "mqttclient.h"


#define container_of(ptr, type, member) \
  ((type*) ((uintptr_t) (ptr) - offsetof(type, member)))

typedef struct mqttAdapter{

    mqtt_client_t *client;
    struct msg_ops msg_ops;
    MBT_MQ mq;
}mqttAdapter_t;

//static struct mqttAdapter _mqtt;


void * mqtt_client_init(char *token, char *cid, char *uri)
{
    mqtt_client_t *client =  mqtt_lease();

    mqtt_set_host(client, uri);
    mqtt_set_port(client, "1883");
    mqtt_set_user_name(client, token);
    mqtt_set_client_id(client, cid);
    mqtt_set_clean_session(client, 1);

    mqttAdapter_t *mqtt = m_malloc(sizeof(mqttAdapter_t));

    mqtt->client = client;

    return mqtt;
}


static void mqtt_sub_callback(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");

    struct mqttAdapter *mqtt = container_of(client, struct mqttAdapter, client);


    mqtt->msg_ops.nodes[0].onMessage((char*)msg->message->payload);

}
 
static void mqtt_send_thread(void *arg)
{
    mqttAdapter_t *mqtt = (mqttAdapter_t *)arg;

    telemetry_t *raw;

    int rc;

    for(;;){

        rc = MBT_mqRecv(mqtt->mq, (void **)&raw, -1);
        if(rc == 0){
            mqtt_message_t msg;
            memset(&msg, 0, sizeof(msg));

            msg.qos = 1;
            msg.payload = raw->msg;

            mqtt_publish(mqtt->client, raw->topic, &msg);


            m_free(raw->msg);

            m_free(raw);
        }

    }


}


void mqtt_client_start(void *userdata, struct stat_ops *stat_ops, struct msg_ops *msg_ops)
{
    mqttAdapter_t *mqtt = (mqttAdapter_t *)userdata;

    mqtt_client_t *client = mqtt->client;


    mqtt_connect(client);

    mqtt_subscribe(client, msg_ops->nodes[0].topic, QOS1, mqtt_sub_callback);


    mqtt->msg_ops = *msg_ops;

    mqtt->mq = MBT_mqCreate(REALTIME_OUT_CACHE_MAX);


    MBT_threadCreate("mqtt_send",mqtt_send_thread, mqtt, 4096, MQTT_OUT_THREAD_PRIORITY);

}


int mqtt_push_data(void *userdata, struct telemetry *tt, char *topic)
{
    (void)topic;

    mqttAdapter_t *mqtt = (mqttAdapter_t *)userdata;

    int rc = MBT_mqSend(mqtt->mq, tt);
    if(rc == -MBT_QERROR_FULL){
        return -REALTIME_ERR_QUEUE_FULL;
    }

    return rc;
}


static int do_dump(void *msg, char *buf, size_t size)
{
    struct telemetry *tt = (struct telemetry *)msg;

    if(tt->size > size){
        return -1;
    }

    memcpy(buf, tt->msg, tt->size);

    m_free(tt->msg);

    m_free(tt);

    return 0;
}


int mqtt_dump_cached_data(void *userdata, char *buf, int off, int limit)
{

    mqttAdapter_t *mqtt = (mqttAdapter_t *)userdata;

    return MBT_mqDumpLimit(mqtt->mq, do_dump, buf, off, limit);
}

