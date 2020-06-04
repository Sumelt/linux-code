/*
 * 模拟窗口服务 信号量
 *
 */

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define CUSTOMER_NUM 10 //顾客人数

static sem_t sem; //信号量
void * get_service(void *thread_id){

   int customer_id = *((int *)thread_id); //保存顾客ID

    if(sem_wait(&sem) == 0) {  //信号非0则可以进入临界区，多个线程可以进入
        usleep(100); /* service time: 100ms */
        printf("customer %d receive service ...\n", customer_id);
        printf("customer %d leaving service ...\n", customer_id);
        sem_post(&sem); //信号量加1
    }
}

int main(int argc, char *argv[]){
    int i, iRet;

    sem_init(&sem, 0, 2); //初始化信号量，0表示当前进程的局部信号，初值为2表示最多接受两名顾客服务
    pthread_t customers[CUSTOMER_NUM]; //


    for(i = 0; i < CUSTOMER_NUM; i++){
        int customer_id = i; //顾客的ID
        iRet = pthread_create(&customers[i], nullptr, get_service, &customer_id); //创建顾客线程

        if(iRet){
            perror("pthread_create");
            return iRet;
        }
        else{
            printf("Customer %d arrived.\n", i);
        }

        usleep(10);
    }

    int j;
    for(j = 0; j < CUSTOMER_NUM; j++) {
            pthread_join(customers[j], nullptr);
        }

    sem_destroy(&sem);

    return 0;
}
