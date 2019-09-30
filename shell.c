//
//  shell.c
//
//  Created by kirillyat
//

#include <stdio.h>
#include <stdlib.h>

struct commandWord{
    int value;
    struct commandWord *next;
};

struct commandLine{
    struct commandWord *value;
    struct commandLine *next;
};

void freeTheWord(struct commandWord* W)
{
    struct commandWord* freeW;
    while (W != NULL) {
        freeW = W;
        W = W->next;
        free(freeW);
    }
}

void freeTheLine(struct commandLine* line)
{
    struct commandLine* freeLine;
    while (line != NULL) {
        freeLine = line;
        line = line->next;
        freeTheWord(freeLine->value);
        free(freeLine);
    }
}

void printCommandWord(struct commandWord* W)
{
    if (W != NULL) {
        printf("%c", (char)W->value);
        printCommandWord(W->next);
    }
}

void printCommandLine(struct commandLine* line)
{
    if (line != NULL) {
        printCommandWord(line->value);
        printf(" ");
        printCommandLine(line->next);
    }
}

struct commandWord* readInOneWord()
{
    struct commandWord *first = NULL, *last = NULL;
    int c;
    
    while ((c = getchar()) != '\n') {
        if (first != NULL) {
            last->next = malloc(sizeof(struct commandWord));
            last = last->next;
        } else {
            last = malloc(sizeof(struct commandWord));
            first = last;
        }
        
        last->value = (char)c;
        last->next = NULL;
        
        if (c == EOF) {
            freeTheWord(first->next);
            first->value = EOF;
            first->next = NULL;
            break;
        }
    }
    
    
    return first;
}

struct commandLine* splitCommand(struct commandWord * inputLine)
{
    struct commandLine *first = NULL, *last = NULL;
    int commaFlag = 0;
    if (inputLine == NULL) {
        return first;
    } else if (inputLine->value == EOF){
        first = malloc(sizeof(struct commandLine));
        first->value = inputLine;
        first->next = NULL;
        return first;
    } else {
        struct commandWord *cheracter = inputLine;
        while (cheracr) {
            <#statements#>
        }
        
        //codddddeeeeé
    }
    if(!commaFlag) return first;
    else {
        freeTheLine(first);
        return NULL;
    }
}

void shell()
{
    struct commandLine *shellCommand;
    while (shellCommand = splitCommand(readInOneWord())) {
        if ((shellCommand == NULL) || (shellCommand->value == NULL)) {
            freeTheLine(shellCommand);
            printf("\n");
            continue;
        } else if (shellCommand->value->value == EOF) {
            freeTheLine(shellCommand);
            break;
        }
        
        printCommandLine(shellCommand);
        printf("\n");
        freeTheLine(shellCommand);
    }
}

int main() {
    shell();
    return 0;
}
