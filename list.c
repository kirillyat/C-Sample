#include <stdio.h>

struct item{
    int value;
    struct item *next;
};

void printList(struct item* list)
{
    if (list != NULL){
        printf("%d ", list->value);
        printList(list->next);
    }
}

struct item* vec2list(const int* vec, int sz)
{
    int i;
    struct item* last;
    struct item* first= NULL;

    for(i = 0; i < sz; i++){
        if (i == 0){
            first = malloc(sizeof(struct item));
            last = first;
        } else {
            last->next = malloc(sizeof(struct item));
            last = last->next;
        }

        last->value = vec[i];
        last->next = NULL;
    }

    return first;
}

int main() {
    int V[] = {0,1,2,3,4,5,5,43,444};
    printList(vec2list(&V, sizeof(V)/4));
    return 0;
}
