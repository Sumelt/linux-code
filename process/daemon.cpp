/*
 * @Description: 
 * @Author: melt
 * @Date: 2020-05-30 12:40:59
 * @LastEditTime: 2020-05-30 17:33:41
 */

#include<iostream>

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void daemon_fork_creat() {
    pid_t pid;
    int fd;

    if( ( pid = fork() ) < 0 ) {
        std::cerr << "error fork" << std::endl;
        exit( 1 );
    }
    else if( pid != 0 ) {
        exit( 0 ); //父进程退出
    }
    
    //更新会话ID
    setsid();

    //修改工作目录
    chdir( "/" );
    
    //重定向三个标准的IO流
    fd = open( "/dev/null", O_RDWR );
    dup2( fd, STDIN_FILENO );
    dup2( fd, STDOUT_FILENO );
    dup2( fd, STDERR_FILENO );
    
    //设置掩码
    umask( 0 );    
}

void daemon_api_creat() {

    daemon( 0, 0 );
}

int main( int argc, char* argv[] ) {

    daemon_api_creat();
    while ( true )
    {
        sleep( 1 );
    }
    
    return 0;
}