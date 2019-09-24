//
// Created by Кирилл Яценко on 23/09/2019.
//

#include <stdio.h>
#include <stdlib.h>

struct commandWord{
    char value;
    struct commandWord *next;
};

struct commandLine{
    struct commandWord *value;
    struct commandLine *next;
};


void freeTheWord(struct commandWord* W)
{
    struct commandLine* freeW;
    while(W != NULL){
        freeW = W;
        W = W->next;
        free(freeW);
    }
}

void freeTheLine(struct commandLine* line)
{
    struct commandLine* freeLine;
    while(line != NULL){
        freeLine = line;
        line = line->next;
        freeTheWord(freeLine->value);
        free(freeLine);
    }
}

void printCommandWord(struct commandWord* W)
{
    if (W != NULL){
        printf("%c", W);
        printCommandWord(W->next);
    }
    printf(" ");
}

void printCommandLine(struct commandLine* line)
{
    if (line != NULL){
        printCommandWord(line->value);
        printCommandLine(line->next);
    }
    printf("/n");
}



void shell()
{
    int point = 0;
    struct commandLine* line;
    struct commandWord* wrd;
    short flag = 0;              //флаг кавычек

    while(point != EOF){
        if(point == EOL){
            printCommandLine(line);
            freeTheLine(line);
            point = 0;
            continue;
        }
        while(f and ((point = getchar())!=EOL)){

        }


    }

}





int main(int argc, char *argv[])
{
    shell();
    return 0;
}
