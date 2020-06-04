#include "Chead.h"

const int mx = 4096;

int SetNoBlock( int fd )
{
    int oldOpt = fcntl( fd, F_GETFL );
    fcntl( fd, F_SETFL, oldOpt | O_NONBLOCK );
    return oldOpt;
}

int Connect( int &sockfd, struct sockaddr* servaddr, socklen_t servlen, int time )
{
    int oldOpt = SetNoBlock( sockfd );
    int error;

    int ret = connect( sockfd, servaddr, servlen );
    if( ret < 0 && errno != EINPROGRESS ) {
        close( sockfd );
        perror( "connect faile" );
        return -1;
    }
    else if( ret == 0 ) {
        cout << "connection Immediately OK" << endl;
        fcntl( sockfd, F_SETFL, oldOpt );
        return sockfd;
    }

    /*****select option*****/
    fd_set rset, wset;
    struct timeval timeout;

    FD_ZERO( &rset );
    FD_ZERO( &wset );
    FD_SET( sockfd, &rset );
    FD_SET( sockfd, &wset );
    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    ret = select( sockfd + 1, &rset, &wset, nullptr, &timeout );
    if( ret == 0 ) {
        cout << "time out" << endl;
    }
    else if( FD_ISSET( sockfd, &rset ) || FD_ISSET( sockfd, &wset ) ) {
        socklen_t errlen = sizeof( error );

        if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &error, &errlen ) < 0 ) {
            perror( "getsockopt faile" );
            return -1;
        }
        if( error == 0 ) {
            cout << "connect OK" << endl;
            fcntl( sockfd, F_SETFL, oldOpt ); //设置回原来的文件标志状态
            return sockfd;
        }

    }
    close( sockfd );
    return -1;
}


void echo( int sockfd  )
{
    char recvBuff[ mx ] = { 0 };
    char sendBuff[ mx ] = { 0 };
    ssize_t recvByteSize = 0;
    cout << "please input string" << endl;
    while( true ) {
        fgets( sendBuff, mx, stdin );//获取标准输入
        if( strcmp( sendBuff, "end\n\0" ) == 0 )
            break;
        write( sockfd, sendBuff, sizeof( sendBuff ) );
        recvByteSize = read( sockfd, recvBuff, sizeof( recvBuff ) );
        if( recvByteSize > 0 ) {
            recvBuff[ recvByteSize ] = 0;
            cout << "echo message from server: " << recvBuff << endl;

        }
        else cout << "read error and pthread exit" << endl;

        memset( &sendBuff, 0, sizeof ( sendBuff ) );
        memset( &recvBuff, 0, sizeof( recvBuff ) );
        break;
    }
}

void init()
{
    struct sockaddr_in servaddr;
    socklen_t servlen = sizeof( servaddr );
    char servip[] = "127.0.0.1";//服务器IP地址
    int ret, sockfd;

    memset( &servaddr, 0, sizeof( servaddr ) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( 5555 );
    ret =  inet_pton( AF_INET, servip, &servaddr.sin_addr );//点十分到二进制转换
    if( ret <= 0 ) {
        cout << "inet_pton failefrom client" << endl;
        exit( 1 );
    }

    cout << "-------client begin-------" << endl;
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    assert( sockfd );
    Connect( sockfd, ( struct sockaddr* )&servaddr, servlen, 5 );
    if( sockfd > 0 )
        echo( sockfd );
}

void client()
{
    init();
}
