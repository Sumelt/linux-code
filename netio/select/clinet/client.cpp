#include "cshead.h"
#define MAXSIZE 4096

void clientCore( void *value )
{

    int sockfd;
    char sendLine[ MAXSIZE ];
    char revBuff[ MAXSIZE ];
    char serIp[] = "127.0.0.1";//服务器IP地址
    int ret;
    struct sockaddr_in seraddr;//套接字结构体

    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 ) {
        cout << "socket creath faile from client" << endl;
        exit( 0 );
    }

    memset( &seraddr, 0, sizeof ( seraddr ) );
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons( 6666 );//服务器端口号
    ret =  inet_pton( AF_INET, serIp, &seraddr.sin_addr );//点十分到二进制转换
    if( ret <= 0 ) {
        cout << "inet_pton failefrom client" << endl;
        exit( 1 );
    }

    ret = connect( sockfd, ( struct sockaddr* )&seraddr, sizeof ( seraddr ) );//套接字连接，需要进行结构体强制转换
    if( ret < 0 ) {
        cout << "connect faile from server" << endl;
        exit( 0 );
    }
    else
        cout << "-------Successfully connected to the server-------" << endl;

    recv( sockfd,&revBuff, sizeof ( revBuff ), 0 );
    cout << revBuff << endl;
    send( sockfd, "Hello server!", strlen( "Hello server!" ), 0 );

    cout << "-------please input string send to server-------" << endl;
    while ( true ) {
        fgets( sendLine, MAXSIZE, stdin );//获取标准输入
        if( strcmp( sendLine, "end\n\0" ) == 0 )
            break;
        if( send( sockfd, sendLine, strlen( sendLine ), 0 ) < 0 ) {//向套接字发送数据
            cout << "send faile from client" << endl;
            exit( 0 );
        }
        memset( sendLine, 0, sizeof ( sendLine ) );

    }
    close( sockfd );//关闭套接字
}
