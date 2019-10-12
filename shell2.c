//
//  shell.c
//  kirillyat
//  MSU CMC 2019
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

struct word* readWord()
{
    struct word *first = NULL, *last = NULL, *extra = NULL;
    int symbol, commaFlag = 0;
    for (;;) {
        symbol = getchar();
        if ( ((symbol == ' ') && (!commaFlag)) || (symbol == '\n') || (symbol == EOF) ){
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

struct line* readCommand()
{
    struct line *first = NULL, *last = NULL;
    struct word *buffer;
    for (;;) {
        buffer = readWord();
        if (buffer == NULL) {
            continue;
        } else if (buffer->value == '\"') {
            printf("\nError: commas balanse!\n");
            freeword(buffer);
            freeline(first);
            first = NULL;
            break;
        } else if (buffer->value == EOF) {
            if ((buffer->next != NULL) || (first != NULL)){
                printf("\nError: eof!\n");
                freeword(buffer->next);
                buffer->next = NULL;
            }
            freeline(first);
            first = malloc(sizeof(struct line));
            first->value = buffer;
            first->next = NULL;
            break;
        } else if ((buffer->value == ' ') || (buffer->value == '\n')) {
            if (buffer->next == NULL)
                goto nextline;
            if (first == NULL){
                first = malloc(sizeof(struct line));
                last = first;
            } else {
                last->next = malloc(sizeof(struct line));
                last = last->next;
            }
            last->value = buffer->next;
            last->next = NULL;
            nextline: /* NEXT STEP LABEL */
            if (buffer->value == ' '){
                free(buffer);
                continue;
            } else {
                free(buffer);
                break;
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

void executeCommand(char** command)
{
    int rez, p = fork();
    if (p == 0) { /* CHILD */
        if (ifChangeDir(command)) {
            rez = chdir(command[1]);
            if (rez == -1)
                perror(command[1]);
        } else {
            execvp(command[0], command);
            perror(command[0]);
            exit(1); }
    }
    wait(NULL);
}


void shell()
{
    struct line * input = NULL;
    for (;;) {
        printf(">>> ");
        input = readCommand();
        if (input == NULL)
            continue;
        else if ((input->value)->value == EOF) {
            freeline(input);
            break;
        } else {
            //printline(input);
            //printf("\n");
            char **command = list2string(input);
            printstring(command);
            executeCommand(command);
            freestring(command);
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
