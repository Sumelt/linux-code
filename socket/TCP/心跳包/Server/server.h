/*
 *服务器主线程接受新连接
 *子线程检测客户机状态
 *
 */

#include "Shead.h"

enum Type {HEART, OTHER};

struct package{
    Type type;
    int length;
};

void* heartHeadle( void *arg );


class Server{

    friend void *heartHeadle( void* );
public:
    Server( int port );
    ~Server();
    void CreatSock();
    void Bind( int port );
    void Listen();
    void Accept();
    void Recv();
    void Run();

private:
    struct sockaddr_in servaddr;
    struct timeval timeout;//select的超时时间
    socklen_t servlen;
    int sockfd;
    int maxfd;
    fd_set cliSetFd;//用于接收新连接请求的集合
    fd_set masterSet;//处理主要事件
    map<int, pair<string, int> >mp; //记录客户的IP的心跳包次数 套接字，IP地址　次数

};

Server::Server( int port )
{
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;

    Bind( port );
}

Server::~Server()
{
    for( int fd = 0; fd <= maxfd; ++fd )
        if( FD_ISSET( fd, &masterSet ) )
            close( fd );
}

void Server::Bind( int port )
{
    int ret;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( port );
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );

    ret = bind( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 ) {
        perror( "bind faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "bing Ok" << endl;

    Listen();
}

void Server::Listen()
{
    int ret = listen( sockfd, 10 );
    if( ret != 0 ) {
        perror( "listen faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "listen Ok" << endl;

}

void Server::Accept()
{
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof( cliaddr );

    connfd = accept( sockfd, ( struct sockaddr* )&cliaddr, &clilen );
    if( connfd == -1 ) {
        perror( "accept faile" );
        exit( EXIT_FAILURE );
    }

    string IP = string( inet_ntoa( cliaddr.sin_addr ) );
    mp.insert( make_pair( connfd, make_pair( IP, 0 ) ) );
    cout << "client from: " << IP << endl;

    FD_SET( connfd, &masterSet );
    maxfd = ( connfd > maxfd ? connfd : maxfd );

}

void Server::Recv()
{
    package messPackage;
    bool connStatus = false;
    for( int fd = 0; fd <= maxfd; ++fd ) {
        if( FD_ISSET( fd, &cliSetFd ) ) {
            int byte = recv( fd, &messPackage, sizeof( messPackage ) , 0 );

            if( byte == -1 ) {
                perror( "recv faile" );
                connStatus = true;
            }
            else if( byte == 0 )
                continue;

            if( messPackage.type == HEART ) { //成功收到心跳包
                cout << "Received heart-beat from client" << endl;
                mp[ fd ].second = 0; //累计清零
            }
            else cout << "Received message" << endl;

            if( connStatus ) {
                close( fd );
                FD_CLR( fd, &masterSet );
                while( maxfd >= 0 && FD_ISSET( maxfd, &masterSet ) == false )
                        --maxfd;
            }
        }
    }
}


void Server::Run()
{
    FD_ZERO( &masterSet );
    FD_SET( sockfd, &masterSet );
    maxfd = sockfd;

    pthread_t pid;
    int ret;
    ret = pthread_create( &pid, nullptr, heartHeadle, ( void* )this );
    if( ret != 0 ) {
        perror( "pthread create faile" );
        exit( EXIT_FAILURE );
    }
    cout << "wait client connect" << endl;
    while( true ) {
        FD_ZERO( &cliSetFd );
        memcpy( &cliSetFd, &masterSet, sizeof( masterSet ) );

        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        int num = select( maxfd + 1, &cliSetFd, nullptr, nullptr, &timeout );
        if( num == -1 ) {
            perror( "select faile" );
            exit( EXIT_FAILURE );
        }
        else if( num == 0 ) {
            cout << "time out" << endl;
            continue;
        }

        //事件发生了
        if( FD_ISSET( sockfd, &cliSetFd ) )
            Accept();
        else Recv();
    }
}

void *heartHeadle( void *arg )
{
    cout << "The heartbeat checking thread started" << endl;
    Server* s = (Server*)arg;

    while( true ) {
        auto iter = s->mp.begin();
        for( ; iter != s->mp.end(); ++iter ) {
            //该客户掉线
            if( iter->second.second == 5 ) {
                cout << "The client " << iter->second.first
                        << " has be offline" << endl;
                close( iter->first );//关闭该套接字
                FD_CLR( iter->first, &s->masterSet );//清除该位
                //更新maxfd
                if( iter->first == s->maxfd )
                    while( s->maxfd >= 0 &&
                            FD_ISSET( s->maxfd, &s->masterSet ) == false )
                                --s->maxfd;

                s->mp.erase( iter );
            }
            //遍历下一个
            else if( iter->second.second >=0 && iter->second.second <= 5 )
                ++iter->second.second;
        }
        sleep( 3 );
    }
}

