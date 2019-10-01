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
        printf("%c", W->value);
        printCommandWord(W->next);
    }
}

void printCommandLine(struct commandLine* line)
{
    if (line != NULL) {
        printCommandWord(line->value);
       // printf(" ");
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
        last->value = c;
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


struct commandWord* cleanFromSpaces(struct commandWord * inputLine)
{
    struct commandWord *last = NULL, *first = NULL, *freeW;
    int commaFlag = 0, spaceFlag = 1;
    while (inputLine != NULL) {
        
        if ((inputLine->value == ' ') && !commaFlag && spaceFlag){
            freeW = inputLine;
            inputLine = inputLine->next;
            free(freeW);
            continue;
        }
        if (inputLine->value == '\"') commaFlag = (commaFlag)?0:1;
        if (inputLine->value != ' ') spaceFlag = 0;
        if (inputLine->value == ' ') spaceFlag = (commaFlag)?0:1;
       
        
        if (first == NULL){
            first = inputLine;
            last = first;
        } else {
            last->next = inputLine;
            last = last->next;
        }
        
        inputLine = inputLine->next;
        last->next = NULL;
    }

    return first;
}

struct commandLine* splitCommand(struct commandWord * inputLine)
{
    struct commandLine *first = NULL, *last = NULL;
    struct commandWord *prev = inputLine, *freeW;
    int commaFlag = 0;
    
    while (inputLine!=NULL) {
        if (first == NULL){
            first = malloc(sizeof(struct commandLine));
            first->value = inputLine;
            last = first;
        } else {
            last->next = malloc(sizeof(struct commandLine));
            last = last->next;
            last->value = inputLine;
        }
        last->next = NULL;
        
        while((inputLine->value != ' ') && !commaFlag) {
        
            if (inputLine->value == '\"') commaFlag = (commaFlag)?0:1;
            prev = inputLine;
            inputLine = inputLine->next;
            if (inputLine == NULL) break;
        }
        freeW = inputLine;
        if (inputLine) inputLine = inputLine->next;
        free(freeW);
        prev->next = NULL;
    }
    return first;
}

void shell()
{
    
}

int main() {

    struct commandLine* s = splitCommand(cleanFromSpaces(readInOneWord()));
    printf("\n");
    printCommandLine(s);
    printf("\n");
    freeTheLine(s);

    
    
    return 0;
}
