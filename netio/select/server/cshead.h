#ifndef CSHEAD_H
#define CSHEAD_H

#include <iostream>
#include <string>
#include <cstring>
//#include <pthread.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

using namespace std;

void serve( void );

#endif // CSHEAD_H
