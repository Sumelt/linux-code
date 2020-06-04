
#include "Shead.h"

const int MAXSIZE = 4096;

void handle( int signo )
{
    pid_t pid = 0;
    int stat;

    while( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 )
        cout << "A child progress exit" << endl;
    return;
}

void echo( struct sockaddr_in *cliaddr, int sockfd )
{
    socklen_t clilen = sizeof( *cliaddr );
    char recvBuff[ MAXSIZE ] = { 0 };
    int recvByteSize = 0;

    cout << "------serve waiting client connect---------" << endl;
    signal( SIGCHLD, handle );

    while( true ) {
        int connfd = accept( sockfd, ( struct sockaddr* )cliaddr, &clilen );
        assert( connfd );
        cout << "New connection" << endl;
        if( fork() == 0 ) {
            close( sockfd );
            recvByteSize = read( connfd, recvBuff, MAXSIZE );
            recvBuff[ recvByteSize ] = 0;
            if( recvByteSize > 0 ) {
                cout << "recv message and echo" << endl;
                send( connfd, recvBuff, MAXSIZE, 0 );
                exit( EXIT_SUCCESS );
            }
            else {
                cout << "client exit" << endl;
                exit( EXIT_FAILURE );
            }
        }
        close( connfd );
    }
}

void init()
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int ret;

    memset( &servaddr, 0, sizeof ( servaddr ) );//初始化套接字结构体
    memset( &cliaddr, 0, sizeof( cliaddr ) );

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );//创建套接字：IPV4协议 TCP套接字 自动选择协议
    servaddr.sin_family = AF_INET;//IPV4协议
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡
    servaddr.sin_port = htons( 5555 );//服务器端口号

    ret =  bind( sockfd, ( struct sockaddr* )&servaddr, sizeof( servaddr ) );
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }

    ret = listen( sockfd, 20 );
    if( ret == -1 ){
        cout << "listen socket faile from server" << endl;
        exit( 0 );
    }
    echo( &cliaddr, sockfd );
}

void server()
{
    init();
}


