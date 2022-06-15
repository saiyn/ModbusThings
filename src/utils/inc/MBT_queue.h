#ifndef _MBT_QUEUE_H_
#define _MBT_QUEUE_H_

typedef void* MBT_MQ;

enum {
    MBT_QERROR_NONE = 0,
    MBT_QERROR_FULL = 1,

};


MBT_MQ MBT_mqCreate(int maxItem);


int MBT_mqSend(MBT_MQ mq, void *msg, size_t size);


int MBT_mqSendUrgent(MBT_MQ mq, void *msg, size_t size);


int MBT_mqRecv(MBT_MQ mq, void **msg, int timeout);


int MBT_mqDumpLimit(MBT_MQ mq, void (*do_dump)(void *msg, char *buf, size_t size), char *buf, size_t size, size_t max);


#endif

