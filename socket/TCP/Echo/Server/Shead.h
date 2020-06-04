#ifndef __SHEAD_H__
#define __SHEAD_H__

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void server();

#endif // __SHEAD_H__


