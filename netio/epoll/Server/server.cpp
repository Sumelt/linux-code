
#include "server.h"

Server::Server( uint16_t port ) {
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;

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
    if( byte == -1 ) {
        perror( "read faile" );
        deleteEvent( eventfd, EPOLLIN );
        close( eventfd );
        exit( EXIT_FAILURE );
    }
    else if ( byte > 0 ) {
        cout << "recv: " << buf << endl;
        modifyEvent( eventfd, EPOLLOUT );
    }
    else {
        cout << " client close" << endl;
        close( eventfd );
        deleteEvent( eventfd, EPOLLIN );
    }
}

void Server::doWrite( int eventfd, char *buf ) {
    ssize_t byte;
    byte = write( eventfd, buf, SIZE );
    if( byte == -1 ) {
        perror( "write faile" );
        deleteEvent( eventfd, EPOLLOUT );
        close( eventfd );
        exit( EXIT_FAILURE );
    }
    else {
        cout << "Server Echo OK" << endl;
        modifyEvent( eventfd, EPOLLIN );
    }
    memset( buf, 0, SIZE );
}

void Server::handlEvents( int eventfd, struct epoll_event *events, int cnt ) {
    char buf[ SIZE ] = { 0 };
    //根据事件类型执行不同的处理
    //遍历就绪队列
    for ( int i = 0; i < cnt; ++i ) {
        int fd = events[ i ].data.fd;
        if( fd == eventfd && events[ i ].events & EPOLLIN )
            Accept();
        else if ( events[ i ].events & EPOLLIN )
            doRead( fd, buf );
        else
            doWrite( fd, buf );       
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
    doEpoll();
}
