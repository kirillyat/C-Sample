//
//  shell.c
//  kirillyat
//  MSU CMC 2019
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


struct word{
    int value;
    struct word *next;
};

struct line{
    struct word *value;
    struct line *next;
};

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
    struct  line *fline;
    while (inputline != NULL) {
        fline = inputline;
        inputline = inputline->next;
        freeword(fline->value);
        free(fline);
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

int ifWordIsReady(int symbol, int commaFlag)
{
    return ( ((symbol == ' ') && (!commaFlag)) || (symbol == '\n') || (symbol == EOF) || ((symbol == '&') && (!commaFlag)) );
}

struct word* readWord()
{
    struct word *first = NULL, *last = NULL, *extra = NULL;
    int symbol, commaFlag = 0;
    for (;;) {
        symbol = getchar();
        if (ifWordIsReady(symbol, commaFlag)) {
            if ((symbol == '\n') && (commaFlag)){
                symbol = '\"';
            }
            extra = malloc(sizeof(struct word));
            extra->value = symbol;
            extra->next = first;
            first = extra;
            break;
        } else if (symbol == '\"') {
            commaFlag = (commaFlag == 1)?0:1;
            continue;
        } else {
            if (first == NULL) {
                first = malloc(sizeof(struct word));
                last = first;
            } else {
                last->next = malloc(sizeof(struct word));
                last = last->next;
            }
            last->value = symbol;
            last->next = NULL;
        }
    }
    return first;
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
    struct word *buffer;
    for (;;) {
        buffer = readWord();
        if (buffer == NULL) {
            continue;
        } else if (buffer->value == '\"') {
            printf("\nerror: incorrect input {commas}\n");
            freeword(buffer);
            freeline(first);
            first = NULL;
            break;
        } else if (buffer->value == EOF) {
            if ((buffer->next != NULL) || (first != NULL)){
                printf("\nerror: incorrect input {EOF}\n");
                freeword(buffer->next);
                buffer->next = NULL;
            }
            freeline(first);
            addWord(&first, &last, buffer);
            break;
        } else if ((buffer->value == ' ') || (buffer->value == '\n') || (buffer->value == '&')) {
            if (buffer->next != NULL)
                addWord(&first, &last, buffer->next);
        
            if (buffer->value == ' '){
                free(buffer);
                continue;
            } else if (buffer->value == '\n') {
                free(buffer);
                break;
            } else if (buffer->value == '&'){
                buffer->next = NULL;
                addWord(&first, &last, buffer);
                continue;
            }
        }
    }
    return first;
}

void freestring(char **input)
{
    int i;
    for(i = 0; input[i] != NULL; i++){
        free(input[i]);
    }
    free(input[i]);
    free(input);
}

void printstring( char* const *output)
{
    int i;
    for(i = 0; output[i] != NULL; i++)
        printf("%s ", output[i]);
    printf("\n");
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

int ifChangeDir(char** command)
{
    return ((command[0][0] == 'c') && (command[0][1] == 'd') && (command[0][2] == 0));
}

int ifBackground(struct line **command)
{
    int rez = 0;
    struct line *last = *command, *prev = *command;
    while (last != NULL) {
        struct word* tmp = last->value;
        if ((tmp->value == '&') && (tmp->next == NULL)){
            if ((rez == 0) && (last->next == NULL)) {
                rez = '&';
                free(prev->next);
                prev->next = NULL;
            } else {
                rez = -1;
                break;
            }
        }
        prev = last;
        last = last->next;
    }
    return rez;
}

void сleaningZombieProcesses()
{
    while(wait4(-1, NULL, WNOHANG, NULL) > 0){
    }
}


void executeCommand(char** command, int mode)
{
    int rez, p, status, zombiePid;
    сleaningZombieProcesses();
    if (ifChangeDir(command)) {
            rez = chdir(command[1]);
            if (rez == -1)
                perror(command[0]);
    } else {
        p = fork();
        if (p == 0) {  /* CHILD */
            execvp(command[0], command);
            perror(command[0]);
            exit(1);
        }
        if (mode != '&')
            do {
                zombiePid = wait(&status);//clean while pid !=p
            }while (zombiePid != p);
    }
}



void shell()
{
    struct line * input = NULL;
    char **command;
    for (;;) {
        printf(">>> ");
        input = readCommand();
        if (input == NULL)
            continue;
        else if ((input->value)->value == EOF) {
            freeline(input);
            break;
        } else {
            int mode = ifBackground(&input);
            if (mode != -1){
                command = list2string(input);
                printstring(command);
                executeCommand(command, mode);
                freestring(command);
            } else {
                printf("error: incorrect input {&}");
            }
            freeline(input);
            input = NULL;
        }
    }
}


int main(int argc, const char * argv[])
{
    shell();
    return 0;
}
