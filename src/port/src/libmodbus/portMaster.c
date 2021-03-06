
#include "modbus.h"
#include "MBT_config.h"
#include "MBT_portMaster.h"


void *MBT_masterInit(void)
{
    modbus_t *ctx = NULL;

    ctx = modbus_new_rtu(MODBUS_MASTER_DEVICE, MODBUS_MASTER_BAUDRATE, MODBUS_MASTER_PORITY, 8, 1);

    modbus_set_debug(ctx, TRUE);
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

    modbus_set_slave(ctx, MODBUS_SLAVE_ID);                    

    modbus_set_response_timeout(ctx, 0, 999999);


    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }      


    return ctx;
}


int MBT_masterReadInputRegs(void *ctx ,int addr, int nb, unsigned short *dest)
{
    return modbus_read_input_registers(ctx, addr, nb, dest);
}