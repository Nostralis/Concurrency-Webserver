#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
typedef struct tPool tPool;
tPool* defPool(int maxThread, int buf);

void add_req(tPool *, void *(*routine)(void *), void *arg);
void stop(tPool *);
void kille(tPool *);
