#ifndef _MBT_CONFIG_H_
#define _MBT_CONFIG_H_

//realtime service
#define REALTIME_SUBTOPIC_MAX_NUM   (3)
#define DEFAULT_SERVER_FQDN         "tqauto.asuscomm.com:9529"
#define DEFAULT_DEV_UUID            "ModbusThings-000001"
#define DEFAULT_BULK_SERVICE_API    ""
#define REALTIME_ATTR_SUBTOPIC           "v1/devices/me/attributes"
#define REALTIME_OTA_SUBTOPIC		"v1/devices/me/attributes/response/+"

#define TELEMETRY_TOPIC             "v1/devices/me/telemetry"
#define MQTT_OUT_CACHE_MAX          (50)
#define MQTT_OUT_THREAD_PRIORITY    (20)

#define REALTIME_SERVER_DEFAULT     "tcp://tqauto.asuscomm.com:1884"

#define DEV_PROVISION_API  "/api/v1/provision"

#define DEV_PROVISION_NAME_PREFIX  "trans"


#define DEV_ATTRI_CONFIG_KEY  "mb_config"


#define DEV_OTA_API_PREFIX				"api/v1"
#define DEV_OTA_API_SUFFIX				"firmware"


#define DEV_OTA_FILE_PATH         "/ota_firmware.bin"


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
#define ATTRIBUTE_KEY_PREFIX        "MBT_ATTRI"
#define MBT_DB_FILE_PATH            "/opt/MBT/simple.db"
#define MBT_DB_FILE_PATH_LENGTH_MAX (512)

//bulk service
#define BILK_SERVICE_API_SUFFIX		"bulktelemetry"
#define BULK_SERVICE_API_PREFIX		"api/v1"
#define BULK_SERVICE_FORM_STR		"filed1"			
#define BULK_SAVE_FILE_PREFIX   	"mbt"
#define BULK_SAVE_FILE_SUFFIX   	"dat"
#define BULK_UPLOADED_FILE_SUFFIX	"bk"

#endif
