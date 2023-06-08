#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define LL_ADD(item, list)do{           \
    item->prev = NULL;                  \
    item->next = list;                  \
    if(list != NULL) list->prev = item; \
    list = item;                        \
}while(0);

#define LL_REMOVE(item, list)do{        \
    if(item->prev != NULL) item->prev->next = item->next;\
    if(item->next != NULL) item->next->prev = item->prev;\
    if(list == item) list = item->next \
    list->prev = item->next = NULL; \
}while(0);

//一個nworker節點代表一個工作線程
typedef struct NWORKER{
    nThreadPool *pool;
    pthread_t thread_id;
    //到底線程要不要釋放
    int terminate;
    //工作中還是空閒中
    int flag;
    //透過雙向鏈錶把所有工作線程連在一起，如果你用C++寫可以用Queue
    struct NWORKER *next;
    struct NWORKER *prev;
}nWorker;

typedef struct NJOB{
    // 開啟線程的callback function，pthread 默認為返回空值
    // C++ 直接使用 functor和 std::thread
    void (*func)(void *arg);
    void *user_data;

    struct NJOB *prev;
    struct NJOB *next;

}nJob;

//管理回調函數和線程，和互斥
typedef struct NMANAGER{
    nWorker *workers;
    nJob *jobs;
    //有多少線程工作中
    int sum_thread;
    int free_thread;
    
    pthread_mutex_t *jobs_mtx;
    pthread_cond_t *jobs_cond;
}nManager;

typedef nManager nThreadPool;


//消費者，這裡會不斷輪詢任務隊列，一有任務就拿來做
void nWorkerCallback(void * arg){
    nWorker *worker = (nWorker*)arg;
    while(1){
        pthread_mutex_lock(&worker->pool->jobs_mtx);
        while(worker->pool->jobs == NULL){
            if(worker->terminate == 1)break;
            pthread_cond_wait(&worker->pool->jobs_cond, &worker->pool->jobs_mtx);
        }
        if(worker->terminate == 1){
            pthread_mutex_unlock(&worker->pool->jobs_mtx);
            break;
        }

        nJob *job = worker->pool->jobs;
        LL_REMOVE(job, job);

        pthread_mutex_unlock(&worker->pool->jobs_mtx);
        
        job->func(job->user_data);
        free(job);

    }
}
int nThreadPoolCreate(nThreadPool *pool, int numWorkers){
    if(pool == NULL)return -1;
    if(numWorkers < 1)numWorkers = 1;
    memset(pool, 0, sizeof(nThreadPool));

    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&pool->jobs_mtx, &blank_mutex, sizeof(pthread_mutex_t));

    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    memcpy(&pool->jobs_cond, &blank_cond, sizeof(pthread_cond_t));

    int i = 0;
    for(i = 0; i < numWorkers; ++i){
        //不要把C style的cast 拿去C++用，操你媽的 undefined behavior
        nWorker *worker = (nWorker*)malloc(sizeof(nWorker));
        if(worker == NULL){
            perror("malloc");
            return 1;
        }
        memset(worker, 0, sizeof(nWorker));

        int ret = pthread_create(worker->thread_id, NULL, nWorkerCallback, worker);
        if(ret){
            perror("pthread_create");
            nWorker *w = pool->workers;
            for(w = pool->workers; w != NULL; w=w->next){
                w->terminate = 1;
            }
            return -1;
        }
        LL_ADD(worker, pool->workers);
    }
    return 0;
}


int nThreadPoolPushJob(nThreadPool *pool, nJob* job){
    pthread_mutex_lock(&pool->jobs_mtx);
    LL_ADD(job, pool->jobs);

    pthread_cond_signal(&pool->jobs_cond);
    pthread_mutex_unlock(&pool->jobs_mtx);
};