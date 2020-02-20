//
// robot.c
// kirillyat
// CMC MSU 2020
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>


class Player {
    int number;
    int money;
    int raw;
    int factory;
    int product;
    public:
    PLayer(int N);
    Player(int N, int M, int R, int F, int P);
    void PrintStat();
    void AddMoney(int S) { this->money+=S; }
    void AddFactory() { this->factory++; }
    void SetRaw(int R) { this->raw = raw; }
    void SetProduct(int P) { this->product = P; }
    void SetMoney(int M) { this->money = M; }
    void SetFactory(int F) { this->factory = F; }
}

class Game {
    int level;
    int prod;
    public:
        
    void SetLevel(int L) { this->level = L; }
    void UpLevel() { this->level++; }
    void SetProd(int P) { this->prod = P}


}


class Bot: Player {
    int sd;
}


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
