/*
 *
 *模拟火车售票系统 互斥锁
 *
 */

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
using namespace std;

static int total_ticket_num = 20; //火车总票数
static pthread_mutex_t mutex; //互斥锁全局变量
void *sell_ticket(void *arg){  //线程循环函数
    for(int i=0; i<20; i++){
        pthread_mutex_lock( &mutex );
        if( total_ticket_num > 0 ){
            sleep(1);
            printf("sell the %dth ticket by %ld\n", 20-total_ticket_num+1, pthread_self() ); //成功卖出一张票
            total_ticket_num--; //票数减一
        }
        //cout << "after sale funtion ID: " << pthread_self() << endl;
        pthread_mutex_unlock( &mutex );
    }
    cout << "pthread end ID: " << pthread_self() << endl; //线程结束输出
    return nullptr;
}

void *sell_ticket2( void *arg )
{
    for ( int i = 0; i < 10 ; ++i ) {
        if( pthread_mutex_trylock( &mutex ) == EBUSY ) //尝试获得互斥锁
            cout << "mutex using by other pthread\n";
        else {
            if( total_ticket_num > 0 ){
                sleep(1);
                printf("sell the %dth ticket by %ld\n", 20-total_ticket_num+1, pthread_self() ); //成功卖出一张票
                total_ticket_num--; //票数减一
            }
        }
         pthread_mutex_unlock( &mutex );
    }
    cout << "pthread end ID: " << pthread_self() << endl; //线程结束输出
    return nullptr;
}

int main(){
    int iRet;
    int i=0;
    const int pthreadCount = 2; //线程个数
    pthread_t tids[ pthreadCount ]; //创建2个线程
    pthread_mutex_init( &mutex, nullptr ); //互斥锁初始化

    for( i=0; i<pthreadCount;  ){
        iRet = pthread_create(&tids[ i++ ], nullptr, &sell_ticket, nullptr ); //执行函数1
        if(iRet){
            printf("pthread_create error, iRet=%d\n",iRet);
            return iRet;
        }

        iRet = pthread_create(&tids[ i++ ], nullptr, &sell_ticket2, nullptr ); //执行函数2
        if(iRet){
            printf("pthread_create error, iRet=%d\n",iRet);
            return iRet;
        }
    }

    sleep(10); //主线程休眠20秒
    void *retval;
    for( i=0; i<pthreadCount; i++ ){
        iRet=pthread_join(tids[i], &retval ); //等待子线程
        if(iRet){
            printf("tid=%d join error, iRet=%d\n", tids[i], iRet );
            return iRet;
        }
        printf("retval=%ld\n", (long*)retval );
    }
    cout << "ticket Remaining: " << total_ticket_num << endl;
    pthread_mutex_destroy( &mutex );  //销毁互斥锁

    return 0;
}
