#include "cshead.h"

#define MaxLINE 4096
#define MaxBuff 4096
const int maxConNumber = 5;

void serve( void )
{
    int serListenfd, clientSockfd;//监听套接字 连接套接字
    int ret;//返回值检测
    char revBuff[ MaxBuff ];//缓冲区
    struct sockaddr_in servaddr;//服务器套接字结构体
    struct timeval timeout;	//超时计时器
    fd_set client_fdset;//客户机集合
    int maxSocketNumber;
    int client_sockArrayfd[ maxConNumber ];   /*存放活动的客户机sockfd*/
    int conn_amount = 0;    /*用来记录连接描述符数量*/
    ssize_t byteSize;//接收的字节数

    memset( revBuff, 0, sizeof ( revBuff ) );
    memset( client_sockArrayfd, 0, sizeof ( client_sockArrayfd ) );
    memset( &servaddr, 0, sizeof ( servaddr ) );//初始化套接字结构体

    /**服务器初始化**/
    serListenfd = socket( AF_INET, SOCK_STREAM, 0 );//创建套接字：IPV4协议 TCP套接字 自动选择协议
    if( serListenfd == -1 ){
        cout << "creath socket faile from server" << endl;
        exit( 0 );
    }
    maxSocketNumber = serListenfd;//select中要求的最大fd


    servaddr.sin_family = AF_INET;//IPV4协议
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡
    servaddr.sin_port = htons( 6666 );//服务器端口号

    ret =  bind( serListenfd, ( struct sockaddr* )&servaddr, sizeof ( servaddr ) );//将结构体信息绑定到该套接字上
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }

    ret = listen( serListenfd, 10 );//创建监听套接字 监听指定端口的请求，10个请求队列
    if( ret == -1 ){
        cout << "listen socket faile from server" << endl;
        exit( 0 );
    }

    /**服务器初始化完成等待客户机连接**/
    cout << "------serve waiting client connect---------" << endl;
    struct sockaddr_in cliaddr;//客户机信息结构体
    socklen_t len = sizeof ( cliaddr );
    //服务器循环
    while ( true ) {

        timeout.tv_sec = 10;//等待10秒
        timeout.tv_usec = 0;
        FD_ZERO( &client_fdset );
        FD_SET( serListenfd, &client_fdset );//加入服务器监听加入到集合中

        //套接字已经和服务器建立通讯后，加入集合
        if( conn_amount > 0 ) {
            for ( int i = 0; i < maxConNumber; ++i ) {
                if( client_sockArrayfd[ i ] != 0 )
                    FD_SET( client_sockArrayfd[ i ], &client_fdset );
            }
        }
        //只关心读动作
        ret = select( maxSocketNumber + 1, &client_fdset, nullptr, nullptr, &timeout );
        if( ret == 0 ) {
            cout << "time out" << endl;
            continue;
        }
        else if ( ret < 0 ) {
            cout << "select error" << endl;
            break;
        }

        /*轮询各个文件描述符*/
        for( int i = 0; i < conn_amount; ++i ){
            if( FD_ISSET( client_sockArrayfd[ i ], &client_fdset ) ){
                printf("start recv from client[ %d ]:\n",i);
                byteSize = recv( client_sockArrayfd[ i ], revBuff, MaxLINE, 0 );

                if( byteSize <= 0 ){
                    cout << "client" << "[ " << i << " ]: " << "close" << endl;
                    close( client_sockArrayfd[ i ] );
                    client_sockArrayfd[ i ] = 0;
                    FD_CLR( client_sockArrayfd[ i ], &client_fdset);//从集合中删除
                    --conn_amount;
                }
                else
                    cout << "recv message from client" << "[ " << i << " ]: "  << revBuff << endl;
                 memset( revBuff, 0, sizeof ( revBuff ) );//清空缓冲区
            }
        }

        //发生客户机请求事件
        if( FD_ISSET( serListenfd, &client_fdset ) ) {
            clientSockfd = accept( serListenfd, ( struct sockaddr *)&cliaddr, &len );//等待客户端请求，连接成功创建连接套接字
            if( clientSockfd == -1 ){
                cout << "accept faile from server" << endl;
                continue;
            }
            //和客户机进行交互
            if( conn_amount < maxConNumber ) {

                if( clientSockfd > maxSocketNumber )
                        maxSocketNumber = clientSockfd; //更新最大值

                //构建发送给客户机消息
                char clientIP[ MaxBuff ];
                time_t rowTimeNumber = time( nullptr );

                inet_ntop( AF_INET, &cliaddr.sin_addr, clientIP, sizeof ( clientIP ) );
                string sendMessage = "welcome connect server, your IP is ";
                sendMessage += string( begin( clientIP ), end( clientIP ) );//IP
                sendMessage += ctime( &rowTimeNumber );//time
                sendMessage += '\0';
                cout << sendMessage<< endl;
                send( clientSockfd, sendMessage.c_str(), sendMessage.size(), 0 );

                //显示连接的客户机信息
                printf( "connect from Client[ %d ]", conn_amount );
                cout <<" IP: " << clientIP << " Port: " << ntohs( cliaddr.sin_port ) << endl;

                //读取客户机套接字信息
                byteSize = recv( clientSockfd, revBuff, MaxLINE, 0 );//利用连接套接字，读取缓冲区的字节数，这里会自动添加一个换行
                if( byteSize > 0  ) {

                    string receStr( revBuff, ( unsigned long )byteSize - 1 );

                    cout << "recv message from client" << "[ " << conn_amount << " ]: "  << revBuff;
                    client_sockArrayfd[ conn_amount++ ] = clientSockfd;//保存活动的客户机套接字
                    cout << endl;
                    memset( revBuff, 0, sizeof ( revBuff ) );
                }
                else
                    cout << "recv client" << "[ " << conn_amount << " ] " << "error" << endl;
            }
            //超过连接的阈值
            else {
                cout << "max connect size and server quit" << endl;
                break;
            }
        }
    }
    //关闭所有连接
    for(int i = 0; i < maxConNumber; ++i){
        if( client_sockArrayfd[ i ] != 0 ){
            close( client_sockArrayfd[ i ] );
        }
    }
    close( serListenfd );//关闭监听套接字
}
