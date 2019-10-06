//
//  shell
//

#include <stdio.h>
#include <stdlib.h>


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
        printf(" ");
        printline(inputline->next);
    }
}

struct word* readWord(){
    struct word *first = NULL, *last = NULL, *extra = NULL;
    int symbol, commaFlag = 0;
    for (;;) {
        symbol = getchar();
        if (((symbol == ' ') && (!commaFlag)) || (symbol == '\n') || (symbol == EOF)){
            if ((symbol == '\n') && (commaFlag)){
                freeword(first);
                first = NULL;
                printf("error : commas balanse\n");
            } else if ((symbol == EOF) && (first != NULL)) {
                printf("error: eof \n");
                freeword(first);
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
        } else if (buffer->value == EOF) {
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
            nextline:
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

void shell()
{
    struct line * input = NULL;
    for (;;) {
        printf(">>>");
        input = readCommand();
        if (input == NULL)
            continue;
        else if (input->value->value == EOF){
            freeline(input);
            break;
        } else {
            printline(input);
            printf("\n");
            freeline(input);
        }
    }
}


int main(int argc, const char * argv[]) {
    shell();
    return 0;
}
