#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

static int globalValue = 2;
int main()
{

    pid_t kidPid;

    kidPid = fork(); //创建子进程
    if( kidPid < 0 ) {
        cout << "creath kid progress faile" << endl;
        exit( 0 );
    }
    if( kidPid > 0 ) {
        sleep( 1 );
        int status;//获取子进程的退出状态
        cout << "father progress start" << endl;
        cout << "globalValue: " << globalValue << endl;
        //wait( &status );
        waitpid( kidPid, &status, 0 );//等待子进程并获取退出状态
        if( WIFEXITED( status ) ){//该宏判断子进程是否正常退出
            cout << "kid progress exit status:"
                 << WEXITSTATUS( status ) << endl;//正常退出提取子进程的退出状态
        }
    }
    if( kidPid == 0 ) {
        cout << "kid progress start" << endl;
        ++globalValue;//修改子进程自己空间的全局变量不影响父进程空间
        exit( 1 );//正常退出
    }

    return 0;
}
