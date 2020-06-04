/*
 *
 * 读写锁
 *
 */

#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#define THREADNUM 5

static pthread_rwlock_t rwlock;
void *readers(void *arg){  //读者函数

    pthread_rwlock_rdlock(&rwlock); //获取读锁
    printf("reader %ld got the lock\n", (long)arg);
    pthread_rwlock_unlock(&rwlock); //解锁

    pthread_exit( nullptr );

}

void *writers(void *arg){ //写者函数

    pthread_rwlock_wrlock(&rwlock); //获得写锁
    printf("writer %ld got the lock\n", (long)arg);
    pthread_rwlock_unlock(&rwlock);

    pthread_exit((void*)0);
}

int main(int argc, char **argv){
    int iRet, i;
    pthread_t writer_id, reader_id; //写者线程 读者线程
    pthread_attr_t attr; //线程属性

    int nreadercount = 1, nwritercount = 1; //读者个数 写者个数

    iRet = pthread_rwlock_init(&rwlock, nullptr ); //动态初始化读写锁
    if (iRet) {
        fprintf(stderr, "init lock failed\n");
        return iRet;
    }

    pthread_attr_init(&attr); //线程属性初始化
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置线程分离

    for (i = 0; i < THREADNUM; i++){
        //创建三个读者线程
        if (i % 3) {
            pthread_create(&reader_id, &attr, readers, (void *)nreadercount);
            printf("create reader %d\n", nreadercount++);
        }
        //创建俩个写者线程
        else {
            pthread_create(&writer_id, &attr, writers, (void *)nwritercount);
            printf("create writer %d\n", nwritercount++);
        }
    }
    sleep(5); /*sleep是为了等待另外的线程的执行*/

    return 0;
}
