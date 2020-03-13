//
// lex.cpp
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


#ifndef LexBufferSize
#define LexBufferSize 2048
#endif

bool ifSpecialSymbol(char C);
bool ifSepSymbol(char C);
bool ifNumber(char C);
bool ifAlpha(char C);

enum MachineState
{
    HomeState,
    NumberState,
    StringState,
    KeyWordState,
    EqualState,
    IdentifierState,
    ErrorState
};

struct tokens
{        
    char* token;
    tokens* next;
    int size;
    int type;
    int number;
};

class lex
{
    char* Buffer;
    int BufferUsed;
    int fd;
    enum MachineState state;
private:
    bool ReadData();
    tokens* GetOneToken();
    MachineState newStateHome(char C) const;
    MachineState newStateKeyWord(char C) const;
    MachineState newStateString(char C) const;
    MachineState newStateIdentifier(char C) const;
    MachineState newStateNumber(char C) const;
    MachineState newState(char C) const;
public:
    lex(int fd);
    lex(char* PathName);
    ~lex();
};


lex::lex(int fd)
{
    this->Buffer = new char[LexBufferSize];
    this->BufferUsed = 0;
    this->fd = fd;
    this->state = HomeState;
}

lex::lex(char* PathName)
{
    this->Buffer = new char[LexBufferSize];
    this->BufferUsed = 0;
    this->fd = open(PathName, O_RDONLY);
    this->state = HomeState;
}

lex::~lex()
{
    delete []this->Buffer;
}


bool lex::ReadData()
{
    int rc; 
    rc = read(fd, Buffer + BufferUsed, LexBufferSize - BufferUsed);
    if (rc <= 0) {
        //TODO : error
        return false;
    }
    BufferUsed += rc;
    if (BufferUsed >= LexBufferSize) {
        //TODO : error
        return false;
    }
    return true;
}


tokens* lex::GetOneToken()
{
    int begin = 0, end = 0;
    bool b = true;
    tokens* tok = new tokens;
    for (end = 0; end < BufferUsed; end++) {
            MachineState s = newState(Buffer[end]);

            state = s;
    }
    return tok;
}


bool ifSpecialSymbol(char C)
{
    return  (C == '+') || (C == '-') ||
            (C == '*') || (C == '/') ||
            (C == '<') || (C == '>') ||
            (C == '(') || (C == ')');
}

bool ifIdentifier(char C)
{
    return  (C == '$') ||
            (C == '@') ||
            (C == '?');
}

bool ifSepSymbol(char C)
{
    return  (C == ' ')  ||
            (C == '\t') || 
            (C == '\n');
}

bool ifNumber(char C)
{
    return  ((C >= '0') && (C <= '9'));
}

bool ifAlpha(char C)
{
    return  ((C >= 'a') && (C <= 'z')) ||
            ((C >= 'A') && (C <= 'Z'));
}



MachineState lex::newStateHome(char C) const
{
    if (ifSepSymbol(C))
        return HomeState;
    else if (ifIdentifier(C))
        return IdentifierState;
    else if (ifAlpha(C))
        return KeyWordState;
    else if (ifNumber(C))
        return NumberState;
    else if (ifSpecialSymbol(C))
        return HomeState;
    return ErrorState;
}

MachineState lex::newStateKeyWord(char C) const
{
    if (ifAlpha(C))
        return KeyWordState;
    else 
        return HomeState;
}

MachineState lex::newStateNumber(char C) const
{
    if (ifNumber(C))
        return NumberState;
    else
        return HomeState;
}

MachineState lex::newStateString(char C) const
{
    if (C == '"')
        return HomeState;
    else
        return StringState;
}

MachineState lex::newStateIdentifier(char C) const
{
    if (ifAlpha(C))
        return IdentifierState;
    else
        return HomeState;
}

MachineState lex::newState(char C) const
{
    if (this->state == HomeState)
        return newStateHome(C);
    else if (this->state == KeyWordState)
        return newStateKeyWord(C);
    else if (this->state == NumberState)
        return newStateNumber(C);
    else if (this->state == StringState)
        return newStateString(C);
    else if (this->state == IdentifierState)
        return newStateIdentifier(C);
    else return ErrorState;
}









int main (int argc, char* argv[])
{
    lex(open(*(argv+1), O_RDONLY));
    return 0;
}

