#include <pthread.h>
#include "request.h"
#include "io_helper.h"
#include "pool.h"


struct rout
{
    void *(*routine)(void *);
    void *arg;
};

struct tPool
{
    pthread_t *workers;
    struct rout* queue;
    int head,end;
    int max_workers;
    int max_task;
    int queued;
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t empty;

};

void add_req(struct tPool *pool, void *(*routine)(void *), void *arg)
{
    pthread_mutex_lock(&pool->mutex);

    if (pool->head == pool->end)
    {
        pthread_cond_broadcast(&pool->notEmpty);
    }
    printf("req");
    struct rout req;
    req.routine=routine;
    req.arg=arg;

    pool->queue[pool->end]=req;
    pool->end++;
    pool->end%=pool->max_task;

    pthread_mutex_unlock(&pool->mutex);
}

void stop(struct tPool *pool)
{
    pthread_mutex_lock(&pool->mutex);
    while(pool->queued>0)
    {
        pthread_cond_wait(&pool->notEmpty, &pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
}

void *manage(void *poolArgs)
{
    struct tPool *pool = (struct tPool *)poolArgs;
    while(1)
    {
        struct rout currentTask;
        pthread_mutex_lock(&pool->mutex);

        while (pool->head == pool->end)
        {
            pthread_cond_wait(&pool->notEmpty, &pool->mutex);
        }

        currentTask = pool->queue[pool->head];
        pool->head++;
        pool->head%=pool->max_task;
        pool->queued++; //task scheduled

        pthread_mutex_unlock(&pool->mutex);

        currentTask.routine(currentTask.arg);

        pthread_mutex_lock(&pool->mutex);

        pool->queued--;

        if(pool->queued==0)
        {
            pthread_cond_signal(&pool->empty);
        }
        pthread_mutex_unlock(&pool->mutex);
    }
}

struct tPool* defPool(int maxThread, int buf)
{
    struct tPool* pool = malloc(sizeof(struct tPool));
    pool->head=0;
    pool->head=0;
    pool->queued=0;
    pool->queue = malloc(sizeof(pthread_t)*maxThread);
    pool->max_task=buf;
    pool->max_workers=maxThread;
    pool->workers = malloc(sizeof(pthread_t)*maxThread);

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->notEmpty, NULL);
    pthread_cond_init(&pool->empty, NULL);

    for (int i=0; i<maxThread; i++)
    {
        pthread_create(&pool->workers[i], NULL, manage, pool);
    }

    return pool;
}

void kille(struct tPool *pool)
{
    stop(pool);

    for(int i=0; i<pool->max_workers;i++)
    {
        pthread_detach(pool->workers[i]);
    }

    free(pool);

}
