/*----------------------------------------------------------------

* @Author: Su

* @Description:

* @Creath Date: 2019-05-28-11.10.34

----------------------------------------------------------------*/

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>       //select
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
using namespace std;

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, void *buf, size_t count);


//解决方案一：使用定长数据

//读取固定长度的数据
ssize_t readn( int fd, void *buf, size_t count ) {
    size_t length = count;//需要读的数据长度
    size_t readByte;
    size_t accumulative = 0;
    char *ptr = ( char* )buf; //缓冲区

    while( length > 0 ) {
        readByte = read( fd, ptr, count );
        if( readByte == -1 && errno == EINTR ) //可能被信号中断
            continue;//重启
        else if( readByte == 0 ) //文件结束
            return accumulative; //返回已经读取的长度
        else if( readByte > 0 ) {
            accumulative += readByte;
            ptr += readByte;
            length -= readByte;
        }
        else return -1; //出错
    }
    return count;
}

//写入固定长度
ssize_t writen(int fd, void *buf, size_t count) {
    size_t length = count;//需要读的数据长度
    size_t writeByte;
    size_t accumulative = 0;
    char *ptr = ( char* )buf; //缓冲区

    while( length > 0 ) {
        writeByte = write( fd, ptr, count );
        if( writeByte == -1 && errno == EINTR ) //可能被信号中断
            continue;//重启
        else if( writeByte == 0 ) //文件结束
            return accumulative; //返回已经写入的长度
        else if( writeByte > 0 ) {
            accumulative += writeByte;
            ptr += writeByte;
            length -= writeByte;
        }
        else return -1; //出错
    }
    return count;
}


class Server{

    friend void *heartHeadle( void* );
public:
    Server( int port );
    ~Server();
    void CreatSock();
    void Bind( int port );
    void Listen();
    void Accept();
    void Recv();
    void Run();

private:
    struct sockaddr_in servaddr;
    struct timeval timeout;//select的超时时间
    socklen_t servlen;
    int sockfd;
};

Server::Server( int port )
{
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;

    Bind( port );
}

Server::~Server()
{
    close( sockfd );
}

void Server::Bind( int port )
{
    int ret;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( port );
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );

    ret = bind( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 ) {
        perror( "bind faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "bing Ok" << endl;

    Listen();
}

void Server::Listen()
{
    int ret = listen( sockfd, 10 );
    if( ret != 0 ) {
        perror( "listen faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "listen Ok" << endl;

}

void Server::Accept()
{
    size_t count = 10; //读10个字节
    char buff[ count ] = { 0 };
    int connfd = accept( sockfd, nullptr, nullptr );
    cout << "accpet new connection" << endl;
    if( connfd >= 0 ) {
            size_t byte = readn( connfd, buff, count );
        if( byte == count )
            cout << "Complete reception: " << count << endl;
        else cout << "Lose data and now recvice: " << byte << endl;
        buff[ byte ] = '\0';
        cout << "Data: " << buff << endl;
    }
    else cout << "read data error" << endl;
}


void Server::Run()
{
    cout << "wait client connect" << endl;
    while( true ) {
        Accept();
    }
}

int main(int argc, char *argv[])
{
    Server serv( 6666 );
    serv.Run();
    return 0;
}
