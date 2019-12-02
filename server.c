//
//  server.c
//  CMC MSU
//  Created by kirillyat on 14.11.2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include "myString.h"

#ifndef MAX_USER_AMOUNT
#define MAX_USER_AMOUNT 16
#endif

#ifndef MAX_QUEUE_LEN
#define MAX_QUEUE_LEN 16
#endif

struct session {
    
    int fd;
    char *username;
    unsigned long from_ip;
    unsigned short from_port;
    char buf[1024];
    int buf_used;
};

struct server {
    int ls;
    struct session **session_array;
    int sessionCount;
};

struct session *initNewSession(int fd, struct sockaddr_in *from)
{
    struct session *NewSession = malloc(sizeof(struct session));
    NewSession->fd = fd;
    NewSession->username = NULL;
    NewSession->from_ip = ntohl(from->sin_addr.s_addr);
    NewSession->from_port = ntohs(from->sin_port);
    return NewSession;
}

int initServer(struct server *serv, int port)
{
    int i;
    int sd = socket(AF_INET, SOCK_STREAM, 0);
 
    if (sd == -1) {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    

    if (bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        perror("bind");
        return -1;
    }
    
    if (listen(sd, 5) != 0) {
        perror("listen");
        return -1;
    }
    
    serv->ls = sd;
    serv->sessionCount = MAX_USER_AMOUNT;
    serv->session_array = malloc(sizeof(*serv->session_array) * MAX_USER_AMOUNT);
    for (i = 0; i < MAX_USER_AMOUNT; i++) {
        serv->session_array[i] = NULL;
    }
    return 0;
}

void acceptClient (struct server *serv)
{
    int sd, i;
       struct sockaddr_in addr;
       socklen_t len = sizeof(addr);
       sd = accept(serv->ls, (struct sockaddr*) &addr, &len);
       if(sd == -1) {
           perror("accept");
           return;
       }
}

int serverLoop(struct server *serv)
{
    
    for (;;) {
        
    }
    
    return 0;
}


int main(int argc, const char * argv[])
{
    struct server my_server;
    int port = 2222;
    if (initServer(&my_server, port) == -1){
        perror("initServer");
        return 1;
    }
    serverLoop(&my_server);
    return 0;
}
