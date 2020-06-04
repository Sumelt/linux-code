/*
 *服务器端
 */

#include "Chead.h"

const int MAXSIZE = 4096;
const int ClientPort = 6666;
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

//线程处理函数参数
struct threadarg {
    struct sockaddr_in* addr;
    package &recvPackage;
    package &sendPackage;
    int sockfd;
    socklen_t len;

    threadarg( struct sockaddr_in* ad, int fd, socklen_t l,
              struct package &recv, struct package &send )
                : addr( ad ), sockfd( fd ), len( l ),
                    recvPackage( recv ), sendPackage( send ) {}
};

void *listenThread( void *argThread )
{
    int recvByte;
    char clientIP[ 32 ];
    struct threadarg* arg = static_cast< struct threadarg* > ( argThread );

    while( true ) {
        recvByte = recvfrom( arg->sockfd, arg->recvPackage.mess, MAXSIZE, 0,
                            ( struct sockaddr* )arg->addr, &arg->len );
        inet_ntop( AF_INET, arg->addr, clientIP, sizeof( clientIP ));

        if( recvByte <= 0 )
            cout << "recv error" << endl;
        else {
            //包头验证
            if( strncmp( arg->recvPackage.mess, head, strlen( head ) ) == 0 ) {
                arg->recvPackage.length = strlen( arg->recvPackage.mess );
                cout << "Receive: " << arg->recvPackage.mess + strlen( head )
                    << " Byte: " << arg->recvPackage.length << ' '
                    << "From IP: " << clientIP << ' '
                    << "Port: " << ntohs( arg->addr->sin_port ) << endl;

                sendto( arg->sockfd, arg->sendPackage.mess,
                            arg->sendPackage.length, 0,
                                ( struct sockaddr* )arg->addr, arg->len );
            }
            else cout << "error package" << endl;
        }
    }
    pthread_exit( nullptr );
}


void provide( struct sockaddr_in *cliaddr, int sockfd )
{
    cout << "------server recvive package---------" << endl;
    int ret;
    pthread_t tpid;
    package sendPackage( "Hi!" );
    package recvPackage;
    socklen_t servlen = sizeof( *cliaddr );

    //线程参数
    threadarg arg( cliaddr, sockfd, servlen, recvPackage, sendPackage );
    //创建线程
    ret = pthread_create( &tpid, nullptr, listenThread, &arg );
    if( ret != 0 )
        cout << "pthread creath error" << endl;

    ret = pthread_join( tpid, nullptr );
    if( ret != 0 )
        cout << "pthread creath join" << endl;
}

void init()
{
    int sockfd, ret;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof( servaddr );

    memset( &servaddr, 0, sizeof( servaddr ) );
    memset( &cliaddr, 0, sizeof( cliaddr ));

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( ClientPort );
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡

    ret = bind( sockfd, ( struct sockaddr* )&servaddr, clilen );
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }

    provide( &cliaddr, sockfd );
}

void client()
{
    init();
}
