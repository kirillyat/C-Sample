//
//  Created by kirillyat on 14.11.2019.
//

#include "myString.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


void freeword(struct word *inputword)
{
    struct word *fword;
    while (inputword != NULL) {
        fword = inputword;
        inputword = inputword->next;
        free(fword);
    }
}


void freeline(struct line* inputline)
{
    struct line *fline;
    while (inputline != NULL) {
        fline = inputline;
        inputline = inputline->next;
        freeword(fline->value);
        free(fline);
    }
}


void freestring(char **input)
{
    if (input != NULL){
        int i;
        for (i = 0; input[i] != NULL; i++) {
            free(input[i]);
        }
        free(input[i]);
        free(input);
    }
}


void printword(struct word* inputword)
{
    if (inputword != NULL) {
        printf("%c", (char)inputword->value);
        printword(inputword->next);
    }
}


void printline(struct line* inputline)
{
    if (inputline != NULL) {
        printword(inputline->value);
        if (inputline->next != NULL)
            printf(" ");
        printline(inputline->next);
    }
}


void printstring(char* const *output)
{
    if (output != NULL) {
        int i;
        for (i = 0; output[i] != NULL; i++)
            printf("%s ", output[i]);
        printf("\n");
    }
}


int wordlen(struct word *inputword)
{
    int count = 0;
    while (inputword != NULL) {
        inputword = inputword->next;
        count++;
    }
    return count;
}

int linelen(struct line *inputline)
{
    int count = 0;
    while (inputline != NULL) {
        inputline = inputline->next;
        count++;
    }
    return count;
}


char* word2string (struct word* list)
{
    int wordsize = wordlen(list), i;
    char *str = malloc(sizeof(char) * (1 + wordsize));
    for (i = 0; i < wordsize; i++) {
        str[i] = (char)list->value;
        list = list->next;
    }
    str[wordsize] = 0;
    return str;
}


char** list2string (struct line* list)
{
    int i, j, linesize = linelen(list);
    char **str = malloc(sizeof(char*)*(linesize + 1));
    str[linesize] = NULL;
    for (i = 0; i < linesize; i++) {
        struct word *curword = list->value;
        int wordsize = wordlen(curword);
        
        str[i] = malloc(sizeof(char) * (1 + wordsize));
        for (j = 0; j < wordsize; j++) {
            str[i][j] = (char)curword->value;
            curword = curword->next;
        }
        str[i][wordsize] = 0;
        list = list->next;
    }
    
    return str;
}


int ifWordIsReady(int symbol)
{
    return ((symbol == '\n') || (symbol == EOF ));
}


int readWord(struct word** first)
{
    struct word *last = NULL;
    int symbol, commaFlag = 0;
    freeword(*first);
    (*first) = NULL;
    for (;;) {
        symbol = getchar();
        if (ifWordIsReady(symbol)) {
            if ((symbol == '\n') && (commaFlag))
                symbol = '\"';
            return symbol;
        } else if (symbol == '\"') {
            commaFlag = (commaFlag == 1)?0:1;
        } else {
            if (*first == NULL) {
                *first = malloc(sizeof(struct word));
                last = *first;
            } else {
                last->next = malloc(sizeof(struct word));
                last = last->next;
            }
            last->value = symbol;
            last->next = NULL;
        }
    }
    return symbol;
}


void addWord(struct line **first, struct line **last, struct word* element)
{
    if (*first == NULL){
        *first = malloc(sizeof(struct line));
        *last = *first;
    } else {
        (*last)->next = malloc(sizeof(struct line));
        *last = (*last)->next;
    }
    (*last)->value = element;
    (*last)->next = NULL;
}


struct line* readCommand()
{
    struct line *first = NULL, *last = NULL;
    struct word *buffer = NULL;
    int status;
    
    for (;;) {
        status = readWord(&buffer);
        if (buffer != NULL) {
            addWord(&first, &last, buffer);
            buffer = NULL;
        }
        if (ifWordIsReady(status))
            break;
    }
    return first;
}



