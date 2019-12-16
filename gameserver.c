/*
    gameserver.c
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
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef MAX_USER_AMOUNT
#define MAX_USER_AMOUNT 50
#endif

#ifndef MAX_QUEUE_LEN
#define MAX_QUEUE_LEN 50
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif

#ifndef FACTORY_PRICE
#define FACTORY_PRICE 5000
#endif

#ifndef RAW_TAX
#define RAW_TAX 300
#endif

#ifndef PRODUCT_TAX
#define PRODUCT_TAX 500
#endif

#ifndef FACTORY_TAX
#define FACTORY_TAX 1000
#endif


const char text_welcome[] = "Welcome on board!\n";
const char help_text_help[] = "help   -> to get this instractions \n";
const char help_text_market[] = "market -> to get info about market \n";
const char help_text_build[] = "build  -> to build a factory\n";
const char help_text_turn[] = "turn   -> to finish this turn\n";
const char help_text_player[] = "player -> to get info about player #<Player Number> \n";
const char help_text_buy[] = "buy    -> \n";
const char help_text_sell[] = "sell   -> \n";
const char text_bye[] = "Disconnect #";

const int level_change [5] [5] = {
    {4, 4, 2, 1, 1},
    {3, 4, 3, 1, 1},
    {1, 3, 4, 3, 1},
    {1, 1, 3, 4, 3},
    {1, 1, 2, 4, 4}
};

enum command_set {
    start_state,
    market_command,
    prod_command,
    buy_command,
    sell_command,
    build_command,
    help_command,
    turn_command,
    error_state,
    finish_state = error_state
};

struct player {
    int number;
    int fd;
    int turn;
    int raw;
    int factory;
    int product;
    int money;
    int build;
    char buf[BUFFER_SIZE];
    int buf_used;
    enum command_set state;
};

struct server {
    int month;
    int turns;
    int level;
    int onlinePlayers;
    int ls;
    struct player **session_array;
    int sessionCount;
};

void sendMessage(struct player *plr, char *str)
{
    write(plr->fd, str, sizeof(str)*sizeof(char));
}

void sendInt(struct player *plr, int number)
{
    char numm[256];
    sprintf(numm, "%d", number);
    sendMessage(plr, numm);
}


void sendMessageAndInt(struct player *plr, char *str, int number)
{
    sendMessage(plr, str);
    sendInt(plr, number);
    write(plr->fd, "\n", 1);
}

void sendStats(struct player *plr, struct player *data)
{
    write(plr->fd, "\n", 1);
    sendMessageAndInt(plr, "Player   №", data->number);
    sendMessageAndInt(plr, "Money:   $", data->money);
    sendMessageAndInt(plr, "Factory:  ", data->factory);
    sendMessageAndInt(plr, "Product:  ", data->product);
    sendMessageAndInt(plr, "Raw:      ", data->raw);
}

void sendHelp(struct player *plr)
{
    write(plr->fd, "\n", 1);
    write(plr->fd, help_text_help, sizeof(help_text_help));
    write(plr->fd, help_text_market, sizeof(help_text_market));
    write(plr->fd, help_text_player, sizeof(help_text_player));
    write(plr->fd, help_text_buy, sizeof(help_text_buy));
    write(plr->fd, help_text_sell, sizeof(help_text_sell));
    write(plr->fd, help_text_build, sizeof(help_text_build));
    write(plr->fd, help_text_turn, sizeof(help_text_turn));
}

void sendMarket(struct player *plr, struct server* serv)
{
    write(plr->fd, "\n", 1);
    sendMessageAndInt(plr, "Online : ", serv->onlinePlayers);
    sendMessageAndInt(plr, "Month  : ", serv->month);
    // TODO: sd/klfk
}

void sendGoodbye(struct server* serv, int num)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        if ((serv->session_array[i]) &&
            (serv->session_array[i]->fd >= 0)) {
            write(serv->session_array[i]->fd, text_bye, sizeof(text_bye));
            sendInt(serv->session_array[i], num);
        }
    }
}



void sendMessageALL(struct server *serv, char *str)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        if (serv->session_array[i]->fd >= 0){
            sendMessage(serv->session_array[i], str);
        }
    }
}


void sendIntALL(struct server *serv, int num)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        if (serv->session_array[i]->fd >= 0){
            sendInt(serv->session_array[i], num);
        }
    }
}

void sendMessageAndIntALL(struct server *serv, char *str, int num)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        if (serv->session_array[i]->fd >= 0){
            sendMessage(serv->session_array[i], str);
            sendInt(serv->session_array[i], num);
            write(serv->session_array[i]->fd, "\n", 1);
        }
    }
}



int newLevel(int currentLevel)
{
    int i, sum = 0, r = 1 + (int)(12.0*rand()/(RAND_MAX+1.0));
    for (i = 0; i < 5; i++) {
        sum += level_change[currentLevel][i];
        if (sum >= r)
            return i;
    }
    return (r % 5);
}



struct player *initPlayer(int fd, struct sockaddr_in *from)
{
    struct player *newPlayer = malloc(sizeof(struct player));
    newPlayer->fd = fd;
    newPlayer->number = fd;
    newPlayer->factory = 2;
    newPlayer->turn = 0;
    newPlayer->money = 10000;
    newPlayer->raw = 4;
    newPlayer->product = 2;
    newPlayer->build = 0;
    newPlayer->buf_used = 0;
    newPlayer->state = start_state;
    write(fd, text_welcome, sizeof(text_welcome));
    sendHelp(newPlayer);
    return newPlayer;
}



void commandExecute(struct player *sess, char *line)
{
    switch (sess->state) {
        case start_state:
            sendStats(sess, sess);
            free(line);
            break;
        
        case finish_state:
            free(line);
            break;
    }
}


void session_check_lf(struct player *sess)
{
    int pos = -1;
    int i;
    char *line;
    for(i = 0; i < sess->buf_used; i++) {
        if(sess->buf[i] == '\n') {
            pos = i;
            break;
        }
    }
    if (pos == -1)
        return;
    line = malloc(pos+1);
    memcpy(line, sess->buf, pos);
    line[pos] = 0;
    memmove(sess->buf, sess->buf+pos, pos+1);
    sess->buf_used -= (pos+1);
    if(line[pos-1] == '\r')
        line[pos-1] = 0;
    commandExecute(sess, line);
}

int getAnswer(struct player *sess)
{
    int rc;
    rc = read(sess->fd, sess->buf + sess->buf_used, BUFFER_SIZE - sess->buf_used);
    if (rc <= 0) {
        sess->state = error_state;
        return 0;
    }
    sess->buf_used += rc;
    session_check_lf(sess);
    
    if (sess->buf_used >= BUFFER_SIZE) {
        sendMessage(sess, "Buffer overflow! Close session.\n");
        sess->state = error_state;
        return 0;
    }
    if (sess->state == finish_state)
        return 0;
    return 1;
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
    
    serv->session_array[sd] = initPlayer(sd, &addr);
    
}


int registratePlayers(struct server* serv)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        acceptClient(serv);
        // TODO: new user message
        
    }
    return 0;
}

int initServer(struct server *serv, int port, int playersNumber)
{
    int i, sd = socket(AF_INET, SOCK_STREAM, 0);
 
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
    serv->month = 0;
    serv->level = 0;
    serv->onlinePlayers = playersNumber;
    serv->sessionCount = playersNumber;
    serv->session_array = malloc(sizeof(*serv->session_array) * MAX_USER_AMOUNT);
    for (i = 0; i < playersNumber; i++) {
        serv->session_array[i] = NULL;
    }
    
    registratePlayers(serv);
    
    return 0;
}



void server_remove_session(struct server *serv, int sd)
{
    serv->onlinePlayers--;
    close(sd);
    serv->session_array[sd]->fd = -1;
    free(serv->session_array[sd]);
    serv->session_array[sd] = NULL;
    sendGoodbye(serv, sd);
}

void getTaxes(struct server* serv)
{
    int i;
    for (i = 0; i < serv->sessionCount; i++) {
        if (serv->session_array[i]->fd >= 0){
            serv->session_array[i]->money -=
            serv->session_array[i]->raw*RAW_TAX +
            serv->session_array[i]->product*PRODUCT_TAX +
            serv->session_array[i]->factory*FACTORY_TAX;
        }
    }
}
int readSockets(struct server *serv)
{
    int i, maxfd, selectrezult, readrez;
    fd_set readfds;
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
    for (i = 0; i < serv->sessionCount; i++) {
        if (serv->session_array[i] && FD_ISSET(i, &readfds)) {
            readrez = getAnswer(serv->session_array[i]);
            if (!readrez)
                server_remove_session(serv, i);
        }
    }
    return 0;
}

int serverLoop(struct server *serv)
{
    int i;
    for (;;) {
        serv->turns = 0;
        for (;;) {
            readSockets(serv);
            if (serv->turns >= serv->onlinePlayers) {
                getTaxes(serv);
                serv->month++;
                serv->turns = 0;
                serv->level = newLevel(serv->level);
                for (i = 0; i < serv->sessionCount; i++) {
                    if (serv->session_array[i]) {
                        serv->session_array[i]->turn = 0;
                    }
                }
                break;
            }
        }
        if (serv->onlinePlayers <= 0) {
            break;
        }
    }
    return 0;
}


int main(int argc, const char * argv[])
{
    struct server my_server;
    int port = 2222, playersNumber = 0;
    
    if (argc >= 3) {
        port = atoi(argv[1]);
        playersNumber = atoi(argv[2]);
    } else {
        printf("error init");
        return 1;
    }
    if (initServer(&my_server, port, playersNumber) == -1){
        printf("error init");
        return 1;
    }
    serverLoop(&my_server);
    return 0;
}
