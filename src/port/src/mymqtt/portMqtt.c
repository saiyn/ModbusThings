#include "modbus_realtime.h"
#include "mqtt_client.h"

#include <string.h>

#define DBG_TAG              "portMqtt"
#define DBG_LVL              DBG_LOG
#include <rtdbg.h>

struct rt_messagequeue mqtt_mq;
static char _msg_pool[512];

void * mqtt_client_init(char *token, char *cid, char *uri)
{
	
		mqtt_client *mqtt = rt_malloc(sizeof(mqtt_client));
		if(!mqtt){
			return NULL;
		}
	
	
	/* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
	
		
		mqtt->isconnected = 0;
    mqtt->uri = uri;
	
	
		/* config connect param */
		rt_memcpy(&mqtt->condata, &condata, sizeof(condata));
		mqtt->condata.clientID.cstring = cid;

		mqtt->condata.username.lenstring.data = token;
		mqtt->condata.username.lenstring.len = rt_strlen(token);
		mqtt->condata.password.cstring = "";
		mqtt->condata.keepAliveInterval = 30;
	
		mqtt->condata.cleansession = 1;
		
		
		/* malloc buffer. */
		mqtt->buf_size = mqtt->readbuf_size = 1024;
		mqtt->buf = rt_calloc(1, mqtt->buf_size);
		mqtt->readbuf = rt_calloc(1, mqtt->readbuf_size);
		if (!(mqtt->buf && mqtt->readbuf))
		{
				LOG_E("no memory for MQTT client buffer!");
				return NULL;
		}
		
	
	return mqtt;
}


static void mqtt_connect_callback(mqtt_client *c)
{
	c->stat_ops.onConnect();
    
}

static void mqtt_online_callback(mqtt_client *c)
{
	c->stat_ops.onConnected();
}

static void mqtt_offline_callback(mqtt_client *c)
{
	c->stat_ops.onDisconnected();
}


static void mqtt_sub_callback(mqtt_client *c, message_data *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_I("mqtt sub callback: %.*s %.*s",
               msg_data->topic_name->lenstring.len,
               msg_data->topic_name->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
	
	
		c->msg_ops.onMessage((char *)msg_data->message->payload);
	
}

static void mqtt_sub_default_callback(mqtt_client *c, message_data *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_I("mqtt sub default callback: %.*s %.*s",
               msg_data->topic_name->lenstring.len,
               msg_data->topic_name->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}


static void mqtt_app_thread(void *arg)
{
	rt_err_t result;
	mqtt_client *mqtt = (mqtt_client *)arg;
	
	int rc = 0;
	telemetry_t msg;
	
	for(;;)
	{

		result = rt_mq_recv(&mqtt_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
		if(result == RT_EOK){
			
			rc = paho_mqtt_publish(mqtt, QOS1, msg.topic , msg.msg, rt_strlen(msg.msg));
			
			rt_free(msg.msg);
			
			if(rc == 0){
				LOG_I("publish one msg success");
			}
			
		}
		
	}
			
}


void mqtt_client_start(void *userdata, stat_ops_t *stat_ops, msg_ops_t *msg_ops)
{
	mqtt_client *mqtt = (mqtt_client *)userdata;
	
	mqtt->stat_ops = *stat_ops;
	
	mqtt->msg_ops = *msg_ops;
	
	
	//		/* set event callback function */
	mqtt->connect_callback = mqtt_connect_callback;
	mqtt->online_callback = mqtt_online_callback;
	mqtt->offline_callback = mqtt_offline_callback;

	/* set subscribe table and event callback */
	mqtt->message_handlers[0].topicFilter = rt_strdup(msg_ops->topic);
	mqtt->message_handlers[0].callback = mqtt_sub_callback;
	mqtt->message_handlers[0].qos = QOS1;
	

	/* set default subscribe event callback */
	mqtt->default_message_handlers = mqtt_sub_default_callback;
		
		
		
	{
      int value;
      uint16_t u16Value;
      value = 5;
      paho_mqtt_control(mqtt, MQTT_CTRL_SET_CONN_TIMEO, &value);
      value = 5;
      paho_mqtt_control(mqtt, MQTT_CTRL_SET_MSG_TIMEO, &value);
      value = 30;
      paho_mqtt_control(mqtt, MQTT_CTRL_SET_RECONN_INTERVAL, &value);
      value = 30;
      paho_mqtt_control(mqtt, MQTT_CTRL_SET_KEEPALIVE_INTERVAL, &value);
      u16Value = 3;
      paho_mqtt_control(mqtt, MQTT_CTRL_SET_KEEPALIVE_COUNT, &u16Value);
	}

	/* run mqtt client */
	paho_mqtt_start(mqtt, 2048, 12);
	
	rt_thread_t tid;
	rt_err_t ret = RT_EOK;

	ret = rt_mq_init(&mqtt_mq, "mqtt_mq", _msg_pool, sizeof(telemetry_t), sizeof(_msg_pool),RT_IPC_FLAG_FIFO);
	if(ret != RT_EOK)
	{
		LOG_E("create mqtt mq failed");
		
		
		//here should call clean function
		
		return;
	}


	tid = rt_thread_create("mqtt_app", mqtt_app_thread, mqtt, 4096, 20, 20);
	if(tid != RT_NULL)
	{
		rt_thread_startup(tid);
	}
	else
	{
		LOG_E("create mqtt app thread failed");
	}
	
	
}



int mqtt_push_data(void *userdata, telemetry_t *tt, char *topic)
{
	
	mqtt_client *mqtt = (mqtt_client *)userdata;
	
	int err = rt_mq_send(&mqtt_mq, (void *)tt, sizeof(*tt));
	if(err == -RT_EFULL){
		
		return -REALTIME_ERR_QUEUE_FULL;
	}
		
	
	return 0;
}

static int do_dump(void *dst, void * src)
{
	telemetry_t *msg = (telemetry_t *)src;
	
	strcpy(dst, msg->msg);
	
	return strlen(msg->msg);
}


int mqtt_dump_cached_data(void *userdata, char *buf, int off, int limit)
{
	mqtt_client *mqtt = (mqtt_client *)userdata;
	
	
	return rt_mq_dump_limit(&mqtt_mq, buf, off, limit, do_dump);

}







