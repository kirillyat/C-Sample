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
#include <limits.h>

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
    int errflag;
    int conveyerflag;
    struct word* outfd;
    struct word* infd;
};

struct pidlist {
    int pid;
    struct pidlist* next;
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


void freecommand(struct command* command)
{
    if (command != NULL) {
        freeline(command->first);
        freeword(command->infd);
        freeword(command->outfd);
        free(command);
    }
}

void freepids(struct pidlist* pids)
{
    struct pidlist *fpid = NULL;
    while (pids != NULL) {
        fpid = pids;
        pids = pids->next;
        free(fpid);
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

/*---------------------READING COMMAND PART---------------------*/

int ifWordIsReady(int symbol, int commaFlag)
{
    return ( (symbol == '\n') ||
             (symbol == EOF ) );
}

int ifExtraSymbol(int sumbol){
    return ( (symbol == ' ') ||
             (symbol == '&') ||
             (symbol == '>') ||
             (symbol == '<') ||
             (symbol == '|') );
}

int readWord(struct word** first)
{
    struct word *last = NULL;
    int symbol, commaFlag = 0;
    freeword(*first);
    (*first) = NULL;
    for (;;) {
        symbol = getchar();
        if (ifWordIsReady(symbol) || (ifExtraSymbol(symbol)&&(!commaflag))) {
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

struct command* initCommand()
{
    struct command* input = malloc(sizeof(struct command));
    input->first = NULL;
    input->backgroundflag = 0;
    input->inflag = 0;
    input->outflag = 0;
    input->appendflag = 0;
    input->eofflag = 0;
    input->errflag = 0;
    input->conveyerflag = 0;
    input->outfd = NULL;
    input->infd = NULL;
    return input;
}

int lexicalAnalysis(struct command **input, int status)
{
    int analysis = 0;
    if (((*input)->backgroundflag == 1)&&(status!=' ')&&(status != '\n')){
        (*input)->errflag = 1;
        write(2,"Bad input{&}\n", 13);
    }
    if (status == '&') {
        (*input)->backgroundflag = 1;
    } else if (status == '<') {
        if ((*input)->inflag) {
            (*input)->errflag = 1;
            write(2, "Incorrect input {<}\n", 20);
        }
        else
            (*input)->inflag = 1;
    } else if (status == '>') {
        if ((*input)->outflag) {
            (*input)->errflag = 1;
           
            if ((*input)->errflag == 0)
                write(2, "Incorrect input {> or >>}\n", 26);
        }
        else
            (*input)->outflag = 1;
    } else if (status == '\"') {
        analysis = 1;
        (*input)->errflag = 1;
        write(2, "Incorrect input {commas}\n", 25);
    } else if (status == '\n') {
         analysis = 1;
    } else if (status == '|') {
        (*input)->conveyerflag = 1;
        analysis = 0;
    } else if (status == EOF) {
        analysis = 1;
        if ((*input)->errflag == 0)
            write(2, "\nIncorrect input {EOF}\n", 23);
        (*input)->eofflag = 1;
        
    }
    return analysis;
}

int readLocation(struct command* result, int streem)
{
    int status;
    struct word *buffer = NULL;
    status = readWord(&buffer);
    if (status != ' ') {
        if ((buffer == NULL) && (status == streem) && (status == '>'))
            result->appendflag = 1;
        else if (buffer == NULL) {
            if (result->errflag == 0)
                write(2, "Incorrect input {incorrect location}\n", 37);
            result->errflag = 1;
            return status;
        }
    }
    while (buffer == NULL) {
        status = readWord(&buffer);
        if ((status != ' ') && (buffer == NULL)){
            if (status == EOF)
                result->eofflag = 1;
            if (result->errflag == 0)
                write(2, "Incorrect input {more ten one >}\n", 33);
            result->errflag = 1;
            return status;
        }
    }
    if (streem == '>')
        result->outfd = buffer;
    else if (streem == '<') {
        result->infd = buffer;
    }
    return status;
}

struct command* readCommand()
{
    struct command *result = initCommand();
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
        afterlocation:;
        
        if (status == '|') {
            buffer = malloc(sizeof(struct word));
            buffer->value = status;
            buffer->next = NULL;
            addWord(&first, &last, buffer);
            if (result->first == NULL)
                result->first = first;
            buffer = NULL;
        }
        
        analysis = lexicalAnalysis(&result, status);
        
        if (analysis == 0){
            if ((status == '>') || (status == '<')){
                status = readLocation(result, status);
                goto afterlocation;
             }
        }
        if (analysis == 1)
            break;
    }
    return result;
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

/*---------------------NOT CONVEYER PART (EXEC)---------------------*/

int ifChangeDir(char** command)
{
    return ((command != NULL) &&
            (command[0] != NULL) &&
            (command[0][0] == 'c') &&
            (command[0][1] == 'd') &&
            (command[0][2] == 0) );
}

int openIOfd(struct command* input, int* fd)
{
    char* location = NULL;
    
    if (input->inflag) {
        location = word2string(input->infd);
        fd[0] = open(location, O_RDONLY, 0666);
        if (fd[0] == -1){
            input->errflag = 1;
            perror(location);
            return -1;
        }
        free(location);
    }
    
    if (input->outflag) {
        location = word2string(input->outfd);
        if (input->appendflag)
            fd[1] = open(location, O_WRONLY|O_CREAT|O_APPEND, 0666);
        else
            fd[1] = open(location, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fd[1] == -1){
            input->errflag = 1;
            perror(location);
            return -1;
        }
        free(location);
    }
    return 1;
}


int openIfd(struct command* input, int* fd)
{
    char* location = NULL;
    
    if (input->inflag) {
        location = word2string(input->infd);
        fd[0] = open(location, O_RDONLY, 0666);
        if (fd[0] == -1){
            input->errflag = 1;
            perror(location);
            return -1;
        }
        free(location);
    }
    return 1;
}

int openOfd(struct command* input, int* fd)
{
    char* location = NULL;
    if (input->outflag) {
        location = word2string(input->outfd);
        if (input->appendflag)
            fd[0] = open(location, O_WRONLY|O_CREAT|O_APPEND, 0666);
        else
            fd[0] = open(location, O_WRONLY|O_CREAT|O_TRUNC, 0666);
        if (fd[0] == -1){
            input->errflag = 1;
            perror(location);
            return -1;
        }
        free(location);
    }
    return 1;
}


void сleaningZombieProcesses()
{
    while(wait4(-1, NULL, WNOHANG, NULL) > 0){
    }
}

void executeCommand(struct command *input, int fdin, int fdout)
{
    int rez, p = -1, status, zombiePid;
    char** executeString = list2string(input->first);
    if (ifChangeDir(executeString)) {
            rez = chdir(executeString[1]);
            if (rez == -1)
                perror(executeString[0]);
    } else {
        while (p == -1) {
            p = fork();
            if (p == -1) {
                sleep(1);
                perror("fork");
            }
        }
        
        if (p == 0) {  /* CHILD */
            if (fdin != 0) {
                dup2(fdin, 0);
                close(fdin);
            }
            if (fdout != 1) {
                dup2(fdout, 1);
                close(fdout);
            }
            execvp(executeString[0], executeString);
            perror(executeString[0]);
            exit(1);
        }
        freestring(executeString);
        if (input->backgroundflag != 1) {
            do { /* clean while (pid != p) */
                zombiePid = wait(&status);
            } while (zombiePid != p);
        }
    }
}

/*---------------------CONVEYER PART---------------------*/

int pidsum(struct pidlist* pids)
{
    int sum = 0;
    struct pidlist* tmp = pids;
    while (tmp != NULL) {
        sum += tmp->pid;
        tmp = tmp->next;
    }
    return sum;
}


void zeropid(struct pidlist* pids, int pid)
{
    struct pidlist* tmp = pids;
    while (tmp != NULL) {
        if (tmp->pid == pid)
            tmp->pid = 0;
        tmp = tmp->next;
    }
}

void waitpidlist(struct pidlist* pids)
{
    int status, zombie;
    while (pidsum(pids) != 0) {
        zombie = wait(&status);
        zeropid(pids, zombie);
    }
}

void addPid(struct pidlist** pids, int new)
{
    struct pidlist* tmp = malloc(sizeof(struct pidlist));
    tmp->pid = new;
    tmp->next = *pids;
    *pids = tmp;
}

int ifpipe(struct line* input)
{
    return ((input != NULL) &&
            (input->value->value == '|') &&
            (input->value->next == NULL));
}

char** readConveyerCommand(struct command *conveyerProgs)
{
    struct line *rezcmd = conveyerProgs->first, *tmp = NULL;
    struct line* prev = NULL;
    char** result = NULL;
    
    while ((conveyerProgs->first != NULL) && (ifpipe(conveyerProgs->first) != 1)) {
        prev = conveyerProgs->first;
        conveyerProgs->first = (conveyerProgs->first)->next;
    }
    
    if (conveyerProgs->first != NULL) {
        prev->next = NULL;
        tmp = conveyerProgs->first;
        conveyerProgs->first = (conveyerProgs->first)->next;
        result = list2string(rezcmd);
        freeline(rezcmd);
        free(tmp);
    } else {
        result = list2string(rezcmd);
        freeline(rezcmd);
    }
    return result;
}

void conveyer(struct command **conveyerProgs, int fdin, struct pidlist** pids)
{
    int p, fdout = 1, fd[] = {0,1};
    char** executeString = NULL;
    executeString = readConveyerCommand(*conveyerProgs);
    if ((*conveyerProgs)->first == NULL) {
        if ((*conveyerProgs)->outflag == 1){
            openOfd(*conveyerProgs, &fdout);
        } else
            fdout = 1;
    } else {
        pipe(fd);
        fdout = fd[1];
    }
    p = fork();
    if (p != 0) {
        addPid(pids, p);
    } else {
        if (fd[0] != 0) {
            close(fd[0]);
        }
        if (fdin != 0) {
            dup2(fdin, 0);
            close(fdin);
        }
        if (fdout != 1) {
            dup2(fdout, 1);
            close(fdout);
        }
        execvp(executeString[0], executeString);
        perror(executeString[0]);
        exit(1);
    }
    freestring(executeString);
    executeString = NULL;
    
    if((*conveyerProgs)->first != NULL){
        if (fdin != 0)
            close(fdin);
        if (fd[1] != 1)
            close(fd[1]);
        conveyer(conveyerProgs, fd[0], pids);
        if (fd[0] != 0)
            close(fd[0]);
       
    } else {
        if (fd[0] != 0)
            close(fd[0]);
        if (fd[1] != 1)
            close(fd[1]);
        if (fdin != 0)
            close(fdin);
        if (fdout != 1)
            close(fdout);
    }
}

void printwelcome()
{
   char PathName[PATH_MAX];
   char PN;
   PN = (char)getwd(PathName);
   if (PN == (char)NULL)
       printf ("error");
   else
       printf ("[%s] > ",PathName);

}

int main(int argc, const char * argv[])
{
    struct command *input = NULL;
    for (;;) {
        printwelcome();
        input = readCommand();
        сleaningZombieProcesses();
        if (input == NULL){
            continue;
        } else if (input->eofflag) {
            freecommand(input);
            break;
        } else if (input->errflag) {
            freecommand(input);
            continue;
        } else {
            if (input->conveyerflag) {
                struct pidlist* pids = NULL;
                int fdin = 0;
                if ((openIfd(input, &fdin) == -1) || (fdin == -1))
                    fdin = 0;
                conveyer(&input, fdin, &pids);
                if (input->backgroundflag == 0)
                    waitpidlist(pids);
                freepids(pids);
                freecommand(input);
                
                input = NULL;
            } else {
                int fd[] = {0, 1};
                openIOfd(input, fd);
                executeCommand(input, fd[0], fd[1]);
                if (fd[0] != 0)
                    close(fd[0]);
                if (fd[1] != 1)
                    close(fd[1]);
                freecommand(input);
                input = NULL;
            }
        }
    }
    return 0;
}


