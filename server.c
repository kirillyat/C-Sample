/*
    server.c
    CMC MSU
    Created by kirillyat
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef MAX_USER_AMOUNT
#define MAX_USER_AMOUNT 16
#endif

#ifndef MAX_QUEUE_LEN
#define MAX_QUEUE_LEN 16
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

enum session_states {
    start_state,
    game_state,
    error_state,
    finish_state,
};
struct session {
    int fd;
    char *username;
    unsigned long from_ip;
    unsigned short from_port;
    char buf[BUFFER_SIZE];
    int buf_used;
    enum session_states state;
};

struct server {
    int ls;
    struct session **session_array;
    int sessionCount;
};

void sendMessage(struct session *sess, char *str)
{
    write(sess->fd, str, sizeof(str));
}



int getAnswer(struct session *sess)
{
    int rc, bufp = sess->buf_used;
    rc = read(sess->fd, sess->buf + bufp, BUFFER_SIZE - bufp);
    if(rc <= 0) {
        sess->state = error_state;
        return 0;   /* this means "don't continue" for the caller */
    }
    sess->buf_used += rc;
    session_check_lf(sess);
    
    if(sess->buf_used >= BUFFER_SIZE) {
        sendMessage(sess, "Buffer overflow! Close session.\n");
        sess->state = error_state;
        return 0;
    }
    if(sess->state == finish_state)
        return 0;
    return 1;
}

struct session *initNewSession(int fd, struct sockaddr_in *from)
{
    struct session *newSession = malloc(sizeof(struct session));
    newSession->fd = fd;
    newSession->username = NULL;
    newSession->from_ip = ntohl(from->sin_addr.s_addr);
    newSession->from_port = ntohs(from->sin_port);
    newSession->buf_used = 0;
    newSession->state = start_state;
    return newSession;
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

    if (listen(sd, MAX_QUEUE_LEN) != 0) {
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

void acceptClient(struct server *serv)
{
    int sd, i;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
   
    sd = accept(serv->ls, (struct sockaddr*) &addr, &len);
    if (sd == -1) {
        perror("accept");
        return;
    }
    
    if (sd >= serv->sessionCount) {  /* need to resize */
        int newlen = serv->sessionCount;
        
        while (sd >= newlen)
            newlen += MAX_USER_AMOUNT;
        
        serv->session_array =realloc(serv->session_array, newlen * sizeof(struct session*));
        for (i = serv->sessionCount; i < newlen; i++)
            serv->session_array[i] = NULL;
        serv->sessionCount = newlen;
    }
    
    serv->session_array[sd] = initNewSession(sd, &addr);
}

void server_remove_session(struct server_str *serv, int sd)
{
    close(sd);
    serv->sess_array[sd]->fd = -1;
    session_cleanup(serv->sess_array[sd]);
    free(serv->sess_array[sd]);
    serv->sess_array[sd] = NULL;
}

void server_close_session(struct server_str *serv, int sd)
{
    if(serv->sess_array[sd]->state == fsm_finish)
        session_commit(serv->sess_array[sd], serv->res);
    server_remove_session(serv, sd);
}

int serverLoop(struct server *serv)
{
    int i, maxfd, selectrezult, readrez;
    fd_set readfds;
    for (;;) {
        FD_ZERO(&readfds);
        FD_SET(serv->ls, &readfds);
        maxfd = serv->ls;
        for (i = 0; i < serv->sessionCount; i++) {
            if (serv->session_array[i]) {
                FD_SET(i, &readfds);
                if (i > maxfd)
                    maxfd = i;
            }
        }
        
        selectrezult = select(maxfd+1, &readfds, NULL, NULL, NULL);
        if (selectrezult == -1) {
            perror("select");
            return -1;
        }
        if (FD_ISSET(serv->ls, &readfds)) /*New Client*/
            acceptClient(serv);
        for (i = 0; i < serv->sessionCount; i++) {
            if (serv->session_array[i] && FD_ISSET(i, &readfds)) {
                readrez = getAnswer(serv->session_array[i]);
                if (!readrez)
                    server_close_session(serv, i);
            }
        }
    
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

