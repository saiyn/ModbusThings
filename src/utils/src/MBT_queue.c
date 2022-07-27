#include <pthread.h>
#include <assert.h>
#include <sys/time.h>

#include "MBT_osMemory.h"

#include "MBT_queue.h"


typedef struct qitem
{
    void *msg;
    

    struct qitem *next;
}qitem_t;


typedef struct queue{
    qitem_t *head;
    qitem_t *tail;

    pthread_mutex_t lock;

    pthread_cond_t  notify;

    int itemNum;

    int itemMax;
}queue_t;


MBT_MQ MBT_mqCreate(int maxItem)
{
    queue_t * q = m_malloc(sizeof(queue_t));

    pthread_mutex_init(&q->lock, NULL);

    pthread_cond_init(&q->notify, NULL);

    q->itemMax = maxItem;
    q->itemNum = 0;

    q->head = q->tail = NULL;

    return q;
}


int MBT_mqSend(MBT_MQ mq, void *msg)
{

    queue_t * q = (queue_t *)mq;

    pthread_mutex_lock(&q->lock);

    if(q->itemNum + 1 > q->itemMax){

        pthread_mutex_unlock(&q->lock);

        return -MBT_QERROR_FULL;
    }

    qitem_t *item = m_malloc(sizeof(qitem_t));

    item->msg = msg;
    item->msg = NULL;

    if(q->tail){
        q->tail->next = item;
        q->tail = item;
    }else{
        q->tail = item;
        q->head = q->tail;
    }

    q->itemNum++;

    pthread_mutex_unlock(&q->lock);


    pthread_cond_signal(&q->notify);

    return 0;

}


int MBT_mqRecv(MBT_MQ mq, void **msg, int timeout)
{
    int rc = 0;

    queue_t * q = (queue_t *)mq;

    pthread_mutex_lock(&q->lock);

    if(q->itemNum < 1){

        if(timeout > 0){

            struct timeval tv;

            gettimeofday(&tv, NULL);

            struct timespec ts = {
                .tv_sec = tv.tv_sec + timeout,
                .tv_nsec = tv.tv_usec * 1000,
            };

            rc = pthread_cond_timedwait(&q->notify, &q->lock, &ts);

            if(rc < 0){
                pthread_mutex_unlock(&q->lock);

                return -1;
            }

        }else{
            pthread_cond_wait(&q->notify, &q->lock);
        }

        
    }

    *msg = q->head->msg;

    if(q->tail == q->head){
        q->tail = NULL;

        assert(q->itemNum == 1);
    }

    q->head = q->head->next;

    q->itemNum--;


    pthread_mutex_unlock(&q->lock);

    return 0;
}



int MBT_mqDumpLimit(MBT_MQ mq, int (*do_dump)(void *msg, char *buf, size_t size), char *buf, int off, size_t max)
{
    size_t total = 0;

    queue_t * q = (queue_t *)mq;

    pthread_mutex_lock(&q->lock);

    while(q->itemNum){

        qitem_t *item = q->head;

        int rc = do_dump(item->msg, buf+off, max - total);
        if(rc < 0){
            return total;
        }

        off += rc;
        total += rc;

        q->head = q->head->next;

        q->itemNum--;

    }

    if(q->itemNum == 0){

        assert(q->head == NULL);

        q->tail = NULL;
    }

    return total;

}