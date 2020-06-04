#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/epoll.h>
using namespace std;

const int SUMEVENT = 100;
const int SIZE = 256;
class Server {

private:
    struct sockaddr_in servaddr;
    struct epoll_event event;
    socklen_t servlen;
    int sockfd;
    int epollfd;
    
    void Bind( uint16_t );
    void Listen();
    void Accept();
       
    void addEvent( int eventfd, uint32_t eventState );
    void modifyEvent( int eventfd, uint32_t eventState );
    void deleteEvent( int eventfd, uint32_t eventState );
    void modifyevent( int eventfd, uint32_t eventState );
    void doEpoll( );
    void doRead( int eventfd, char *buf );
    void doWrite( int eventfd, char *buf );
    void handlEvents( int eventfd, struct epoll_event* events, int cnt );
    
public:
    Server( uint16_t );
    ~Server();
    void Run();

};


#endif // SERVER_H
