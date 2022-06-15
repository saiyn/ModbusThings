#ifndef _MBT_CONFIG_H_
#define _MBT_CONFIG_H_

//realtime service
#define DEFAULT_SERVER_FQDN         "tqauto.asuscomm.com:9529"
#define DEFAULT_DEV_UUID            "ModbusThings-000001"
#define DEFAULT_BULK_SERVICE_API    ""
#define REALTIME_SUBTOPIC           "v1/devices/me/attributes"
#define TELEMETRY_TOPIC             "v1/devices/me/telemetry"
#define MQTT_OUT_CACHE_MAX          (50)
#define MQTT_OUT_THREAD_PRIORITY    (20)



#define DEV_PROVISION_API  "/api/v1/provision"

#define DEV_PROVISION_NAME_PREFIX  "trans"


#define DEV_ATTRI_CONFIG_KEY  "mb_config"



#define DEV_ATTRI_CONFIG_EXPIRE_SECOND  (60*60*12)


//modbus master
#define MODBUS_MASTER_DEVICE  "/dev/uart1"
#define MODBUS_MASTER_BAUDRATE  9600
#define MODBUS_MASTER_PORITY   'N'
#define MODBUS_SLAVE_ID         1


//core thread
#define MODBUS_CORE_THREAD_STACK_SIZE  (4096)
#define MODBUS_CORE_THREAD_PRIORITY    (23)

//db
#define ATTRIBUTE_KEY_PREFIX  "MBT_ATTRI"
#define MBT_DB_FILE_PATH        "/opt/MBT/simple.db"



#endif
