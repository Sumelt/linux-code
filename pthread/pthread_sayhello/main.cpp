/**
 * @author Melt
 * @email [example@mail.com]
 * @create date 2019-03-03 16:09:58
 * @modify date 2019-03-03 16:09:58
 * @desc [description] use pthread say hello
 *演示线程创建和线程等待
 */

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
using namespace std;

struct sayhello_arg //线程调用函数结构体参数
{
    /* data */
    string str;
    int number;
    sayhello_arg( string valueStr, int num ) : str( valueStr ), number( num ){}
};

class sayhello //定义的类
{
private:
    /* data */
public:
    static void *say_hello( void *value ); // 静态函数
    sayhello(/* args */) = default;
    ~sayhello(){}
};

void *sayhello::say_hello( void *value ) //类外静态函数定义
{
    struct sayhello_arg *arg = static_cast<struct sayhello_arg *>(value); //结构体参数强制转换
    //cout << pthread_self() << endl; //返回自身线程的id
    cout << "say_hello funtion: " << ' ' << arg->number << ' ' << arg->str << endl;
    pthread_exit( ( void* )2 ); //子线程的退出返回值
}

int main()
{
    pthread_t tid; //线程ID变量
    pthread_attr_t attr; //线程的参数变量
    int iRet;  //获取返回值
    sayhello_arg arg( "Hello word", 425 ); //结构体参数变量

//    iRet = pthread_attr_init(&attr); //初始化线程参数
//    if( iRet ) {
//        printf(" pthread_attr_init error\n ");
//        return iRet;
//    }

//    iRet = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置线程分离属性
//    if( iRet ) {
//        printf(" pthread_attr_setdetachstat error\n ");
//        return iRet;
//    }

    iRet = pthread_create( &tid, nullptr, sayhello::say_hello, &arg ); //创建带着属性的线程
    if( iRet ) {
        printf(" creathe pthread error\n ");
        return iRet;
    }

    iRet = pthread_detach(tid); //线程创建后进行状态分离
    if( iRet ) {
        printf(" pthread_detach error\n ");
        return iRet;
    }
    sleep( 1 ); //主线程休眠1秒
    cout << "子线程ID: " << tid << endl; //创建线程后的id

    void *retval; //获取子线程的返回值
    iRet = pthread_join( tid, &retval ); //等待子线程退出
    if ( iRet ) {
        printf("pthread_join error or pthread detached\n");
        return iRet;
    }
    printf("retval = %ld\n", static_cast<long*>(retval)  ); //输出子线程的退出值
    return 0;
}

