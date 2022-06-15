#ifndef _MBT_PORT_MASTER_H_
#define _MBT_PORT_MASTER_H_



void *MBT_masterInit(void);


int MBT_masterReadInputRegs(void *ctx ,int addr, int nb, unsigned short *dest);





#endif

