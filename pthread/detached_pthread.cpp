/**
 * @author Melt
 * @email [example@mail.com]
 * @create date 2019-03-05 17:25:14
 * @modify date 2019-03-05 17:25:14
 * @desc [description]
 */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <pthread.h>

void * tfn1(void * arg){
    printf("the thread\n");
    return NULL;
}

int main(void){
    int iRet;
    pthread_t tid;
    pthread_attr_t attr; //线程参数属性

    iRet = pthread_attr_init(&attr); //初始化线程属性
    if(iRet){
        printf("can't init attr %s/n", strerror(iRet));
        return iRet;
    }

    iRet = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);// 设置线程分离
    if(iRet){
        printf("can't set attr %s\n", strerror(iRet));
     return iRet;
    }

    iRet = pthread_create(&tid, &attr, tfn1, NULL);//创建线程同时传入线程属性
    if(iRet){
        printf("can't create thread %s\n", strerror(iRet));
        return iRet;
    }

    iRet = pthread_join(tid, NULL); //等待子线程 由于设置线程分离，等待会失败
    if(iRet){
        printf("thread has been detached\n");
        return iRet;
    }

    return 0;
}
