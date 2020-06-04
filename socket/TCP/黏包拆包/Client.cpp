/*----------------------------------------------------------------

* @Author: Su

* @Description:

* @Creath Date: 2019-05-28-13.19.15

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

//解决方案二：结构体，包头和包尾
struct package{
    size_t length;//包头占4字节，表明包体的长度大小
    char buff[ 512 ];//包体
    package( size_t len ) : length( len ) {}
};


//解决方案三：数据中加入特殊字符

class Client{

    friend void *heartHeadle( void* );
public:
    Client( int port );
    ~Client();
    void Accept();
    void Recv();
    void Run();
    void Send();

private:
    struct sockaddr_in servaddr;
    struct timeval timeout;//select的超时时间
    socklen_t servlen;
    int sockfd;
    int servPort;
};

Client::Client( int port ) : servPort( port )
{
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );
    servaddr.sin_port = htons( servPort );
    servaddr.sin_family = AF_INET;

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;
}

Client::~Client()
{
    close( sockfd );
}


void Client::Accept()
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

void Client::Send()
{
    int count = 20;//发出20字节的数据，模拟黏包现象
    char buff[ count ] = "helloworldworldhello";
    int byte = writen( sockfd, buff, count );
    if( byte >= 0 ) {
        if( byte == count )
            cout << "Complete send: " << byte << endl;
        else cout << "Lose data and now send: " << byte << endl;
    }
    else cout << "send error" << endl;
}


void Client::Run()
{
    cout << "Connect server" << endl;
    int ret = connect( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 )
        exit( EXIT_FAILURE );
    else cout << "connection OK" << endl;
    Send();
}


int main(int argc, char *argv[])
{
    Client cli( 6666 );
    cli.Run();
    return 0;
}
