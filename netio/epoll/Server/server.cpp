#include "server.h"

static void sighandle( int signo ){
    cout << "signal no : " << signo;
    switch( signo ){
        case 2 :
            cout << " SIGINT " << endl;
            exit( EXIT_SUCCESS );
            break;
        case 3 :
            cout << " SIGQUIT " << endl;
            exit( EXIT_SUCCESS );
            break;
        default :
            cout << " UNKOWN SIGNAL " << endl;
    }
}

char buf[ SIZE ] = {0,};
Server::Server( uint16_t port ) {
    int opt = 1;
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    Bind( port );
    cout << "服务器已就绪，等待客户端连接......." << endl;
}

Server::~Server() {
    close( sockfd );
}

void Server::Bind( uint16_t port) {
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

void Server::Listen() {
    int ret = listen( sockfd, 10 );
    if( ret != 0 ) {
        perror( "listen faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "listen Ok" << endl;
}

void Server::Accept() {
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof( cliaddr );

    connfd = accept( sockfd, ( struct sockaddr* )&cliaddr, &clilen );
    if( connfd == -1 ) {
        perror( "accept faile" );
        exit( EXIT_FAILURE );
    }
    string IP = string( inet_ntoa( cliaddr.sin_addr ) );
    cout << "成功接收一个客户端: " << IP << endl;
    
    addEvent( connfd, EPOLLIN );
}

void Server::addEvent( int eventfd, uint32_t eventState ) {
    event.events = eventState;
    event.data.fd = eventfd;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, eventfd, &this->event );
}

void Server::deleteEvent( int eventfd, uint32_t eventState ) {
    event.events = eventState;
    event.data.fd = eventfd;
    epoll_ctl( epollfd, EPOLL_CTL_DEL, eventfd, &this->event );
} 

void Server::modifyEvent( int eventfd, uint32_t eventState ) {
    event.events = eventState;
    event.data.fd = eventfd;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, eventfd, &this->event );    
}

void Server::doRead( int eventfd, char *buf ) {
    ssize_t byte;
    byte = read( eventfd, buf, SIZE );
    buf[ byte ] = '\0';
    if( byte <= 0 ) {

        switch( errno ) {
            case EAGAIN :
                break;
            case EINTR :
                byte = read( eventfd, buf, SIZE );
                break;
            default :
                perror( "read faile" );
                close( eventfd );
                deleteEvent( eventfd, EPOLLIN );
                cout << "client close connect " << endl;
        }
    }
    else {
        cout << "recv byte : " << byte;
        modifyEvent( eventfd, EPOLLOUT );
    }
}

void Server::doWrite( int eventfd, char *buf ) {
    ssize_t byte;
    byte = write( eventfd, buf, strlen( buf ) );

    if( byte == -1 ) {
        perror( "write faile" );
        deleteEvent( eventfd, EPOLLOUT );
        close( eventfd );
    }
    else {
        cout << " Server broadcast OK and write byte : " << byte << endl;
        modifyEvent( eventfd, EPOLLIN );
    }
    memset( buf, 0, SIZE );
}

void Server::handlEvents( int eventfd, struct epoll_event *events, int cnt ) {
    //根据事件类型执行不同的处理
    //遍历就绪队列
    for ( int i = 0; i < cnt; ++i ) {
        int fd = events[ i ].data.fd;
        if( fd == eventfd && events[ i ].events & EPOLLIN )
            Accept();
        else if ( events[ i ].events & EPOLLIN )
            doRead( fd, buf );
        else {
            doWrite( fd, buf );       
        }
    }
}

void Server::doEpoll() {
    struct epoll_event events[ SUMEVENT ]; //保存从内核获取的就绪队列
    epollfd = epoll_create( SUMEVENT );

    while ( true ) {
        addEvent( sockfd, EPOLLIN );
        int cnt = epoll_wait( epollfd, events, SUMEVENT, -1 );
        handlEvents( sockfd, events, cnt );
    }
    close( sockfd );
}

void Server::Run() {
    signal_set();
    doEpoll();
}

void Server::daemon_run(){
    daemon( 0, 0 );
    Run();
}

void Server::signal_set(){
    signal( SIGINT, sighandle );
    signal( SIGQUIT, sighandle );
}