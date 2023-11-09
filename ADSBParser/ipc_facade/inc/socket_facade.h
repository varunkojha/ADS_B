#ifndef _SOCKET_FACADE_H_
#define _SOCKET_FACADE_H_

#include "errno.h"
#include <stdio.h>
#include <string.h>     /* strlen */
#include <sys/socket.h> /* sockets */
#include <arpa/inet.h>  /* inet_addr */
#include <unistd.h>     /* write */

#define __USE_GNU
#include <sys/poll.h>

int socketCreate();
int socketBind(int socket_desc, int port);
int socketListen(int socket_desc, int max_num_clients);
int socketAccept(int socket_desc);
int socketConnect(int socket_desc, char* ip_address, int port);
int socketDisconnect(int socket_desc);
int socketPoll(int socket_desc, int usecs);
int socketSend(int socket_desc, char* message, int length);
int socketReceive(int socket_desc, char* data, int length, int usecs);
int socketReceive_nowait(int socket_desc, char* data, int length);

#endif