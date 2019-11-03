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
#include <fcntl.h>

struct word {
    int value;
    struct word *next;
};

struct line {
    struct word *value;
    struct line *next;
};

struct command {
    struct line *first;
    int backgroundflag;
    int inflag;
    int outflag;
    int appendflag;
    int eofflag;
    struct word* outfd;
    struct word* infd;
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


void freecommand(struct command* command)
{
    if (command != NULL) {
        freeline(command->first);
        freeword(command->infd);
        freeword(command->outfd);
        free(command);
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

int ifWordIsReady(int symbol, int commaFlag)
{
    return ( (symbol == '\n') ||
             (symbol == EOF ) ||
             ((symbol == ' ') && (!commaFlag)) ||
             ((symbol == '&') && (!commaFlag)) ||
             ((symbol == '>') && (!commaFlag)) ||
             ((symbol == '<') && (!commaFlag)) ||
             ((symbol == '|') && (!commaFlag)) );
}

int readWord(struct word** first)
{
    freeword(*first);
    (*first) = NULL;
    struct word *last = NULL;
    int symbol, commaFlag = 0;
    for (;;) {
        symbol = getchar();
        if (ifWordIsReady(symbol, commaFlag)) {
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

void initCommand(struct command **input)
{
    if (*input == NULL){
        (*input) = malloc(sizeof(struct command));
        (*input)->first = NULL;
        (*input)->backgroundflag = 0;
        (*input)->inflag = 0;
        (*input)->outflag = 0;
        (*input)->appendflag = 0;
        (*input)->eofflag = 0;
        (*input)->outfd = NULL;
        (*input)->infd = NULL;
    }
}

int lexicalAnalysis(struct command **input, int status)
{
    int analysis = 0;
    
    if ((*input)->backgroundflag == 1)
        analysis = -1;
    
    if (status == '&') {
        (*input)->backgroundflag = 1;
    } else if (status == '<') {
        
        if ((*input)->inflag) {
            analysis = -1;
            printf("\nError: incorrect input {<}\n");
        }
        else
            (*input)->inflag = 1;
        
    } else if (status == '>') {
        if ((*input)->outflag) {
            analysis = -1;
            printf("\nError: incorrect input {> or >>}\n");
        }
        else
            (*input)->outflag = 1;
        
    } else if (status == '\"') {
        analysis = -1;
        printf("\nError: incorrect input {commas}\n");
    } else if (status == '\n') {
         analysis = 1;
    } else if (status == EOF) {
        (*input)->eofflag = 1;
        analysis = 1;
        if ((*input)->first != NULL) {
            printf("\nError: incorrect input {EOF}\n");
            analysis = 0;
        }
    }
    return analysis;
}

int readLocation(struct command* result, int streem)
{
    int sucsess = 1, status;
    struct word *buffer = NULL;
    status = readWord(&buffer);
    if (status != ' ') {
        if ((buffer == NULL) && (status == streem) && (status == '>'))
            result->appendflag = 1;
        else
            return !sucsess;
    }
    
    
    while (buffer == NULL) {
        status = readWord(&buffer);
        if ((status != ' ') && (buffer == NULL)){
            if (status == EOF)
                result->eofflag = 1;
            return !sucsess;
        }
        
    }
       

    if (streem == '>')
        result->outfd = buffer;
    else if (streem == '<') {
        result->infd = buffer;
    }
    lexicalAnalysis(&result, status);
    return sucsess;
}

struct command* readCommand()
{
    struct command *result = NULL;
    initCommand(&result);
    struct line *first = NULL, *last = NULL;
    struct word *buffer = NULL;
    int status, analysis;
    
    for (;;) {
        status = readWord(&buffer);
        
        if (buffer != NULL) {
            addWord(&first, &last, buffer);
            if (result->first == NULL)
                result->first = first;
            buffer = NULL;
        }
        
        analysis = lexicalAnalysis(&result, status);
        
        if (analysis == 0){
            if ((status == '>') || (status == '<'))
                analysis = readLocation(result, status);
        }
        
        if (analysis == -1) {
            freecommand(result);
            result = NULL;
            break;
        } else if (analysis == 1)
            break;
        
        
    }
    return result;
}

char* word2string (struct word* list){
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

int ifChangeDir(char** command)
{
    return ((command != NULL) &&
            (command[0] != NULL) &&
            (command[0][0] == 'c') &&
            (command[0][1] == 'd') &&
            (command[0][2] == 0) );
}

int openIOfd(struct command* input)
{
    int fd;
    char* location = NULL;
    
    if (input->inflag) {
        location = word2string(input->infd);
        fd = open(location, O_RDONLY, 0666);
        
        if (fd == -1)
            perror(location);
        else {
            dup2(fd, 0);
            close(fd);
        }
        free(location);
    }
    
    if (input->outflag) {
        location = word2string(input->outfd);
        if (input->appendflag)
            fd = open(location, O_WRONLY|O_CREAT|O_APPEND, 0666);
        else
            fd = open(location, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fd == -1)
            perror(location);
        else {
            dup2(fd, 1);
            close(fd);
        }
        free(location);
    }
    
    return 0;
}

void сleaningZombieProcesses()
{
    while(wait4(-1, NULL, WNOHANG, NULL) > 0){
    }
}

void executeCommand(char **executeString, struct command *input)
{
    int rez, p, status, zombiePid;
    сleaningZombieProcesses();
    if (ifChangeDir(executeString)) {
            rez = chdir(executeString[1]);
            if (rez == -1)
                perror(executeString[0]);
    } else {
        p = fork();
        if (p == 0) {  /* CHILD */
            openIOfd(input);
            execvp(executeString[0], executeString);
            perror(executeString[0]);
            exit(1);
        }
        if (!input->backgroundflag)
            do { /* clean while (pid != p) */
                zombiePid = wait(&status);
            } while (zombiePid != p);
    }
}




void shell()
{
    struct command *input = NULL;
    char** executeString = NULL;
    for (;;) {
        printf(">>> ");
        input = readCommand();
        if (input == NULL)
            continue;
        else if (input->eofflag) {
            freecommand(input);
            break;
        } else {
            printline(input->first);
            printf("\n");
            executeString = list2string(input->first);
            executeCommand(executeString, input);
            
            freestring(executeString);
            executeString = NULL;
            freecommand(input);
            input = NULL;
        }
    }
}


int main(int argc, const char * argv[])
{
    shell();
    return 0;
}

