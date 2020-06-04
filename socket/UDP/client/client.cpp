#include "cshead.h"

const int mx = 4096;

void echo( struct sockaddr_in *servaddr, int sockfd  )
{
    char recvBuff[ mx ] = { 0 };
    char sendBuff[ mx ] = { 0 };
    ssize_t recvByteSize = 0;
    socklen_t servlen = sizeof( *servaddr );
    socklen_t perlen;
    struct sockaddr_in peraddr; //暂存的源地址

    memset( &peraddr, 0, sizeof( peraddr ) );
    perlen = sizeof( peraddr );

    //connect( sockfd, ( struct sockaddr *)servaddr, servlen );
    if( connect( sockfd, ( struct sockaddr *)servaddr, servlen ) < 0 ) {
        cout << "connec faile" << endl;
        exit( 0 );
    }

    cout << "-------client begin-------" << endl;

    while( true ) {
        fgets( sendBuff, mx, stdin );//获取标准输入
        if( strcmp( sendBuff, "end\n\0" ) == 0 )
            break;
        //sendto( sockfd, sendBuff, strlen( sendBuff ), 0, ( sockaddr* )servaddr, servlen );
        //recvByteSize = recvfrom( sockfd, recvBuff, mx, 0, ( sockaddr* )&peraddr, &perlen );
        write( sockfd, sendBuff, sizeof( sendBuff ) );
        recvByteSize = read( sockfd, recvBuff, sizeof( recvBuff ) );
        if( recvByteSize > 0 ) {
            recvBuff[ recvByteSize ] = 0;
            cout << "echo message from server: " << recvBuff << endl;
        }

        memset( &sendBuff, 0, sizeof ( sendBuff ) );
        memset( &recvBuff, 0, sizeof( recvBuff ) );
    }
}

void init()
{
    struct sockaddr_in servaddr;
    char serIp[] = "127.0.0.1";//服务器IP地址
    int ret, sockfd;

    memset( &servaddr, 0, sizeof( servaddr ) );

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( 5555 );
    ret =  inet_pton( AF_INET, serIp, &servaddr.sin_addr );//点十分到二进制转换
    if( ret <= 0 ) {
        cout << "inet_pton failefrom client" << endl;
        exit( 1 );
    }

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    echo( &servaddr, sockfd );
}

void client()
{
    init();
}
