#include "cshead.h"

#define MaxLINE 4096
#define MaxBuff 4096
const int maxConnect = 5;

void serverCore()
{
    int listenfd, connfd;//监听套接字 连接套接字
    int ret;//返回值检测
    int curMaxIndex = 0;  //用来记录连接描述符数量
    char recvBuff[ MaxBuff ];//缓冲区
    struct sockaddr_in servaddr;//服务器套接字结构体
    struct sockaddr_in cliaddr;//客户机信息结构体
    struct timeval timeout;	//超时计时器
    struct pollfd clientArryfd[ maxConnect ]; //poll结构体
    ssize_t recvByteSize;//接收的字节数
    socklen_t clilen = sizeof ( cliaddr );//客户机结构体长度

    memset( recvBuff, 0, sizeof ( recvBuff ) );
    memset( &servaddr, 0, sizeof ( servaddr ) );//初始化套接字结构体
    for( int i = 0; i < maxConnect; ++i )
        clientArryfd[ i ].fd = -1;

    /**服务器初始化**/
    listenfd = socket( AF_INET, SOCK_STREAM, 0 );//创建套接字：IPV4协议 TCP套接字 自动选择协议
    if( listenfd == -1 ){
        cout << "creath socket faile from server" << endl;
        exit( 0 );
    }

    servaddr.sin_family = AF_INET;//IPV4协议
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//多网卡
    servaddr.sin_port = htons( 6666 );//服务器端口号

    ret =  bind( listenfd, ( struct sockaddr* )&servaddr, sizeof( servaddr ) );//将结构体信息绑定到该套接字上
    if( ret == -1 ){
        cout << "bind socket faile from server" << endl;
        exit( 0 );
    }

    ret = listen( listenfd, 10 );//创建监听套接字 监听指定端口的请求，10个请求队列
    if( ret == -1 ){
        cout << "listen socket faile from server" << endl;
        exit( 0 );
    }

    /**添加服务器的信息到poll结构体**/
    clientArryfd[ 0 ].fd = listenfd;
    clientArryfd[ 0 ].events = POLLIN;
    timeout.tv_sec = -1;//阻塞等待
    timeout.tv_usec = 0;

     /**服务器初始化完成等待客户机连接**/
     cout << "------serve waiting client connect---------" << endl;
    while( true ) {

        int ready = poll( clientArryfd, curMaxIndex + 1, timeout.tv_sec );//第二个参数告知有效的结构体数目
        if( ready == -1 )  {
            cout << "poll faile" << endl;
            exit( 0 );
        }

        if( curMaxIndex >= maxConnect ) {
            cout << "max connect and server quit" << endl;
            break;
        }
        //服务器收到新连接
        if( clientArryfd[ 0 ].revents & POLLIN ) {
            connfd = accept( listenfd, ( struct sockaddr* )&cliaddr, &clilen );
            if( connfd == -1 ){
                cout << "accept faile from server" << endl;
                continue;
            }
            else cout << "rev new client connection" << endl;

            for( int i = 1; i < maxConnect; ++i )
                if( clientArryfd[ i ].fd == -1 ) {
                    clientArryfd[ i ].fd = connfd;
                    clientArryfd[ i ].events = POLLIN;
                    curMaxIndex = ( i > curMaxIndex ? i : curMaxIndex ); //获得当前最大下标
                    break;
                }
            //if( --ready <= 0 )//只有服务器准备好,就不必进行后续的收发
                //continue;
        }

        for( int i = 1; i <= curMaxIndex; ++i ) {
            if( clientArryfd[ i ].fd == -1 )
                continue;
            string sendMessage = "welcome connect server, now time is: ";//构建发送消息
            time_t rowtime; time( &rowtime );//时间消息
            sendMessage += ctime( &rowtime );
            write( connfd, sendMessage.c_str(), sendMessage.size() );//发送消息
            if( clientArryfd[ i ].revents & POLLIN ) {
                //利用连接套接字，读取缓冲区的字节数，这里会自动添加一个换行
                recvByteSize = recv( connfd, recvBuff, MaxLINE, 0 );
                if( recvByteSize > 0 ) {
                    cout << "rev message from clint: " << flush;//立即刷新缓冲区
                    write( STDOUT_FILENO, recvBuff, sizeof( recvBuff ) );//服务器显示客户机消息
                    cout << endl;
                    memset( recvBuff, 0, sizeof( recvBuff ) );
                 }
                 else {
                    cout << "rev client message error and close it`s connection" << endl;
                    close( clientArryfd[ i ].fd );
                    clientArryfd[ i ].fd = -1;
                 }
                 if( --ready <= 0 ) break;

            }
        }

    }
    //关闭所有连接
    for(int i = 0; i < maxConnect; ++i){
        if( clientArryfd[ i ].fd != -1 ){
            close( clientArryfd[ i ].fd );
        }
    }
    close( listenfd );
}

