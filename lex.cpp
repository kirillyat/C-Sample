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

#ifndef LEX_BUFFER_SIZE
#define LEX_BUFFER_SIZE 1024
#endif

bool ifSpecialSymbol(char C);
bool ifSepSymbol(int C);
bool ifNumber(char C);
bool ifAlpha(char C);

enum MachineState
{
    HomeState,
    NumberState,
    StringState,
    KeyWordState,
    IdentifierState,
    ErrorState,
    FinishState
};

struct tokens
{        
    char* token;
    tokens* next;
    //int type;
    int number;
};

class lex
{
    FILE * code;
    enum MachineState state;
private:
    tokens* GetOneToken();
    MachineState newStateHome(int C) const;
    MachineState newStateKeyWord(int C) const;
    MachineState newStateString(int C) const;
    MachineState newStateIdentifier(int C) const;
    MachineState newStateNumber(int C) const;
    MachineState newState(int C) const;
public:
    lex(FILE* F);
    lex(char* PathName);
    tokens* LexicalAnalysis();
    ~lex();
};


lex::lex(FILE* F)
{
    this->code = F;
    this->state = HomeState;
}

lex::lex(char* PathName)
{
    this->code = fopen(PathName, "r");
    if(this->code == NULL){
        perror("Error opening file");
    }
    this->state = HomeState;
}

lex::~lex()
{
    fclose(code);
}



tokens* lex::GetOneToken()
{
    int I = 0, C;
    bool flag = false;
    MachineState S;
    char* Buffer = new char[LEX_BUFFER_SIZE];
    C = getc(this->code);
    while (true) {
        S = newState(C);
        if (S == HomeState) {
            if (this->state != HomeState){
                ungetc(C, this->code);
                this->state = HomeState;
                break;
            }
            if (ifSpecialSymbol(C)) {
                 Buffer[I] = C;
                 I++;
                 break;
            }
        } else if (S == FinishState) {
            this->state = S;
            delete[] Buffer;
            return NULL;
        } else {
            this->state = S;
            Buffer[I] = C;
            I++;
        }
                
        C = getc(this->code);
    }
    tokens* tok = new tokens;
    tok->token = new char[I];
    tok->next = NULL;
    memcpy(tok->token, Buffer, I);
    delete[] Buffer;
    return tok;
}


bool ifSpecialSymbol(char C)
{
    return ((C == '+') || (C == '-') ||
            (C == '*') || (C == '/') ||
            (C == '<') || (C == '>') ||
            (C == '(') || (C == ')') ||
            (C == '[') || (C == ']') ||
            (C == ',') || (C == ';'));
}

bool ifIdentifier(char C)
{
    return ((C == '$') ||
            (C == '@') ||
            (C == '?'));
}

bool ifSepSymbol(int C)
{
    return ((C == ' ')  ||
            (C == '\t') || 
            (C == '\n') ||
            (C == EOF));
}

bool ifNumber(char C)
{
    return ((C >= '0') && (C <= '9'));
}

bool ifAlpha(char C)
{
    return ((C >= 'a') && (C <= 'z')) ||
            ((C >= 'A') && (C <= 'Z'));
}

bool ifNewOperator(tokens* T)
{
    return ((T->token[0] == ';') && (T->token[1] == 0));
}

MachineState lex::newStateHome(int C) const
{
    if (C == EOF)
        return FinishState;
    else if (ifSepSymbol(C))
        return HomeState;
    else if (ifIdentifier(C))
        return IdentifierState;
    else if (ifAlpha(C))
        return KeyWordState;
    else if (ifNumber(C))
        return NumberState;
    else if (ifSpecialSymbol(C))
        return HomeState;
    else
        return HomeState;
}

MachineState lex::newStateKeyWord(int C) const
{
    if (ifAlpha(C))
        return KeyWordState;
    else 
        return HomeState;
}

MachineState lex::newStateNumber(int C) const
{
    if (ifNumber(C))
        return NumberState;
    else
        return HomeState;
}

MachineState lex::newStateString(int C) const
{
    if (C == '"')
        return HomeState;
    else
        return StringState;
}

MachineState lex::newStateIdentifier(int C) const
{
    if (ifAlpha(C))
        return IdentifierState;
    else
        return HomeState;
}

MachineState lex::newState(int C) const
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






tokens* lex::LexicalAnalysis()
{
    int OperatorN = 1;
    tokens *first = NULL, *cur = NULL, *buf = NULL;
    buf = GetOneToken();
    while (this->state != FinishState) {
        if (this->state == FinishState){
            break;
        }
        if (buf != NULL) {
            buf->number = OperatorN;
            if (first == NULL) {
                first = buf;
                cur = first;
            } else {
                cur->next = buf;
                cur = cur->next;
            }
            if (ifNewOperator(buf))
                OperatorN++;
        }
        buf = GetOneToken();
    }
    return first;
}


void PrintToken(tokens* tok)
{
    printf("Operator Number:   %i\n", tok->number);
    printf("Value :  %s\n", tok->token);
    printf("\n");
}
void PrintTokens(tokens* first)
{
    tokens* cur = first;
    while (cur != NULL) {
        PrintToken(cur);
        cur = cur->next;
    }
}

int main(int argc, char* argv[])
{

    FILE* F = fopen("1.py", "r");
    lex L = lex(F);
    tokens *T = L.LexicalAnalysis();
    PrintTokens(T);
    return 0;
}





