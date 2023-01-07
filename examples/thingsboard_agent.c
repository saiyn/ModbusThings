#include "MBT_os.h"
#include "ModbusThings.h"

#include "MBT_log.h"


int main(int agrc, char* argv[])
{

    MBT_initLog("/tmp/modbusThings/mbt.log", 100000, 1);

    MBT_LOG_INFO("try start modbus core\n");

    modbus_core_start();

    


    return 0;
}