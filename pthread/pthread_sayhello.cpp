/**
 * @author Melt
 * @email [example@mail.com]
 * @create date 2019-03-03 16:09:58
 * @modify date 2019-03-03 16:09:58
 * @desc [description] use pthread say hello
 */

#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>
using namespace std;

struct sayhello_arg //结构体
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
    ~sayhello(){};
};


void *sayhello::say_hello( void *value ) //类外静态函数定义
{
    struct sayhello_arg *arg = ( struct sayhello_arg * )value; //结构体参数强制转换
    cout << pthread_self() << endl; //返回自身线程的id
    cout << "say_hello funtion:" << ' ' << arg->number << ' ' << arg->str << endl;
    pthread_exit( ( void * )2 ); //子线程的退出返回值
}

int main()
{
    pthread_t tid;
    
    sayhello_arg arg( "Hello word", 425 ); //结构体参数
    int iRet = pthread_create( &tid, NULL, sayhello::say_hello, &arg ); //创建线程

    if( iRet ) {
        printf(" creathe pthread error\n ");
        return iRet;
    }
    cout << tid << endl; //创建线程后的id
    void *retval; //获取子线程的返回值
    iRet = pthread_join( tid, &retval ); //等待子线程退出

    if ( iRet ) {
        /* code */
        printf("pthread_join error\n");
        return iRet;
    }
    printf("retval = %ld\n", ( long )retval ); //输出子线程的退出值
    return 0;
}


