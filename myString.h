//
//  myString.h
//
//
//  Created by kirillyat on 14.11.2019.
//
//

#ifndef myString_h
#define myString_h

#include <stdio.h>
#include <stdlib.h>

struct word {
    int value;
    struct word *next;
};

struct line {
    struct word *value;
    struct line *next;
};

void freeword(struct word *inputword);
void freeline(struct line* inputline);
void freestring(char **input);

void printword(struct word* inputword);
void printline(struct line* inputline);
void printstring(char* const *output);

int wordlen(struct word *inputword);
int linelen(struct line *inputline);

char* word2string (struct word* list);
char** list2string (struct line* list);

int ifWordIsReady(int symbol);
int readWord(struct word** first);
void addWord(struct line **first, struct line **last, struct word* element);
struct line* readCommand(void);

#endif /* myString_h */
