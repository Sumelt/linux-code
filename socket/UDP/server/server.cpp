#include "cshead.h"

const int mx = 4096;

void echo( struct sockaddr_in *cliaddr, int sockfd )
{
    socklen_t clilen;
    char recvBuff[ mx ] = { 0 };
    int recvByteSize = 0;

    clilen = sizeof( *cliaddr );
    cout << "------serve waiting client connect---------" << endl;

    while( true ) {
       recvByteSize = recvfrom( sockfd, recvBuff, mx,
                    0, ( sockaddr* )cliaddr, &clilen );
        recvBuff[ recvByteSize ] = 0;
        if( recvByteSize > 0 ) {
            cout << "recv message and echo" << endl;
            sendto( sockfd, recvBuff, recvByteSize,
                    0, ( sockaddr* )cliaddr, clilen );
        }
        //else cout << "maybe client close" << endl;
    }
}

void init()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int ret;

    memset( &servaddr, 0, sizeof ( servaddr ) );//初始化套接字结构体
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );//创建套接字：IPV4协议 UDP套接字 自动选择协议
    servaddr.sin_family = AF_INET;//IPV4协议
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡
    servaddr.sin_port = htons( 5555 );//服务器端口号

    ret =  bind( sockfd, ( struct sockaddr* )&servaddr, sizeof( servaddr ) );
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }
    echo( &cliaddr, sockfd );
}

void server()
{
    init();
}



