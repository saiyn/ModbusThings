
#include "modbus.h"
#include "MBT_config.h"
#include "MBT_portMaster.h"


void *MBT_masterInit(void)
{
    modbus_t *ctx = NULL;

    ctx = modbus_new_rtu(MODBUS_MASTER_DEVICE, MODBUS_MASTER_BAUDRATE, MODBUS_MASTER_PORITY, 8, 1);

}


int MBT_masterReadInputRegs(void *ctx ,int addr, int nb, unsigned short *dest)
{



}