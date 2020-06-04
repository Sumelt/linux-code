#ifndef CSHEAD_H
#define CSHEAD_H

#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void clientCore( void *value );

#endif // CSHEAD_H
