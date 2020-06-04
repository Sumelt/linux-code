/*
 *
 *模拟出租车 条件变量
 *
 */
#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <error.h>
using namespace std;

static pthread_cond_t taxiCond = PTHREAD_COND_INITIALIZER; //初始化条件变量
static pthread_mutex_t taxiMutex = PTHREAD_MUTEX_INITIALIZER; //初始化互斥锁
int travelerCound = 0; //乘客的数量

void *traveler_arrive( void *name )
{
    cout << "traveler name: " << static_cast<char*>(name) << " need a taxi"<< endl;
    pthread_mutex_lock( &taxiMutex ); //上锁
    ++travelerCound; //上锁后再增加一位乘客
    pthread_cond_wait( &taxiCond, &taxiMutex ); //等待条件变量改变，调用时候必须本线程手动上锁，在线程挂起阻塞时解锁，当收到激活信号时 //这里的条件变量是要一辆出租车
    //重新上锁，执行想要的操作，最后手动解锁
    cout << "traveler name: " << static_cast<char*>(name) << " go to a taxi"<< endl;
    pthread_mutex_unlock( &taxiMutex );//解锁

    pthread_exit( nullptr );
}

void *taxi_arrive( void *name )
{
    cout << "taxi name: " << static_cast<char*>(name) << " need a traveler"<< endl;
    while( true ){
        pthread_mutex_lock( &taxiMutex );
        if( travelerCound > 0 ) {
            pthread_cond_signal( &taxiCond ); //发生激活信号给等待次信号变量而阻塞的线程
            pthread_mutex_unlock( &taxiMutex ); //解锁再退出
            break;
        }
        pthread_mutex_unlock( &taxiMutex );
    }
    cout << "a car end" << endl;
    pthread_exit( nullptr );
}

int main()
{
    int Ret;
    const int pthreadCount = 3;
    pthread_t tid[ pthreadCount ];

    for ( int i = 0; i < pthreadCount; ++i ) {
        Ret = pthread_create( &tid[ i++ ], nullptr, taxi_arrive, ( void* )"BMW" );
        sleep( 1 );
        Ret = pthread_create( &tid[ i++ ], nullptr, traveler_arrive, ( void* )"Melt" );
        sleep( 3 );
        Ret = pthread_create( &tid[ i++ ], nullptr, taxi_arrive, ( void* )"QQ" );
    }
    sleep( 1 );
    void *RetValue;
    for ( int i = 0; i < pthreadCount; ++i ) {
        pthread_join( tid[ i ], &RetValue );
        cout << "pthread return value: " << RetValue << endl;
    }
    pthread_cond_destroy( &taxiCond );
    pthread_mutex_destroy( &taxiMutex );

    return 0;
}
