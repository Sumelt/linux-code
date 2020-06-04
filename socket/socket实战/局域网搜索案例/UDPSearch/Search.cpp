/*
 *服务器搜索功能实现
 */

#include "Shead.h"
const int MAXSIZE = 4096;
const int ServPort = 5555;//服务器端口
const int BroadcastPort = 6666;//广播端口
char head[ MAXSIZE ] = "Melt";//包头验证字符串

struct package {
    char mess[ MAXSIZE ];
    size_t length;

    package( const char str[]  ) {
        strcpy( mess, str );
        length = strlen( str );
    }

    package() : length( 0 ) {
        mess[ MAXSIZE ] = 0;
    }

};

//客户端先广播发包
void search( struct sockaddr_in *broadaddr, int sockfd )
{
    cout << "------client send broadcast package---------" << endl;
    int recvByte;
    char clientIP[ 32 ];
    struct sockaddr_in cliaddr;
    package sendPackage( strcat( head, "Hello!" ) );
    package recvPackage;
    socklen_t broadlen = sizeof( *broadaddr );
    socklen_t clilen = sizeof( cliaddr );
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    //发送广播

    sendto( sockfd, sendPackage.mess, sendPackage.length, 0,
            ( struct sockaddr* )broadaddr, broadlen);
    //等待回应
    recvByte = recvfrom( sockfd, recvPackage.mess, MAXSIZE, 0,
                ( struct sockaddr* )&cliaddr, &clilen);
    inet_ntop( AF_INET, &cliaddr.sin_addr, clientIP, sizeof ( clientIP ) );

    if( recvByte <= 0 )
        cout << "recv error" << endl;
    else {
            recvPackage.length = strlen( recvPackage.mess );
            cout << "Receive: " << recvPackage.mess
                << " Byte: " << recvPackage.length << ' '
                << "From IP: " << clientIP << ' '
                << "Port: " << ntohs( cliaddr.sin_port ) << endl;
    }
}

void init()
{
    int sockfd;
    struct sockaddr_in servaddr, broadaddr;
    int ret;

    memset( &servaddr, 0, sizeof ( servaddr ) );//初始化套接字结构体
    memset( &broadaddr, 0, sizeof( broadaddr ) );

    //客户端配置
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );//创建套接字：IPV4协议 UDP套接字 自动选择协议
    servaddr.sin_family = AF_INET;//IPV4协议
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡
    servaddr.sin_port = htons( ServPort );//服务器端口号

    //广播配置
    const int on = 1;
    setsockopt( sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof( on ) );//必须设置套接字为广播属性
    broadaddr.sin_port = htons( BroadcastPort );
    broadaddr.sin_family = AF_INET;
    inet_pton( AF_INET, "255.255.255.255", &broadaddr.sin_addr );//发送广播地址

    ret =  bind( sockfd, ( struct sockaddr* )&servaddr, sizeof( servaddr ) );//监听端口
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }
    search( &broadaddr, sockfd );
}


void server()
{
    init();
}

