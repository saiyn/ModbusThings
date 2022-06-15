#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "modbus.h"
#include "MBT_portMaster.h"

#include "MBT_config.h"


#define RS485_RE GET_PIN(A, 8)


void *modbus_master_init()
{
	modbus_t *ctx = RT_NULL;
	
	rt_pin_mode(RS485_RE, PIN_MODE_OUTPUT);
	
	
	ctx = modbus_new_rtu(MODBUS_MASTER_DEVICE, MODBUS_MASTER_BAUDRATE, MODBUS_MASTER_PORITY, 8, 1);
	modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
	modbus_rtu_set_rts(ctx, RS485_RE, MODBUS_RTU_RTS_UP);
	modbus_set_slave(ctx, 1);
	modbus_connect(ctx);
	modbus_set_response_timeout(ctx, 0, 999999);
	modbus_set_debug(ctx, 1);
	
	return ctx;
}


int modbus_master_read_input_regs(void *ctx ,int addr, int nb, unsigned short *dest)
{
	
	return modbus_read_input_registers(ctx, addr, nb, dest);
	
}
	
	
	
	
	
