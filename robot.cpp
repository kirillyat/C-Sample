//
// robot.c
// kirillyat
// CMC MSU 2020
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class Player {
    int number;
    int money;
    int raw;
    int factory;
    int product;
  public:
    Player(int N, int M, int R, int F, int P);
    void PrintStat();
    void AddMoney(int S) { this->money+=S; }
    void AddFactory() { this->factory++; }
    void SetRaw(int R) { this->raw = R; }
    void SetProduct(int P) { this->product = P; }
    void SetMoney(int M) { this->money = M; }
    void SetFactory(int F) { this->factory = F; }
};   


class Game {
    int level;
    int prod;
  public:
    void SetLevel(int L) { this->level = L; }
    void UpLevel() { this->level++; }
    void SetProd(int P) { this->prod = P; }
};


class Bot:public Player {
    int sd;

  public:
};


Player::Player(int N, int M, int R, int F, int P)
{
    this->number = N;
    this->money = M;
    this->raw = R;
    this->factory = F;
    this->product = P;
}


void Player::PrintStat()
{
    printf("Player N");
}




int main ()
{
    return 0;
}
